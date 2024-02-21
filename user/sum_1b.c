#include "kernel/types.h"
#include "user/user.h"

#define MAX_INPUT_SIZE 20

int main() {
    char buffer[MAX_INPUT_SIZE];
    int num1 = 0, num2 = 0;

    char *result = gets(buffer, MAX_INPUT_SIZE);

    if (!result) {
        fprintf(2, "Error: Read error\n");
        exit(1);
    }

    // Проверка на пустую строку
    if (buffer[0] == '\0') {
        fprintf(2, "Error: Empty input\n");
        return 1;
    }

    // Поиск пробела для разделения чисел
    int space_index = -1;
    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == ' ') {
            space_index = i;
            break;
        }
    }

    // Проверка на наличие пробела
    if (space_index == -1) {
        fprintf(2, "Error: Input should contain two numbers\n");
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

    return 0;
}
