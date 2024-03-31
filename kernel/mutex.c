#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "spinlock.h"
#include "mutex.h"
#include "stat.h"
#include "proc.h"
#include <stddef.h>

struct mutextable {
    struct spinlock lock;
    struct mutex mutexes_l[NMUTEX];
} mutextable;

void mutex_init(void) {
    initlock(&mutextable.lock, "mutextable");
    for (int i = 0; i < NMUTEX; i++) {  
        initsleeplock(&mutextable.mutexes_l[i].lock, "mutex");
        mutextable.mutexes_l[i].refs = 0;
    }
}

int mutex_alloc(void) {
    acquire(&mutextable.lock);
    for (int i = 0; i < NMUTEX; i++) {
        if (mutextable.mutexes_l[i].refs == 0) {
            mutextable.mutexes_l[i].refs = mutextable.mutexes_l[i].refs + 1;
            release(&mutextable.lock);
            return i;
        }
    }
    release(&mutextable.lock);
    return -1;
}

int mutex_lock(int m_idx) {
    if (m_idx < 0 || m_idx >= NMUTEX) {
        return -1;
    }
    acquiresleep(&mutextable.mutexes_l[m_idx].lock);
    return 0;
}

int mutex_unlock(int m) {
    if (m < 0 || m >= NMUTEX) {
        return -1;
    }
    if (!holdingsleep(&mutextable.mutexes_l[m].lock)) {
        return -2;
    }
    releasesleep(&mutextable.mutexes_l[m].lock);
    return 0;
}

int mutex_close(int m) {
    if (m < 0 || m >= NMUTEX || mutextable.mutexes_l[m].refs == 0) {
        return -1;
    }
    acquire(&mutextable.lock);
    if (mutextable.mutexes_l[m].lock.locked) {
        release(&mutextable.lock);
        return -2;
    }
    mutextable.mutexes_l[m].refs--;
    if (mutextable.mutexes_l[m].refs == 0) {
        releasesleep(&mutextable.mutexes_l[m].lock);
    }
    release(&mutextable.lock);
    return 0;
}


uint64 sys_mopen(void) {
    return mutex_alloc();
}

uint64 sys_mlock(void) {
    int fd;
    argint(0, &fd);
    return mutex_lock(fd);
}

uint64 sys_munlock(void) {
    int fd;
    argint(0, &fd);
    return mutex_unlock(fd);
}

uint64 sys_mclose(void) {
    int fd;
    argint(0, &fd);
    return mutex_close(fd);
}
