#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"

void list_processes() {
    DIR *dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir");
        exit(1);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char *path = malloc(strlen("/proc/") + strlen(entry->d_name) + strlen("/cmdline") + 1);
            if (path == NULL) {
                perror("malloc");
                exit(1);
            }
            snprintf(path, strlen("/proc/") + strlen(entry->d_name) + strlen("/cmdline") + 1, "/proc/%s/cmdline", entry->d_name);
            FILE *file = fopen(path, "r");
            if (file != NULL) {
                char cmdline[256];
                fgets(cmdline, sizeof(cmdline), file);
                fclose(file);
                if (strlen(cmdline) > 0) {
                    printf("PID: %s, Command: %s\n", entry->d_name, cmdline);
                }
            }
        }
    }

    closedir(dir);
}

void print_process_info(int pid) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE *file = fopen(path, "r");
    if (file != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            if (strncmp(line, "Name:", 5) == 0 || strncmp(line, "State:", 6) == 0 || strncmp(line, "VmSize:", 7) == 0) {
                printf("%s", line);
            }
        }
        fclose(file);
    } else {
        printf("Process with PID %d not found\n", pid);
    }
}