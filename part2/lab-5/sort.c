#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define MAX_FILENAME_LENGTH 100

// Функция сортировки массива
void sort(int* array, int array_size) {
    // Сортировка массива
    for (int i = 0; i < array_size; i++) {
        for (int j = i + 1; j < array_size; j++) {
            if (array[i] > array[j]) {
                int temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }
}

int main() {
    char filename[MAX_FILENAME_LENGTH];
    int array_capacity = 10;  // Изначальная емкость массива
    int array_size = 0;      // Фактический размер массива
    int* array = (int*) malloc(array_capacity * sizeof(int));
    printf("Введите имя файла: ");
    scanf("%s", filename);

    // Открытие файла для чтения
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Не удалось открыть файл\n");
        return 1;
    }

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
    printf("Данные из файла прочитаны!\n");
    printf("Массив данных имеет длину: %d\n", array_size);

    // Сортировка массива
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    sort(array, array_size);

    gettimeofday(&end_time, NULL);
    double execution_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    printf("Время выполнения: %.3f сек\n", execution_time);

    // Освобождение памяти
    free(array);

    return 0;
}