struct queue {
    struct spinlock lock;
    char data[BUFFER_SIZE];
    int head;
    int tail;
};
