#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

// Путь к файлу протоколирования
const char* CONFIG_FILE_PATH = "/home/alexander/dev/config.txt";
const char* LOG_FILE_PATH = "/home/alexander/dev/logfile.txt";

// Глобальная переменная для хранения сигнала SIGHUP
volatile sig_atomic_t config_reload = 0;

// Обработчик сигнала SIGHUP
void sighup_handler(int sig) {
    config_reload = 1;
}

// Обработчик сигнала SIGTERM
void sigterm_handler(int sig) {
    // Протоколируем сигнал SIGTERM
    FILE* log_file = fopen(LOG_FILE_PATH, "a");
    if (log_file != NULL) {
        fprintf(log_file, "Received SIGTERM\n");
        fclose(log_file);
    }

    // Выполняем завершение работы
    exit(0);
}

// Функция преобразования процесса в демон
void daemonize() {
    pid_t pid, sid;

    // Создаем новый процесс
    pid = fork();

    // Если произошла ошибка при создании процесса, завершаем работу
    if (pid < 0) {
        exit(1);
    }

    // Если это родительский процесс, завершаем работу
    if (pid > 0) {
        exit(0);
    }

    // Устанавливаем новый SID для дочернего процесса
    sid = setsid();
    if (sid < 0) {
        exit(1);
    }

    // Закрываем стандартные потоки ввода/вывода/ошибок
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Открываем файл протоколирования в режиме добавления
    FILE* log_file = fopen(LOG_FILE_PATH, "a");
    if (log_file != NULL) {
        fprintf(log_file, "Daemon started\n");
        fclose(log_file);
    }
}

void read_config_file() {
    FILE* config_file = fopen(CONFIG_FILE_PATH, "r");
    if (config_file == NULL) {
        // Обработка ошибки открытия файла конфигурации
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), config_file) != NULL) {
        // Удаление символа новой строки из строки
        line[strcspn(line, "\n")] = '\0';

        // Преобразование строки в сигнал
        int sig = atoi(line);

        // Установка обработчика сигнала
        if (signal(sig, SIG_IGN) == SIG_ERR) {
            // Обработка ошибки установки обработчика сигнала
        }
    }

    fclose(config_file);
}

// Главная функция
int main(int argc, char* argv[]) {
    // Установка обработчиков сигналов
    signal(SIGHUP, sighup_handler);
    signal(SIGTERM, sigterm_handler);

    // Проверка опционального флага для выгрузки демона из памяти
    if (argc > 1 && (strcmp(argv[1], "-q") == 0 || strcmp(argv[1], "--quit") == 0)) {
        // Протоколируем запрос на выгрузку
        FILE* log_file = fopen(LOG_FILE_PATH, "a");
        if (log_file != NULL) {
            fprintf(log_file, "Received quit command\n");
            fclose(log_file);
        }

        // Отправляем сигнал SIGTERM себе для завершения работы
        kill(getpid(), SIGTERM);
        exit(0);
    }

    // Преобразуем процесс в демон
    daemonize();

    // Бесконечный цикл работы демона
    while (1) {
        // Если получен сигнал SIGHUP, выполняем перезагрузку конфигурации
        if (config_reload) {
            // Протоколируем перезагрузку конфигурации
            FILE* log_file = fopen(LOG_FILE_PATH, "a");
            if (log_file != NULL) {
                fprintf(log_file, "Reloaded configuration\n");
                fclose(log_file);
            }

            // Здесь можно выполнить перезагрузку конфигурации

            // Сбрасываем флаг перезагрузки
            config_reload = 0;
        }

        // Здесь выполняем основную функциональность демона

        // Ждем некоторое время перед следующей итерацией
        sleep(1);
    }

    return 0;
}