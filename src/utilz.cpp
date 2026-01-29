/*
 * Copyright (C) 2025 M. Wykpis, A. Szwaja, P. Kubicki, S. Szulc, K. Socha
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <iostream>

#include "utilz.hpp"
#include "const.hpp"
#include <pwd.h>
#include <sstream>

std::string getOutputPath() {
  char* path = getenv(ENV_PATH);
  if (!path) {
    struct passwd* pw = getpwuid(getuid());
    char* dir = pw->pw_dir;
    return std::string(dir) + "/debugger_logs";
  }
  return std::string(path) + "/debugger_logs";
}

void generateIndexHtml(std::string path, std::string styles_path, std::string message);
void addLink(std::string index_path, std::string self_path, std::string name);

void deleteContentOfDir(const std::string& path) {
  DIR* dir = opendir(path.c_str());

  struct dirent* entry;
  struct stat fileStat;

  while ((entry = readdir(dir)) != nullptr) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    std::string fullPath = path + "/" + entry->d_name;

    if (stat(fullPath.c_str(), &fileStat) == 0) {
      if (S_ISDIR(fileStat.st_mode)) {
        deleteContentOfDir(fullPath);
        rmdir(fullPath.c_str());
      } else {
        remove(fullPath.c_str());
      }
    }
  }
  closedir(dir);
}

void createStyles(const std::string& path) {
  std::ofstream styles(path + "/all_logs/styles.css");
  styles << R"(body {
  background-color: #272822;
  margin: 0;
  padding: 0
}

table {
	font-size: 14px;
}

a:link {
  color: #9270ff;
  text-decoration: none;
}

a:visited {
  color: #7178ff;
  text-decoration: none;
}

div.line {
  border-bottom: 2px solid #00AAAA;
  position: absolute;
  width: 100%;
}

div.head {
  margin: 10px;
  padding: 0
}

div.info {
	font-family: "Helvetica Neue", Helvetica, Arial, sans-serif;
	font-size: 15px;
	font-style: normal;
	font-variant: normal;
	font-weight: 500;
	line-height: 22px;
}

span.info-title {
  font-style: italic;
  color: #00AAAA;
}

span.info-value {
  color:  #F8F8F2;
	 font-size: 15px;
}

a.info-value {
 color: #F8F8F2;
}

span.info-value-path {
	font-family: monospace;
	font-size: 15px;
	font-style: normal;
	font-variant: normal;
	font-weight: 100;
	line-height: 18px;
}

a.info-value-fromcall-ok {
  color: #00AA00;
}

a.info-value-fromcall-err {
  color: #BB1111
}

a.info-value-fromcall-wait {
  color: #BBBB11
}

span.exitok {
	color: #00AA00;
}

span.exiterr {
	color: #CC1111
}

span.exitno {
	color: #AAAA00;
}

div.headder {
	color:  #F8F8F2;
	 font-size: 25px;
}

div.entries {
	padding: 10px;
	font-family: monospace;
	font-size: 14px;
	font-style: normal;
	font-variant: normal;
	font-weight: 500;
	line-height: 18px;
}

table {
  border-collapse: collapse;
}

td.entry-time {
	font-family: "Helvetica Neue", Helvetica, Arial, sans-serif;
	font-size: 14px;
	font-style: normal;
	font-variant: normal;
	font-weight: 500;
	line-height: 18px;
	font-style: italic;
	color: #B8B8B2;
  text-align: right;
  width: 150px;
  display: inline-block;
}

td.entry-log {
  border-left: 1px solid #B8B8B2;
  padding-left: 10px;
}

td.entry-stdout {
  color:  #F8F8F2;
}

td.entry-stderr {
  color:  #CC5500;
}

td.entry-call-wait {
  color:  #BBBB11;
}

td.entry-call-ok {
  color:  #00AA00;
}

td.entry-call-err {
  color:  #BB1111;
}

a.entry-call {
	overflow: hidden;
	white-space: nowrap;
}

a.entry-call-ok {
  color: #00AA00;
}

a.entry-call-err {
  color: #BB1111
}

a.entry-call-wait {
  color: #BBBB11
}

span.entry-exit {
	font-family: "Helvetica Neue", Helvetica, Arial, sans-serif;
	font-size: 15px;
	font-style: normal;
	font-variant: normal;
	font-weight: 500;
	line-height: 22px;
	font-style: italic;
	padding-right: 0px;
})";

  styles.close();
}


void createIndex(const std::string& path) {
  std::ifstream servers_file_check(path + "/index.html");
  if (servers_file_check.good()) return;

  generateIndexHtml(path, "all_logs/styles.css", "list of servers");

  char hostname[256];
  gethostname(hostname, sizeof(hostname));
  std::string server_path = path + "/" + hostname;
  mkdir(server_path.c_str(), 0777);

  std::ifstream users_file_check(server_path + "/index.html");
  if (users_file_check.good()) return;

  generateIndexHtml(server_path, "../all_logs/styles.css", "list of users");

  addLink(path, server_path, hostname);

  struct passwd* pw = getpwuid(getuid());
  char* user_name = pw->pw_name;
  std::string user_path = server_path + "/" + std::string(user_name);

  mkdir(user_path.c_str(), 0777);

  std::ifstream file_check(user_path + "/index.html");
  if (file_check.good()) return;

  generateIndexHtml(user_path, "../../all_logs/styles.css", "list of debugged programs");
  addLink(server_path, user_path, user_name);
}


void generateIndexHtml(std::string path, std::string styles_path, std::string message) {
  std::ofstream index(path + "/index.html");
  index << R"(<!DOCTYPE html>
<html lang="pl-PL">
<head>
<meta charset="UTF-8" />
<link rel="stylesheet" href=)" << styles_path << R"( />
<title>test/test-basic.sh
</title>
</head>
<body>
<div class="head">
<div class="headder"> DEBUGGER </div>
<div class="info">
<span class="info-title">)" << message << R"(</span>
</div>
<div class="line"></div>
<div class="entries">
<table>
<tboby>)";

  index.close();
}

void addLink(std::string index_path, std::string self_path, std::string name) {
  std::ofstream res(index_path + "/index.html", std::ios::app);
  res << "<td class=\"entry-log entry-link\"><a href=" << self_path + "/index.html" << ">" << name << " </a></td></tr>\n";
  res.flush();
  res.close();
}

std::string buildCommand(char* program, char* argv[])
{
  if (!program || !argv)
    return "";

  std::string prog(program);


  std::ostringstream cmd;
  for (int i = 0; argv[i]; ++i) {
    if (i > 0)
      cmd << ' ';

    if (std::strchr(argv[i], ' ')) {
      cmd << '"' << argv[i] << '"';
    } else {
      cmd << argv[i];
    }
  }

  return cmd.str();
}
