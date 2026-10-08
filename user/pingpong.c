#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
    int p[2];
    if (pipe(p) == -1) {
        printf("Pipe bulid failed...\n");
        exit(1);
    }; // p[0] is read end; p[1] is write end

    char buf[128];

    int pid = fork();
    if (pid > 0) {
        char *message_1 = "AAA";
        write(p[1], message_1, strlen(message_1));
        close(p[1]);
        wait(0); //wait for child finish
        if (read(p[0], buf, sizeof(buf))) {
            close(p[0]);
            printf("%d: received pong\n", getpid());
        }
    } else if(pid == 0) {
        if (read(p[0], buf, sizeof(buf))) {
            close(p[0]);
            printf("%d: received ping\n", getpid());
            char *message_2 = "AAA";
            write(p[1], message_2, strlen(message_2));
            close(p[1]);
        }
        exit(0);
    }
    exit(0);
}