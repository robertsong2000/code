#include <stddef.h>
#include <stdio.h>
#include <kernel/sched.h>
#include <dev/resource.h>
#include <dev/shared_mem.h>
#include "app.h"

void memreader(void);

void ipctest(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: %s\n", argv[0]);
        return;
    }

    rd_t memrd = open_shared_mem();
    if (memrd < 0) {
        printf("Error: unable to open shared mem.\r\n");
    }

    printf("WRITING MEM.\r\n");

    swrite(memrd, "THIS IS A TEST OF SHARED MEMORY REGIONS N STUFF.");

    printf("READING MEM.\r\n");
    new_task(&memreader, 5, 0);
}
DEFINE_APP(ipctest)

void memreader(void) {
    char buf[16];
    rd_t memrd = curr_task->resource_data.top_rd - 1;

    read(memrd, buf, 10);
    buf[10] = 0x00;

    puts(buf);

    close(memrd);
}
