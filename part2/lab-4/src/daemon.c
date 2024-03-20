#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

const char* CONFIG_FILE_PATH = "../config.txt";
const char* LOG_FILE_PATH = "../logfile.txt";

volatile sig_atomic_t config_reload = 0;

void sighup_handler(int sig) {
    config_reload = 1;
}

void sigterm_handler(int sig) {
    FILE* log_file = fopen(LOG_FILE_PATH, "a");
    if (log_file != NULL) {
        fprintf(log_file, "Received SIGTERM\n");
        fclose(log_file);
    }

    exit(0);
}

void daemonize() {
    pid_t pid, sid;

    pid = fork();

    if (pid < 0) {
        exit(1);
    }

    if (pid > 0) {
        exit(0);
    }

    sid = setsid();
    if (sid < 0) {
        exit(1);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    FILE* log_file = fopen(LOG_FILE_PATH, "a");
    if (log_file != NULL) {
        fprintf(log_file, "Daemon started\n");
        fclose(log_file);
    }
}

void read_config_file() {
    FILE* config_file = fopen(CONFIG_FILE_PATH, "r");
    if (config_file == NULL) {
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), config_file) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        int sig = atoi(line);

        if (signal(sig, SIG_IGN) == SIG_ERR) {
            // Обработка ошибки установки обработчика сигнала
        }
    }

    fclose(config_file);
}

int main(int argc, char* argv[]) {
    signal(SIGHUP, sighup_handler);
    signal(SIGTERM, sigterm_handler);

    if (argc > 1 && (strcmp(argv[1], "-q") == 0 || strcmp(argv[1], "--quit") == 0)) {
        FILE* log_file = fopen(LOG_FILE_PATH, "a");
        if (log_file != NULL) {
            fprintf(log_file, "Received quit command\n");
            fclose(log_file);
        }

        kill(getpid(), SIGTERM);
        exit(0);
    }

    daemonize();

    while (1) {
        if (config_reload) {
            FILE* log_file = fopen(LOG_FILE_PATH, "a");
            if (log_file != NULL) {
                fprintf(log_file, "Reloaded configuration\n");
                fclose(log_file);
            }

            config_reload = 0;
        }

        sleep(1);
    }

    return 0;
}