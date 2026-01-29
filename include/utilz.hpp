/*
 * Copyright (C) 2025 M. Wykpis, A. Szwaja, P. Kubicki, S. Szulc, K. Socha
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef UTILZ
#define UTILZ
#include <string>

std::string getOutputPath();
void deleteContentOfDir(const std::string &path);
void createStyles(const std::string &path);
void createIndex(const std::string &path);
std::string buildCommand(char* program, char* argv[], bool extractInner);

#endif // UTILZ