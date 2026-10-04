#include "process.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int compare_by_memory(const void *a, const void *b)
{
    const ProcessInfo *left = a;
    const ProcessInfo *right = b;

    if (left->rss_kb < right->rss_kb) return 1;
    if (left->rss_kb > right->rss_kb) return -1;
    return 0;
}

static int is_pid_name(const char *name)
{
    if (*name == '\0') return 0;

    for (const char *p = name; *p != '\0'; p++) {
        if (*p < '0' || *p > '9') return 0;
    }

    return 1;
}

int process_get_top_by_memory(ProcessInfo *processes, int max_count)
{
    if (processes == NULL || max_count <= 0) return 0;

    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL) return -1;

    ProcessInfo *all_processes = NULL;
    int count = 0;
    struct dirent *entry;

    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) {
        closedir(proc_dir);
        return -1;
    }

    while ((entry = readdir(proc_dir)) != NULL) {
        if (!is_pid_name(entry->d_name)) continue;

        char stat_path[256];
        char statm_path[256];

        int stat_len = snprintf(stat_path, sizeof stat_path,
                                "/proc/%s/stat", entry->d_name);
        int statm_len = snprintf(statm_path, sizeof statm_path,
                                 "/proc/%s/statm", entry->d_name);

        if (stat_len < 0 || (size_t)stat_len >= sizeof stat_path ||
            statm_len < 0 || (size_t)statm_len >= sizeof statm_path) {
            continue;
        }

        FILE *stat_file = fopen(stat_path, "r");
        if (stat_file == NULL) continue;

        char line[1024];
        if (fgets(line, sizeof line, stat_file) == NULL) {
            fclose(stat_file);
            continue;
        }
        fclose(stat_file);

        /*
         * Формат начинается с PID и имени в скобках.
         * Ищем последнюю ')', чтобы не сломаться на скобках в имени.
         */
        char *name_start = strchr(line, '(');
        char *name_end = strrchr(line, ')');

        if (name_start == NULL || name_end == NULL || name_end <= name_start) {
            continue;
        }

        size_t name_len = (size_t)(name_end - name_start - 1);
        if (name_len >= sizeof ((ProcessInfo *)0)->name) {
            name_len = sizeof ((ProcessInfo *)0)->name - 1;
        }

        ProcessInfo process;
        memcpy(process.name, name_start + 1, name_len);
        process.name[name_len] = '\0';

        char *endptr;
        long pid_value = strtol(entry->d_name, &endptr, 10);
        if (*endptr != '\0' || pid_value <= 0) continue;
        process.pid = (pid_t)pid_value;

        FILE *statm_file = fopen(statm_path, "r");
        if (statm_file == NULL) continue;

        unsigned long total_pages;
        unsigned long resident_pages;
        int fields_read = fscanf(statm_file, "%lu %lu",
                                 &total_pages, &resident_pages);
        fclose(statm_file);

        (void)total_pages;

        if (fields_read != 2) continue;

        process.rss_kb =
            (uint64_t)resident_pages * (uint64_t)page_size / 1024;

        ProcessInfo *resized = realloc(
            all_processes, (size_t)(count + 1) * sizeof *all_processes);

        if (resized == NULL) {
            free(all_processes);
            closedir(proc_dir);
            return -1;
        }

        all_processes = resized;
        all_processes[count++] = process;
    }

    closedir(proc_dir);

    qsort(all_processes, (size_t)count, sizeof *all_processes,
          compare_by_memory);

    int result_count = count < max_count ? count : max_count;
    memcpy(processes, all_processes,
           (size_t)result_count * sizeof *processes);

    free(all_processes);
    return result_count;
}