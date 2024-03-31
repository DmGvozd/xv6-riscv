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
        exit(5);
    }
    if (pipe(c2p) < 0) {
        printf("Ошибка создания канала c2p\n");
        exit(5);
    }
    
    int mutex = mopen();
    if (mutex < 0) {
        printf("Ошибка создания mutex\n");
        exit(6);
    }
    
    int pid = fork();
    if (pid < 0) {
        printf("Ошибка при создании нового процесса\n");
        exit(4);
    } else if (pid > 0) {
        close(p2c[0]);
        close(c2p[1]);
        
        mlock(mutex);
        if (write(p2c[1], argv[1], strlen(argv[1])) < 0) {
            printf("Ошибка записи в канал p2c\n");
            munlock(mutex);
            exit(7);
        }
        munlock(mutex);
        close(p2c[1]);
        
        int res;
        while ((res = read(c2p[0], &buffer, 1)) > 0) {
            mlock(mutex);
            printf("%d: received %c\n", getpid(), buffer[0]);
            munlock(mutex);
        }
        
        close(c2p[0]);
        if (res < 0) {
            printf("Ошибка чтения\n");
            mclose(mutex);
            exit(2);
        }
        
        int status;
        if (wait(&status) < 0) {
            printf("Ошибка при ожидании завершения дочернего процесса\n");
            mclose(mutex);
            exit(8);
        }
        
        mclose(mutex);
    } else {
        close(p2c[1]);
        close(c2p[0]);
        
        int res;
        while ((res = read(p2c[0], &buffer, 1)) > 0) {
            mlock(mutex);
            printf("%d: received %c\n", getpid(), buffer[0]);
            munlock(mutex);
            
            mlock(mutex);
            if (write(c2p[1], &buffer, 1) < 0) {
                printf("Ошибка записи в канал c2p\n");
                munlock(mutex);
                exit(7);
            }
            munlock(mutex);
        }
        
        close(p2c[0]);
        close(c2p[1]);
        if (res < 0) {
            printf("Ошибка чтения\n");
            mclose(mutex);
            exit(2);
        }
        
        mclose(mutex);
        exit(0);
    }
    
    exit(0);
}
