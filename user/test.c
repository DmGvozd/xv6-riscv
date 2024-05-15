#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define A 1664525
#define C 1013904223
#define M 4294967296

#define BUF_SIZE 1024 * 4

uint buf[BUF_SIZE];

void generate_sequence(char *filename, uint size, uint seed)
{
    if (size <= 0 || size % 4 != 0)
    {
        fprintf(1, "Invalid file size\n");
        exit(1);
    }
    if (seed < 0)
    {
        fprintf(1, "Invalid seed\n");
        exit(1);
    }

    int fd = open(filename, O_CREATE | O_WRONLY);
    if (fd < 0)
    {
        fprintf(1, "Error creating file\n");
        exit(1);
    }

    uint state = seed;
    uint bytes_written = 0;
    int p = 0;
    for (uint i = 0; i < size / 4; i++)
    {
        buf[p++] = state;
        state = ((uint64) A * state + C) % M;

        if (p == BUF_SIZE || i == size / 4 - 1)
        {
            int cur = write(fd, buf, p * 4);
            bytes_written += cur;
            if (cur != p * 4)
            {
                fprintf(1, "Error writing to file\n");
                close(fd);
                exit(1);
            }
            p = 0;
        }
    }

    close(fd);
    fprintf(1, "Successfully wrote %d bytes to file\n", bytes_written);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(1, "Usage: test_largefile <file_size> <seed>\n");
        exit(1);
    }

    uint size = atoi(argv[1]);
    uint seed = atoi(argv[2]);

    generate_sequence(argv[1], size, seed);

    uint state = seed;
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        fprintf(1, "Error opening file\n");
        exit(1);
    }

    uint bytes_read = 0;
    uint corrupted_bytes = 0;

    while (1)
    {
        int cur = read(fd, buf, BUF_SIZE * 4);
        if (cur == 0)
            break;
        bytes_read += cur;
        for (int j = 0; j < cur / 4; ++j)
        {
            if (state != buf[j])
                corrupted_bytes++;
            state = ((uint64) A * state + C) % M;
        }
    }

    close(fd);

    fprintf(1, "Successfully read %d bytes from file\n", bytes_read);
    if (corrupted_bytes == 0)
    {
        fprintf(1, "File verification successful\n");
    }
    else
    {
        fprintf(1, "File verification failed: %d corrupted bytes\n", corrupted_bytes);
    }
    exit(0);
}