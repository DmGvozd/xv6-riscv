#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Минимум 2 аргумента\n");
        exit(1);
    }

    int p2c[2];
    int c2p[2];
    char buffer[1];

    if (pipe(p2c) < 0) {
        printf("Ошибка создания канала p2c\n");
        exit(1);
    }
    if (pipe(c2p) < 0) {
        printf("Ошибка создания канала c2p\n");
        exit(1);
    }

    int mutex = mopen();
    if (mutex < 0) {
        printf("Ошибка создания mutex\n");
        exit(2);
    }

    int pid = fork();
    if (pid < 0) {
        printf("Ошибка при создании нового процесса\n");
        exit(3);
    }

    if (pid > 0) { // Родительский процесс
        close(p2c[0]);
        close(c2p[1]);
        write(p2c[1], argv[1], strlen(argv[1]));
        close(p2c[1]);
        
        int res;
        while ((res = read(c2p[0], &buffer, 1)) > 0) {
            printf("%d: received %c\n", getpid(), buffer[0]);
        }
        close(c2p[0]);
        if (res < 0) {
            printf("Ошибка чтения\n");
            mclose(mutex);
            exit(4);
        }

        int status;
        wait(&status);
    } else { // Дочерний процесс
        close(p2c[1]);
        close(c2p[0]);

        int res;
        while ((res = read(p2c[0], &buffer, 1)) > 0) {
            printf("%d: received %c\n", getpid(), buffer[0]);
            write(c2p[1], &buffer, 1);
        }
        close(p2c[0]);
        close(c2p[1]);

        if (res < 0){
            printf("Ошибка чтения\n");
            mclose(mutex);
            exit(5);
        }
    }

    mclose(mutex);
    exit(0);
}
