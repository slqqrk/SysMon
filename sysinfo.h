#ifndef SYSINFO_H
#define SYSINFO_H
#include <stdint.h>

/* так ну короче, тут счетчик проца из proc/stat в тиках*/
typedef struct {
    uint64_t user, nice, system, idle, iowait, irq, softirq, steal;
} CpuSample;
typedef struct {
    uint64_t total_kb;
    uint64_t free_kb;
    uint64_t available_kb;
} MemInfo;
/*возвращаем 0 при успехе и -1 при ошибке*/
int sysinfo_read_cpu(CpuSample *out);
int sysinfo_read_mem(MemInfo *out);
int sysinfo_read_uptime(double *seconds);

/* процент загрузки CPU между двумя замерами */
double sysinfo_get_cpu_usage(const CpuSample *prev, const CpuSample *curr);
#endif