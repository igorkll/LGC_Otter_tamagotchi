#include "funcs.h"

int slnprintf(char *str, size_t size, const char *format, ...) {
    int result;
    va_list args;

    va_start(args, format);
    result = vsnprintf(str, size, format, args);
    va_end(args);

    // если размер итоговой строки привышает size
    // стандартная snprintf не записывает \0
    // данная функция исправляет это
    if (size > 0) {
        str[size - 1] = '\0';
    }

    return result;
}
