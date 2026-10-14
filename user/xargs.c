#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[]) 
{
    if (argc < 2) {
        fprintf(2, "Usage: xargs command [args]...\n");
        exit(1);
    }
    char buf[512], *p = buf, *start = buf;
    char *arg[MAXARG];
    for (int x = 0; x < argc-1; x++) {
        arg[x] = argv[1+x];
    }
    int i = argc - 1;
    while (read(0, p, sizeof(char)) != 0) {
        if (*p == ' ') {
            *p = '\0';
            arg[i] = start;
            i++;
            p++;
            start = p;
        }
        else if (*p == '\n') {
            *p = '\0';
            arg[i] = start;
            i++;
            p++;
            arg[i] = 0;
            int pid = fork();
            if (pid == 0) {
                exec(argv[1], arg);
                exit(0);
            } else {
                wait(0);
                i = argc-1;
                p = buf;
                start = buf;
            }
        } else {
            p++;
        }
    }
    exit(0);

    return 0;
}