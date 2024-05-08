#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MULTIPLIER 31
#define INCREMENT 74
#define MODULUS 998244353

#define BUFFER_SIZE 1024 * 4

int buffer[BUFFER_SIZE];

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "test: неверный формат (<size> <x0>)\n");
        exit(0);
    }

    int size = atoi(argv[1]), initial_x = atoi(argv[2]);
    if (size <= 0)
    {
        fprintf(2, "test: <size> должен быть больше 0\n");
        exit(0);
    }
    if (size % 4 != 0)
    {
        fprintf(2, "test: <size> должен быть кратен 4\n");
        exit(0);
    }
    if (initial_x < 0)
    {
        fprintf(2, "test: <x0> не может быть меньше 0\n");
        exit(0);
    }

    int file_descriptor = open(argv[1], O_RDWR | O_CREATE), wrote_bytes = 0, x = initial_x;
    int position = 0;
    for (int i = 0; i < size / 4; ++i)
    {
        buffer[position++] = x;
        x = ((uint64)MULTIPLIER * x + INCREMENT) % MODULUS;

        if (position == BUFFER_SIZE || i == size / 4 - 1)
        {
            int current_bytes_written = write(file_descriptor, buffer, position * 4);
            wrote_bytes += current_bytes_written;
            fprintf(2, "записано %d\n", wrote_bytes);
            if (current_bytes_written != position * 4)
                break;
            position = 0;
        }
    }
    close(file_descriptor);

    printf("всего: записано %d байт\n", wrote_bytes);

    file_descriptor = open(argv[1], O_RDONLY);
    x = initial_x;
    int read_bytes = 0, corrupted_bytes = 0;
    while (1)
    {
        int current_bytes_read = read(file_descriptor, buffer, BUFFER_SIZE * 4);
        if (current_bytes_read == 0)
            break;
        read_bytes += current_bytes_read;
        printf("прочитано %d байт\n", read_bytes);
        for (int j = 0; j < current_bytes_read / 4; ++j)
        {
            if (x != buffer[j])
                corrupted_bytes++;
            x = ((uint64)MULTIPLIER * x + INCREMENT) % MODULUS;
        }
    }
    close(file_descriptor);

    printf("всего: прочитано %d байт\n", read_bytes);
    printf("всего: успешно %d/%d байт\n", read_bytes - corrupted_bytes, read_bytes);

    exit(0);
}
