# sysmon

A small terminal-based system monitor written in C11. It reads system metrics from Linux's `/proc` filesystem and refreshes the display approximately once per second.

## Features

- Overall CPU usage calculated from consecutive samples.
- Total, free, and available memory.
- System uptime.
- Top 10 processes by resident memory (RSS), showing PID, name, and memory usage.
- Colored CPU and RAM usage bars.
- Adjusts the number of displayed processes to the terminal height.
- Uses the terminal's alternate screen and restores it on exit.
- Exits with Ctrl+C.

## Requirements

- Linux with the `/proc` filesystem.
- GCC with C11 support.
- A terminal that supports ANSI escape sequences and the alternate screen.

No third-party libraries are required.

## Build and run

From the project directory:

    gcc -std=c11 -Wall -Wextra -o sysmon main.c sysinfo.c ui.c process.c
    ./sysmon

Press **Ctrl+C** to exit.

## Project structure

- `main.c` — update loop, signal handling, and terminal-size handling.
- `sysinfo.h` — system metric structures and function declarations.
- `sysinfo.c` — reads CPU, memory, and uptime data.
- `process.h` — process information structure and function declaration.
- `process.c` — scans `/proc` and sorts processes by resident memory.
- `ui.h` — user interface function declarations.
- `ui.c` — draws the metrics, usage bars, and process table.

## Data sources

- `/proc/stat` — CPU time counters. Overall CPU usage is calculated using the difference between consecutive samples.
- `/proc/meminfo` — Total, free, and available memory. Used memory is calculated as `MemTotal - MemAvailable`. `MemFree` is shown separately because Linux also caches reclaimable memory.
- `/proc/uptime` — System uptime.
- `/proc/[PID]/stat` — Process name.
- `/proc/[PID]/statm` — Process resident memory (RSS). Because processes can share memory pages, the sum of their RSS values may differ from total system memory usage.

## Limitations

- Processes are sorted by memory usage, not CPU usage.
- Per-process CPU usage is not displayed.
- Processes that exit while `/proc` is being scanned may be skipped.
- When run under WSL, the displayed metrics describe the Linux environment, not the entire Windows host.