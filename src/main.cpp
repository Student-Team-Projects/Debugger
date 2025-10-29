/*
 * Copyright (C) 2025 M. Wykpis, A. Szwaja, P. Kubicki, S. Szulc, K. Socha
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <limits.h>
#include <functional>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>

#include "parser.hpp"
#include "fds_listner.hpp"
#include "package_header.hpp"
#include "child.hpp"
#include "const.hpp"
#include "colors.hpp"
#include "utilz.hpp"
#include "call_handler.hpp"

using namespace std;

int rootProcess(pid_t child_pid) {
    map<string, ofstream> streamMap;
    map<string, string> dataMap;
    streamMap.clear();
    dataMap.clear();

    char* static_filename = getenv(ENV_STATIC_FILENAME);
    if (static_filename) {
        dataMap[to_string(child_pid)] = static_filename;
    }
    const static char* RED = "\x1B[31m";
    const static char* RESET =  "\x1B[0m";
    auto proc = [&streamMap, &dataMap](int size, char* buf, int fd) {
        if (size == sizeof(package_header)) return; // empty log early return;
        parse_buffer(streamMap, dataMap, buf, fd == STDERR_FILENO, size);
        if (fd == STDERR_FILENO) write(fd, RED, 6);
        write(fd, buf + sizeof(package_header), size - sizeof(package_header));
        if (fd == STDERR_FILENO) write(fd, RESET, 5);
        };

    listen_on_fds(proc);

    for (auto& [k, v] : streamMap) {
        v.close();
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc > 1 && string(argv[1]) == "--callhandler") {
        callhandlerProcess(argv[2], argv + 2); // ends with exec
    }

    char* debug_var = getenv(ENV_DEBUG);

    if (!debug_var) {

        setenv(ENV_DEBUG, DEBUGGER, 1);
        string callhandler_var = string(DEBUGGER) + " --callhandler";
        setenv(ENV_CALLHANDLER, callhandler_var.c_str(), 1);


        mode_t old_mask = umask(0); // just in case
        string debugger_path = getOutputPath();
        mkdir(debugger_path.c_str(), 0777); // TODO think about permissions, maybe too loose
        mkdir((debugger_path + "/all_logs").c_str(), 0777);
        char info_dir[] = TMP_INFO_DIR_PATH;
        mkdir(info_dir, 0777);
        umask(old_mask);

        deleteContentOfDir(string(TMP_INFO_DIR_PATH));
        createStyles(debugger_path);
        createIndex(debugger_path);

        if (argc == 1) return 0;

        // create pipe ROOT <- LISTENER 1
        int pipe_fd_out[2], pipe_fd_err[2];
        pipe(pipe_fd_out);
        pipe(pipe_fd_err);

        pid_t pid = fork();
        if (pid != 0) {
            // ROOT <- CHILD
            close(pipe_fd_out[1]);
            close(pipe_fd_err[1]);
            dup2(pipe_fd_out[0], FD_OUT);
            dup2(pipe_fd_err[0], FD_ERR);

            int exit_code = rootProcess(pid);
            unsetenv(ENV_DEBUG);
            return exit_code;
        } else {
            // CHILD -> ROOT
            close(pipe_fd_out[0]);
            close(pipe_fd_err[0]);
            dup2(pipe_fd_out[1], STDOUT_FILENO);
            dup2(pipe_fd_err[1], STDERR_FILENO);
        }
    }

    int exit_code = childProcess(argv[1], argv + 1);
    return exit_code;
}
