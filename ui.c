#include "ui.h"
#include <stdio.h>
/* ANSI цвета */
#define COLOR_RESET  "\033[0m"  
#define COLOR_GREEN  "\033[32m"  
#define COLOR_YELLOW "\033[33m"  
#define COLOR_RED    "\033[31m"  
#define COLOR_BOLD   "\033[1m"  
#define COLOR_CYAN   "\033[36m"
/* Цвета по уровню загрузки */
static const char *level_color(double percent) {
    if (percent < 45.0) return COLOR_GREEN;
    else if (percent < 80.0) return COLOR_YELLOW;
    else return COLOR_RED;
}
void ui_clear(void)
{
    /* \033[?1049h - перейти на альтернативный экран */
    printf("\033[?1049h\033[H\033[2J");
    fflush(stdout);
}

void ui_hide_cursor(void)
{
    printf("\033[?25l");
    fflush(stdout);
}

void ui_show_cursor(void)
{
    /* показать курсор и вернуться на обычный экран */
    printf("\033[?25h\033[?1049l");
    fflush(stdout);
}
void ui_draw_bar(const char *label, double percent, int width){
    if (percent < 0.0) percent = 0.0;
    if (percent > 100.0) percent = 100.0;

    int filled = (int)(percent / 100.0 * width);

    printf("%-5s [%s", label, level_color(percent));  
    for (int i = 0; i < width; i++) {  
        putchar(i < filled ? '|' : ' ');  
    }  
    printf(COLOR_RESET "] %5.1f%%\n", percent);
}
void ui_render(double cpu_percent, const MemInfo *mem, double uptime_sec,  
               const ProcessInfo *procs, int proc_count) {
    double total_mb = mem->total_kb / 1024.0;
    double free_mb = mem->free_kb / 1024.0;
    double used_mb = (mem->total_kb - mem->free_kb) / 1024.0;
    double ram_pct = mem->total_kb ? 100.0 * used_mb / total_mb : 0.0;

    int total_s = (int)uptime_sec;
    int days = total_s / 86400;
    int hours = (total_s % 86400) / 3600;
    int mins = (total_s % 3600) / 60;
    int secs = total_s % 60;

    printf("\033[H");
    printf(COLOR_BOLD COLOR_CYAN "=== sysmon ===" COLOR_RESET "\033[K\n\033[K\n");  
  
    ui_draw_bar("CPU", cpu_percent, 30);  
    ui_draw_bar("RAM", ram_pct, 30);  
  
    printf("\033[K\n");  
    printf("RAM: %.0f MB used / %.0f MB total (free: %.0f MB)\033[K\n",  
           used_mb, total_mb, free_mb);  
    printf("Uptime: %dd %02dh %02dm %02ds\033[K\n", days, hours, mins, secs);  
    printf("\033[K\n");
    printf(COLOR_BOLD "%7s  %10s  %s" COLOR_RESET "\033[K\n",
           "PID", "MEM (MB)", "COMMAND");

    for (int i = 0; i < proc_count; i++) {
        printf("%7d  %10.1f  %s\033[K\n",
               (int)procs[i].pid, procs[i].rss_kb / 1024.0, procs[i].name);
    }

    printf("\033[K\nCtrl+C to exit\033[K\n");
    printf("\033[J");  /* стереть всё ниже, если процессов стало меньше */  
  
    fflush(stdout);
}