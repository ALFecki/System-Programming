#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s [-l | -p <pid>]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-l") == 0) {
        list_processes();
    } else if (strcmp(argv[1], "-p") == 0) {
        if (argc < 3) {
            printf("Usage: %s -p <pid>\n", argv[0]);
            return 1;
        }
        int pid = atoi(argv[2]);
        print_process_info(pid);
    } else {
        printf("Invalid argument: %s\n", argv[1]);
        return 1;
    }

    return 0;
}