#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// "task2" to start

int main(int argc, char *argv[]) {
    int fd[2];
    if (pipe(fd) < 0) {
        fprintf(2, "task2: pipe error\n");
        exit(1);
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "task2: fork error\n");
        exit(1);
    }
    if (pid == 0) {  // child
        close(fd[1]);
        char buf[512];
        int n;
        while ((n = read(fd[0], buf, sizeof(buf))) > 0) {
            write(1, buf, n);
        }
        close(fd[0]);
        exit(0);
    } else {  // parent
        close(fd[0]);
        for (int i = 1; i < argc; i++) {
            write(fd[1], argv[i], strlen(argv[i]));
            write(fd[1], "\n", 1);
        }
        close(fd[1]);
        wait(0);
        exit(0);
    }
}
