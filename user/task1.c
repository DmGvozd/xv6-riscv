#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// "task1 a" to start a
// "task1 b" to start b

int main(int argc, char *argv[]) {
    int pid, status, wpid;
    int p[2];

    if (argc != 2) {
        fprintf(2, "Usage: %s a|b\n", argv[0]);
        exit(1);
    }

    if (pipe(p) < 0) {
        fprintf(2, "pipe creation failed\n");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(5);
    }

    if (pid == 0) { // child
        sleep(50);
        exit(1);
    } else { // parent
        printf("Parent process ID: %d\n", getpid());
        printf("Child process ID: %d\n", pid);

        if (argv[1][0] == 'a') {
            while ((wpid = wait(&status)) != pid);
            printf("Child process ID: %d exited with status: %d\n", wpid, status);
        } else if (argv[1][0] == 'b') {
            kill(pid);
            wait(&status);
            printf("Child process ID: %d exited with status: %d\n", pid, status);
        } else {
            fprintf(2, "Invalid argument\n");
            exit(1);
        }
    }

    exit(0);
}