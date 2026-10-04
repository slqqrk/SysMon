#include "sysinfo.h"
#include <inttypes.h>
#include <stdio.h>

int sysinfo_read_cpu(CpuSample *out) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return -1;
  /*Первая строка все что связано с CPU все инты */
    int n = fscanf(fp, "cpu %" SCNu64 " %" SCNu64 " %" SCNu64 " %" SCNu64 " %" SCNu64 " %" SCNu64 " %" SCNu64 " %" SCNu64,
                   &out->user, &out->nice, &out->system, &out->idle,
                   &out->iowait, &out->irq, &out->softirq, &out->steal);
    fclose(fp);
    return (n == 8) ? 0 : -1;
}
int sysinfo_read_mem(MemInfo *out){
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) return -1;
    char line[256];
    int found = 0;
    out->total_kb = out->free_kb = out->available_kb = 0;

    while (fgets(line, sizeof(line), f)) {
        uint64_t v;
        if (sscanf(line, "MemTotal: %" SCNu64 " kB", &v) == 1) {
            out->total_kb = v;
            found++;
        } else if (sscanf(line, "MemFree: %" SCNu64 " kB", &v) == 1) {
            out->free_kb = v;
            found++;
        } else if (sscanf(line, "MemAvailable: %" SCNu64 " kB", &v) == 1) {
            out->available_kb = v;
            found++;
        }
    }
    fclose(f);
    return (found == 3) ? 0 : -1;
}

int sysinfo_read_uptime(double *seconds) {
    FILE *f = fopen("/proc/uptime", "r");
    if (!f) return -1;
    int n = fscanf(f, "%lf", seconds);
    fclose(f);
    return (n == 1) ? 0 : -1;
}
double sysinfo_get_cpu_usage(const CpuSample *prev, const CpuSample *curr) {
    uint64_t prev_total = prev->user + prev->nice + prev->system + prev->idle +
                          prev->iowait + prev->irq + prev->softirq + prev->steal;
    uint64_t curr_total = curr->user + curr->nice + curr->system + curr->idle +
                          curr->iowait + curr->irq + curr->softirq + curr->steal;

    uint64_t total_diff = curr_total - prev_total;
    uint64_t idle_diff = (curr->idle + curr->iowait) - (prev->idle + prev->iowait);

    if (total_diff == 0) return 0.0; // Avoid division by zero

    return (double)(total_diff - idle_diff) / total_diff * 100.0;
}