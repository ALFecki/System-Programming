#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main() {
    int array_size;
    char filename[MAX_FILENAME_LENGTH];
    printf("Введите имя файла: ");
    scanf("%s", filename);

    // Открытие файла для чтения
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Не удалось открыть файл\n");
        return 1;
    }

    // Чтение размера массива из файла
    fscanf(file, "%d", &array_size);

    // Создание и заполнение массива данными из файла
    int* array = (int*) malloc(array_size * sizeof(int));
    for (int i = 0; i < array_size; i++) {
        fscanf(file, "%d", &array[i]);
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