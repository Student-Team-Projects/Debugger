#include <chrono>
#include <fstream>
#include <map>

#include "const.hpp"

using namespace std;

void writeHeader(ofstream &result, string program_name)
{
    auto now = chrono::system_clock::now();
    auto time = chrono::system_clock::to_time_t(now);
    result << R"(<!DOCTYPE html>
<html lang="pl-PL">
<head>
<meta charset="UTF-8" />
<link rel="stylesheet" href="styles.css" />
<title>)";
    result << program_name << "\n";
    result << R"(</title>
</head>
<body>
<div class="head">
<div class="info">
<span class="info-title">command:</span> <span class="info-value info-value-path">)" +
                  program_name + R"(</span>
</div>
<div class="info">
<span class="info-title">start time:</span> <span class="info-value">)";
    result << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
    result << R"(</span>
</div>
<div class="info">
<span class="info-title">last entry:</span> <span class="info-value"></span>
</div>
</div>
<div class="line"></div>
<div class="entries">
<table>
<tboby>
)";
    result.flush();
}

void writeLine(ofstream &result, string line, string timeStr, bool isError)
{
    long long milliseconds = stoll(timeStr);
    auto duration = chrono::milliseconds(milliseconds);
    auto time_point = chrono::system_clock::time_point(duration);
    time_t time = chrono::system_clock::to_time_t(time_point);

    result << "<tr><td class=\"entry-time\">";
    result << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
    if (isError)
        result << "</td><td>&nbsp;</td><td class=\"entry-log entry-stderr\">";
    else
        result << "</td><td>&nbsp;</td><td class=\"entry-log entry-stdout\">";
    result << line;
    result << "</td></tr>\n";
    result.flush();
}

void writeLink(ofstream &result, string timeStr, string pid, string name)
{
    long long milliseconds = stoll(timeStr);
    auto duration = chrono::milliseconds(milliseconds);
    auto time_point = chrono::system_clock::time_point(duration);
    time_t time = chrono::system_clock::to_time_t(time_point);

    result << "<tr><td class=\"entry-time\">";
    result << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
    result << "</td><td>&nbsp;</td><td class=\"entry-log entry-link\"><a href=\"result_" << pid << ".html\">" << name << " </a></td></tr>\n";
}

void writePackage(map<string, ofstream> &pidMap, char *buf, bool isError)
{
    istringstream stream(buf);
    string line;
    string pid, ppid, name, time;

    while (getline(stream, line))
    {
        if (line == BEGIN_OUT)
        {
            ppid = pid;
            getline(stream, pid);
            getline(stream, name);
            getline(stream, time);
            continue;
        }
        if (line == END_OUT)
            continue;

        //cout<<line<<endl;

        if (pidMap.find(pid) == pidMap.end())
        {
            pidMap[pid].open("result_" + pid + ".html", ios::out | ios::trunc);
            writeHeader(pidMap[pid], name);
            writeLink(pidMap[ppid], time, pid, name);
        }
        ofstream &s = pidMap[pid];
        writeLine(s, line, time, isError);
    }
}
