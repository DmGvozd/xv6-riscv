#include <stdarg.h>
#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "queue.h"

struct queue buffer;

void buffer_init(void) {
    initlock(&buffer.lock, "queue_lock");
    buffer.head = 0;
    buffer.tail = 0;
}

void pr_msg(char *fmt, ...) {
    va_list ap;
    int i, c;

    queue_write(&buffer, '[');

    queue_write(&buffer, '0' + ticks / 10000 % 10);
    queue_write(&buffer, '0' + ticks / 1000 % 10);
    queue_write(&buffer, '0' + ticks / 100 % 10);
    queue_write(&buffer, '0' + ticks / 10 % 10);
    queue_write(&buffer, '0' + ticks % 10);

    queue_write(&buffer, ']');
    queue_write(&buffer, ' ');

    if (fmt == 0)
        panic("pr_msg: null fmt");

    va_start(ap, fmt);

    static const char hex_chars[] = "0123456789abcdef";

    for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
        if (c != '%') {
            queue_write(&buffer, c);
            continue;
        }

        c = fmt[++i] & 0xff;
        if (c == 0)
            break;

        switch(c) {
            case 'd':
                {
                    int num = va_arg(ap, int);
                    if (num < 0) {
                        queue_write(&buffer, '-');
                        num = -num;
                    }
                    char num_str[16];
                    int j = 0;
                    do {
                        num_str[j++] = '0' + num % 10;
                        num /= 10;
                    } while (num);
                    while (j > 0) {
                        queue_write(&buffer, num_str[--j]);
                    }
                }
                break;
            case 'x':
                {
                    int num = va_arg(ap, int);
                    char hex_str[16];
                    int j = 0;
                    do {
                        hex_str[j++] = hex_chars[num % 16];
                        num /= 16;
                    } while (num);
                    while (j > 0) {
                        queue_write(&buffer, hex_str[--j]);
                    }
                }
                break;
            case 'p':
                {
                    int num = va_arg(ap, int);
                    queue_write(&buffer, '0');
                    queue_write(&buffer, 'x');
                    char hex_ptr[16];
                    int k = 0;
                    do {
                        hex_ptr[k++] = hex_chars[num % 16];
                        num /= 16;
                    } while (num);
                    while (k > 0) {
                        queue_write(&buffer, hex_ptr[--k]);
                    }
                }
                break;
            case 's':
                {
                    char *s = va_arg(ap, char *);
                    if (s == 0)
                        s = "(null)";

                    while (*s) {
                        queue_write(&buffer, *s++);
                    }
                }
                break;
            case '%':
                queue_write(&buffer, '%');
                break;
            default:
                queue_write(&buffer, '%');
                queue_write(&buffer, c);
                break;
        }
    }
    va_end(ap);

    queue_write(&buffer, '\n');
}

uint64 sys_dmesg(void) {
    char zero = 0;

    uint64 addr;
    argaddr(0, &addr);

    uint64 n;
    argaddr(1, &n);

    if (addr == 0)
        return -1;

    acquire(&buffer.lock);

    int i = 0, h = buffer.head;
    while (i + 1 < n && h != buffer.tail) {
        if (h == BUFFER_SIZE)
            h = 0;

        if (copyout(myproc()->pagetable, addr + i, &buffer.data[h], 1) < 0) {
            release(&buffer.lock);
            return -1;
        }

        h++;
        i++;
    }

    if (copyout(myproc()->pagetable, addr + i, &zero, 1) < 0) {
        release(&buffer.lock);
        return -1;
    }

    release(&buffer.lock);
    return 0;
}

