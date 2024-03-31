#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define BUFFER_SIZE 22

int main() {
    char buffer[BUFFER_SIZE];
    int num1 = 0, num2 = 0;
    int space_index = -1;
    int bytes_read = 0;
    int index = 0;
    char c;

    while (1) {
        bytes_read = read(0, &c, 1);
        if (bytes_read < 0) {
            fprintf(1, "Error: Read error\n");
            exit(1);
        } else if (bytes_read == 0) {
            break;
        }

        if (c == '\n') {
            buffer[index] = '\0';
            break;
        }

        if (index == BUFFER_SIZE - 1) {
            fprintf(1, "Error: Buffer overflow\n");
            exit(1);
        }

        buffer[index] = c;
        index++;
    }

    // Поиск пробела для разделения чисел
    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == ' ') {
            space_index = i;
            break;
        }
    }

    // Проверка на наличие пробела
    if (space_index == -1) {
        fprintf(1, "Error: Input should contain two numbers\n");
        exit(1);
    }

    // Конвертация первого числа
    char num1_str[space_index + 1];
    for (int i = 0; i < space_index; i++) {
        num1_str[i] = buffer[i];
    }
    num1_str[space_index] = '\0';
    num1 = atoi(num1_str);

    // Конвертация второго числа
    num2 = atoi(buffer + space_index + 1);

    printf("%d\n", num1 + num2);

    exit(0);
}
