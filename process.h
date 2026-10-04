#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <sys/types.h>

typedef struct {
    pid_t pid;
    char name[64];
    uint64_t rss_kb; /* Потребление оперативной памяти в килобайтах */
} ProcessInfo;

/*
 * Читает процессы из /proc, заполняет массив processes (до max_count штук),
 * сортирует их по убыванию потребления памяти (топ-процессы в начале).
 * Возвращает фактическое количество найденных процессов.
 */
int process_get_top_by_memory(ProcessInfo *processes, int max_count);

#endif