#ifndef UI_H
#define UI_H

#include "process.h"
#include "sysinfo.h"

/*очистка экрана и возвращение курсора в левый верхний угол */
void ui_clear(void);
/*Скрыть/показать курсор , чтобы он не мигал каждый тик*/
void ui_hide_cursor(void);
void ui_show_cursor(void);
/*Отобразить информацию о системе */
void ui_draw_bar(const char *label, double percent, int width);
/*отрисовать весь экран : проц и озу*/
void ui_render(double cpu_percent, const MemInfo *mem, double uptime_sec,  
               const ProcessInfo *procs, int proc_count);

#endif