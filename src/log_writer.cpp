/*
 * Copyright (C) 2025 M. Wykpis, A. Szwaja, P. Kubicki, S. Szulc, K. Socha
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <iomanip>
#include <pwd.h>
#include <unistd.h>
#include <fstream>
#include <climits> 

#include "const.hpp"
#include "utilz.hpp"
#include "log_writer.hpp"
#include "colors.hpp"

using namespace std;

string getAccount() {
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    struct passwd *pw = getpwuid(getuid());
    return string(pw ? pw->pw_name : "unknown") + "@" + string(hostname);
}

string getPath() {
    char cwd[PATH_MAX];
    return (getcwd(cwd, sizeof(cwd)) != NULL) ? string(cwd) : "";
}

void writeHeader(ofstream& result, string program_name, string pid, string parent_pid, string parent_filename, string parent_command) {
    auto now = chrono::system_clock::now();
    auto time = chrono::system_clock::to_time_t(now);
    
    string account_info = getAccount();
    string start_path = getPath();

    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    struct passwd *pw = getpwuid(getuid());
    string user_name = (pw) ? pw->pw_name : "unknown";
    string user_index_link = "../" + string(hostname) + "/" + user_name + "/index.html";

    result << R"(
<!DOCTYPE html>
<html lang="pl-PL">
<head>
<meta charset="UTF-8" />
<link rel="stylesheet" href="styles.css" />
<title>)" << program_name << "\n"<< R"(
</title>
</head>
<body>
<div class="head">
)";

    if (parent_pid != "0" && !parent_filename.empty()) {
        result << R"(
<div class="info">
<span class="info-title">from account:</span>
<span class="info-value">)" << account_info << R"(</span>
</div>
<div class="info">
<span class="info-title">from command:</span>
<span class="info-value info-value-path"><a href=")" << parent_filename.substr(1) << R"(">)" << parent_command << R"(</a></span>
</div>
)";
    }

    result << R"(
<div class="info">
<span class="info-title">account:</span>
<span class="info-value"><a href=")" << user_index_link << R"(">)" << account_info << R"(</a></span>
</div>
<div class="info">
<span class="info-title">start path:</span>
<span class="info-value">)" << start_path << R"(</span>
</div>
<div class="info">
<span class="info-title">command:</span>
<span class="info-value info-value-path">)" << program_name << R"(</span>
</div>
<div class="info">
<span class="info-title">parent process:</span>
<span class="info-value">)";

    if (parent_pid != "0" && !parent_filename.empty()) {
        result << R"(<a href=")" << parent_filename.substr(1) << R"(">)" << parent_pid << R"(</a>)";
    } else {
        result << parent_pid;
    }
    
    result << R"(</span>
</div>
<div class="info">
<span class="info-title">start time:</span>
<span class="info-value">)" << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S") << R"(</span>
</div>
<div class="info">
<span class="info-title">last entry:</span> <span class="info-value" id="last_entry"></span>
</div>
<div class="info">
<span class="info-title"><span class="exitno" id="exit_code_wrapper">exit <span id="exit_code"></span textContent="?"></span></span>
</div>
<script src="./)" << pid << R"(.js"></script>
<div class="line"></div>
<div class="entries">
<table>
<tboby>
)";
    result.flush();
}

setting current_setting_normal = {}, current_setting_error = {
        {red, unset}
};

void writeLine(ofstream& result, string line, string timeStr, bool isError) {
    long long milliseconds = stoll(timeStr);
    auto duration = chrono::milliseconds(milliseconds);
    auto time_point = chrono::system_clock::time_point(duration);
    time_t time = chrono::system_clock::to_time_t(time_point);

    result << "<tr><td class=\"entry-time\">";
    result << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
    result << "</td><td><td>&nbsp;</td><td><table style=\"border-collapse:collapse; border-spacing:0\">";

    auto params = parse_params(line);
    setting& current_setting = (!isError) ? current_setting_normal : current_setting_error;
    std::string cat;
    if(params.empty()) {
        cat += get_html(line, current_setting);
    } else {
        int i = 0;
        for (const tuple<int, int, vector<int>>& entry:params) {
            int j = get<0>(entry);
            int k = get<1>(entry);
            std::string substr = line.substr(i, j - i);
            if(!substr.empty()) cat += get_html(substr, current_setting);
            current_setting = get_settings(get<2>(entry));
            i = k;
        }
        size_t n = line.length();
        std::string substr = line.substr(i, n - i);
        if(!substr.empty())
        cat += get_html(substr, current_setting);
    }
    result << cat;
    result << "</table></td></tr>\n";
    result.flush();
}

void writeLink(ofstream& result, string timeStr, string /*pid*/, string name, string file_name) {
    long long milliseconds = stoll(timeStr);
    auto duration = chrono::milliseconds(milliseconds);
    auto time_point = chrono::system_clock::time_point(duration);
    time_t time = chrono::system_clock::to_time_t(time_point);

    string debugger_path = getOutputPath();

    result << "<tr><td class=\"entry-time\">";
    result << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
    result << "</td><td>&nbsp;</td><td class=\"entry-log entry-link\"><a href=" << debugger_path + "/all_logs" + file_name << ">" << name << " </a></td></tr>\n";
}

void registerLink(string timeStr, string /*pid*/, string name, string file_name) {
    long long milliseconds = stoll(timeStr);
    auto duration = chrono::milliseconds(milliseconds);
    auto time_point = chrono::system_clock::time_point(duration);
    time_t time = chrono::system_clock::to_time_t(time_point);

    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    struct passwd *pw = getpwuid(getuid());
    // char* user_name = pw->pw_name; 

    string debugger_path = getOutputPath();

    ofstream res(debugger_path + "/" + hostname + "/" + pw->pw_name  + "/index.html", std::ios::app);
    res << "<tr><td class=\"entry-time\">";
    res << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
    res << "</td><td>&nbsp;</td><td class=\"entry-log entry-link\"><a href=" << debugger_path + "/all_logs" + file_name << ">" << name << " </a></td></tr>\n";
    res.flush();
    res.close();
}

void createJsFile(pid_t pid, int exit_code) {
    string debugger_path = getOutputPath();
    string path = debugger_path + "/all_logs/" + to_string(pid) + ".js";

    auto now = chrono::system_clock::now(); 
    auto time = chrono::system_clock::to_time_t(now);

    fstream js_file;
    js_file.open(path, ios::out | ios::trunc);
    js_file << R"(document.getElementById("exit_code").textContent=")" << exit_code << R"(";)"
            << R"(document.getElementById("last_entry").textContent=")" << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S") << R"(";)";
            
    if (exit_code) js_file << R"(document.getElementById("exit_code_wrapper").className="exiterr")";
    else js_file << R"(document.getElementById("exit_code_wrapper").className="exitok")";
}