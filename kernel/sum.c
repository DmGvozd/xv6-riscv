#include "types.h"
#include "riscv.h"
#include "defs.h"

uint64 sys_sum(void) {
  int num1, num2;

  argint(0, &num1);
  argint(1, &num2);

  return num1 + num2;
}
