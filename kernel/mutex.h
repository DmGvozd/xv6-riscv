#ifndef _MUTEX_H_
#define _MUTEX_H_
#include "sleeplock.h"

struct mutex {
    struct sleeplock lock;   // 0 - unlocked, 1 - locked
    int refs;     // counter for number of references to the mutex
};

#endif