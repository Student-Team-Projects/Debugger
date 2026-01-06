/*
 * Copyright (C) 2025 M. Wykpis, A. Szwaja, P. Kubicki, S. Szulc, K. Socha
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef COLORS_HPP
#define COLORS_HPP
#include <string>
#include <vector>
#include <tuple>

enum color_value {
    black,
    red,
    green,
    yellow,
    blue,
    purple,
    cyan,
    white
};

enum color_style {
    unset,
    basic,
    bright
};

struct color {
    color_value c;
    color_style s;
};

struct font_spec {
    bool bold=false, faint=false, italic=false, underline=false,
            blink=false, inverse=false, conceal=false, strike=false;
};

struct setting {
    color foreground = {white, unset};
    color background = {black, unset};
    font_spec font = {};
    void reset();
};

std::string get_html(const std::string& log, const setting& style);
setting get_settings(const std::vector<int>& codes);
std::vector<std::tuple<int, int, std::vector<int>>> parse_params(const std::string& s);
#endif