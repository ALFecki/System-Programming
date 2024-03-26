#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define MAX_FILENAME_LENGTH 100

int main() {
    int array_capacity = 10;  // Изначальная емкость массива
    int array_size = 0;      // Фактический размер массива
    int* array = (int*) malloc(array_capacity * sizeof(int));

    char filename[MAX_FILENAME_LENGTH];
    printf("Введите имя файла: ");
    scanf("%s", filename);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Не удалось открыть файл\n");
        return 1;
    }

    // Чтение данных из файла и расширение массива при необходимости
    int value;
    while (fscanf(file, "%d", &value) == 1) {
        // Если фактический размер массива равен его емкости, увеличиваем емкость вдвое
        if (array_size == array_capacity) {
            array_capacity *= 2;
            array = (int*) realloc(array, array_capacity * sizeof(int));
        }

        array[array_size] = value;
        array_size++;
    }

    fclose(file);

    // Сортировка массива
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    for (int i = 0; i < array_size - 1; i++) {
        for (int j = 0; j < array_size - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }

    gettimeofday(&end_time, NULL);
    double execution_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    // Вывод отсортированного массива и время выполнения
    printf("Отсортированный массив:\n");
    for (int i = 0; i < array_size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
    printf("Время выполнения: %.6f сек\n", execution_time);

    // Освобождение памяти
    free(array);

    return 0;
}