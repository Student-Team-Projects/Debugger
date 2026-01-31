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
#include <string>
#include <functional>
#include <sys/wait.h>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <signal.h>

#include "const.hpp"
#include "child.hpp"
#include "fds_listner.hpp"
#include "log_writer.hpp"
#include "package_header.hpp"
#include "utilz.hpp"

using namespace std;

static void process(int size, char* buf, int fd, pid_t pid) {
    package_header head;
    memcpy(&head, buf, sizeof(package_header));

    if (size < (int)sizeof(package_header) || head.header_key != HEADER_CONST) { // new line creating header
        auto now = chrono::system_clock::now();
        auto currentTime = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();

        head.type = 0;
        head.time = currentTime;
        head.pid = pid;
        head.parent_pid = 0;
        head.header_key = HEADER_CONST;
        head.command_length = 0;
        if (write(fd, &head, sizeof(package_header)) < 0) {
            perror("write failed");
            exit(1);
        }
    } else { // line with a header
        if (head.parent_pid == 0) { // line without parent
            head.parent_pid = getpid();
            head.time += 1;
            memcpy(buf, &head, sizeof(package_header));
        }
    }
    if (write(fd, buf, size) < 0) {
        perror("write failed");
        exit(1);
    }
}

static void writeInfoToTmpFiles(pid_t pid, pid_t child_pid) {
    int status = 0;
    pid_t wpid = waitpid(child_pid, &status, 0);
    
    int exitCode = -1;
    if (wpid > 0) {
        if (WIFEXITED(status)) {
            exitCode = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            exitCode = 128 + WTERMSIG(status);
        }
    }

    createJsFile(pid, exitCode);
}

int childProcess(char* program, char* argv[]) {
    // create pipe LISTENER <- PROGRAM
    int pipe_fd_out[2], pipe_fd_err[2];
    [[maybe_unused]] int ret1 = pipe(pipe_fd_out);
    [[maybe_unused]] int ret2 = pipe(pipe_fd_err);

    // ignoring SIGINT to update error code
    signal(SIGINT, SIG_IGN);

    pid_t fork_pid = fork();
    if (fork_pid == 0) {
        signal(SIGINT, SIG_DFL);

        // PROGRAM -> LISTENER
        close(pipe_fd_out[0]);
        close(pipe_fd_err[0]);
        dup2(pipe_fd_out[1], STDOUT_FILENO);
        dup2(pipe_fd_err[1], STDERR_FILENO);


        std::string command = buildCommand(program, argv);

        if (write(STDOUT_FILENO, command.c_str(), command.length()) < 0) {
            perror("write failed");
            exit(1);
        }
        if (write(STDOUT_FILENO, "\n", 1) < 0) {
            perror("write failed");
            exit(1);
        }

        execvp(program, argv);
    }

    // LISTENER <- PROGRAM
    close(pipe_fd_out[1]);
    close(pipe_fd_err[1]);
    dup2(pipe_fd_out[0], FD_OUT);
    dup2(pipe_fd_err[0], FD_ERR);

    pid_t pid = getpid();
    listen_on_fds([pid](int size, char* buffer, int fd) {process(size, buffer, fd, pid);});
    writeInfoToTmpFiles(pid, fork_pid);

    return 0;
}