//
// Created by Jakub Wieliczko, Maja Giglok and Anna Szpak on 25/10/2025.
//

#include "colors.hpp"

#include <vector>
#include <string>
#include <sstream>


static const color_value base[8] = {black, red, green, yellow, blue, purple, cyan, white};

void setting::reset() {
    *this = setting{};
}


setting get_settings(const std::vector<int>& codes) {
    setting result;
    for (int code: codes) {
        if (code == 0) {
            result.reset();
            continue;
        }

        if(code==1) { result.font.bold=true; continue; }
        if(code==2) { result.font.faint=true; continue; }
        if(code==3) { result.font.italic=true; continue; }
        if(code==4) { result.font.underline=true; continue; }
        if(code==5 || code==6) { result.font.blink=true; continue; }
        if(code==7) { result.font.inverse=true; continue; }
        if(code==8) { result.font.conceal=true; continue; }
        if(code==9) { result.font.strike=true; continue; }

        if(code==21 || code==22){ result.font.bold=false; result.font.faint=false; continue; }
        if(code==23){ result.font.italic=false; continue; }
        if(code==24){ result.font.underline=false; continue; }
        if(code==25){ result.font.blink=false; continue; }
        if(code==27){ result.font.inverse=false; continue; }
        if(code==29){ result.font.strike=false; continue; }

        else if (code >= 30 && code <= 37) {
            color_value c = base[code - 30];
            result.foreground = {c, basic};
            continue;
        }

        if (code >= 90 && code <= 97) {
            color_value c = base[code - 90];
            result.foreground = {c, bright};
            continue;
        }

        // Background colors
        if (code >= 40 && code <= 47) {
            color_value c = base[code - 40];
            result.background = {c, basic};
            continue;
        }

        // Bright background
        if (code >= 100 && code <= 107) {
            color_value c = base[code - 100];
            result.background = {c, bright};
            continue;
        }

    }
    return result;
}

std::string get_color(const color& c) {
    switch(c.c) {
        case black:
            return "black";
        case red:
            return "red";
        case green:
            return "green";
        case yellow:
            return "yellow";
        case blue:
            return "blue";
        case purple:
            return "purple";
        case cyan:
            return "cyan";
        case white:
            return "white";
        default:
            return "";
    }
}

std::string get_html(const std::string& log, const setting& style) {
    std::ostringstream oss;
    oss << "<span style=\"color:" << get_color(style.foreground) << "\">";
    oss << log;
    oss << "</span>";
    return oss.str();
}

std::vector<std::tuple<int, int, std::vector<int>>> parse_params(const std::string& s) {
    size_t n = s.length();
    std::vector<std::tuple<int, int, std::vector<int>>> result;
    for (size_t i = 0; i < n; i++) {
        char x = s[i];
        if(x == '\x1B') {
            std::vector<int> out;
            int cur = 0; bool have = false;
            size_t j;
            for (j = i; j < n; j++) {
                char ch = s[j];
                if (ch >= '0' && ch <= '9') {
                    cur = cur * 10 + (ch - '0');
                    have = true;
                } else if (ch == ';') {
                    out.push_back(have ? cur : 0);
                    cur = 0;
                    have = false;
                } else if (ch == 'm') {
                    j++;
                    break;
                }
            }
            out.push_back(have ? cur : 0);
            if (out.empty()) out.push_back(0);
            result.emplace_back(i, j, out);
        }
    }
    return result;
}

