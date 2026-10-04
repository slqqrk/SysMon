#define _POSIX_C_SOURCE 200809L

#include <sys/ioctl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "sysinfo.h"
#include "ui.h"
#include "process.h"

static volatile sig_atomic_t g_running = 1;

static void handle_signal(int sig)
{
    (void)sig;
    g_running = 0;
}

int main(void)
{
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    CpuSample prev, curr;
    MemInfo mem;
    double uptime = 0.0;
    ProcessInfo procs[10];
    int proc_count = 0;

    if (sysinfo_read_cpu(&prev) != 0) {
        fprintf(stderr, "Cannot read /proc/stat\n");
        return 1;
    }

    ui_clear();
    ui_hide_cursor();

   while (g_running) {
        sleep(1);

        if (sysinfo_read_cpu(&curr) != 0 ||
            sysinfo_read_mem(&mem) != 0 ||
            sysinfo_read_uptime(&uptime) != 0) {
            break;
        }

        int max_procs = 10;
        struct winsize ws;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_row > 0) {
            max_procs = (int)ws.ws_row - 12;
            if (max_procs > 10) max_procs = 10;
            if (max_procs < 0)  max_procs = 0;
        }

        double cpu = sysinfo_get_cpu_usage(&prev, &curr);
        proc_count = process_get_top_by_memory(procs, max_procs);
        if (proc_count < 0) proc_count = 0;
        ui_render(cpu, &mem, uptime, procs, proc_count);
        prev = curr;
    }

    ui_show_cursor();
    printf("\nBye!\n");
    return 0;
}