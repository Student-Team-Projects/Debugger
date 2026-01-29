/*
 * Copyright (C) 2025 M. Wykpis, A. Szwaja, P. Kubicki, S. Szulc, K. Socha
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef PACKAGE_HEADER_HPP
#define PACKAGE_HEADER_HPP

#include <unistd.h>
#include "const.hpp"

struct package_header {
    unsigned int header_key;
    unsigned char type;
    pid_t parent_pid;
    pid_t pid;
    int64_t time;
    unsigned int command_length;
};

#endif // PACKAGE_HEADER_HPP
