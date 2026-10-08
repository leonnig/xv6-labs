#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void filter(int fd1) {
    int prime, num;
    int check = read(fd1, &prime, sizeof(int));
    if (check < 0) {
        printf("Get prime failed...\n");
        exit(1);
    }
    if (check == 0)
        exit(0);

    printf("prime %d\n", prime);
    int pipefd[2];
    pipe(pipefd);
    int pid = fork();
    if (pid > 0) {
        while (read(fd1, &num, sizeof(int)) > 0) {
            if (num % prime != 0) {
                write(pipefd[1], &num, sizeof(int));
            }
        }
        close(pipefd[0]);
        close(pipefd[1]);
        close(fd1);
        wait(0);
        exit(0);
    } else if (pid == 0){
        close(fd1);
        close(pipefd[1]);
        filter(pipefd[0]);
    } else {
        exit(1);
    }
    exit(0);
}

int
main() {
    int pipefd[2];
    if (pipe(pipefd) == -1)
        exit(1);
    for (int i = 2; i <= 35; i++) {
        write(pipefd[1], &i, sizeof(int));
    }
    close(pipefd[1]);
    filter(pipefd[0]);

    return 0;
}