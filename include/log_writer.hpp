/*
 * Copyright (C) 2025 M. Wykpis, A. Szwaja, P. Kubicki, S. Szulc, K. Socha
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef LOG_WRITER_HPP
#define LOG_WRITER_HPP

#include <chrono>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

void writeHeader(ofstream& file_strem, string program_name, string filename, string parent_pid, string parent_filename = "");

void writeLine(ofstream& file_stream, string line, string time_str, bool is_error);

void writeLink(ofstream& result, string time_str, string pid, string name, string file_name);

void registerLink(string time_str, string pid, string name, string file_name);

void createJsFile(pid_t pid, int exit_code);


#endif // LOG_WRITER_HPP
