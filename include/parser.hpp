/*
 * Copyright (C) 2025 M. Wykpis, A. Szwaja, P. Kubicki, S. Szulc, K. Socha
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <chrono>
#include <fstream>
#include <map>

#include "const.hpp"
#include "log_writer.hpp"

using namespace std;

void parse_buffer(map<string, ofstream>& pidMap, map<string, string>& dataMap, map<string, string>& commandMap, char* buf, bool isError, int end);

string get_file_name(string time, string line, string pid);

#endif // PARSER_HPP
