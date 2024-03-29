#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_THREADS 8
#define MAX_FILENAME_LENGTH 100

// Структура для передачи параметров в поток
typedef struct {
    int* array;
    int start;
    int end;
} SortParams;

// Функция сортировки одного фрагмента массива
void* sort(void* params) {
    SortParams* sp = (SortParams*) params;
    int* array = sp->array;
    int start = sp->start;
    int end = sp->end;

    // Сортировка фрагмента массива
    for (int i = start; i <= end; i++) {
        for (int j = i + 1; j <= end; j++) {
            if (array[i] > array[j]) {
                int temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }

    pthread_exit(NULL);
}

int main() {
    int num_threads;
    char filename[MAX_FILENAME_LENGTH];
    int array_capacity = 10;  // Изначальная емкость массива
    int array_size = 0;      // Фактический размер массива
    int* array = (int*) malloc(array_capacity * sizeof(int));
    printf("Введите количество потоков: ");
    scanf("%d", &num_threads);
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

    // Разделение массива на фрагменты для каждого потока
    int fragment_size = array_size / num_threads;
    pthread_t threads[MAX_THREADS];
    SortParams params[MAX_THREADS];

    // Запуск потоков для сортировки фрагментов
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    for (int i = 0; i < num_threads; i++) {
        params[i].array = array;
        params[i].start = i * fragment_size;
        params[i].end = (i == num_threads - 1) ? array_size - 1 : (i + 1) * fragment_size - 1;
        pthread_create(&threads[i], NULL, sort, (void*) &params[i]);
    }

    // Ожидание завершения всех потоков
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Сборка отсортированных фрагментов
    int* sorted_array = (int*) malloc(array_size * sizeof(int));
    int* fragment_indices = (int*) calloc(num_threads, sizeof(int));

    for (int i = 0; i < array_size; i++) {
        int min_value = 999999;
        int min_index = -1;

        for (int j = 0; j < num_threads; j++) {
            int index = fragment_indices[j];
            if (index <= params[j].end && array[index] < min_value) {
                min_value = array[index];
                min_index = j;
            }
        }

        sorted_array[i] = min_value;
        fragment_indices[min_index]++;
    }

    gettimeofday(&end_time, NULL);
    double execution_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    printf("Время выполнения: %.3f сек\n", execution_time);

    // Освобождение памяти
    free(array);
    free(sorted_array);
    free(fragment_indices);

    return 0;
}