.data
msg_read_fail: .asciz "Read error\n"
msg_args_fail: .asciz "Error: Input should contain two numbers\n"
fmt_num: .asciz "%d\n"
buffer: .space 22

.text
.global main

main:
    la a0, buffer
    li a1, 22
    call gets

    beqz a0, read_fail

    la a0, buffer
    li a1, ' '
    call strchr

    beqz a0, args_fail

    sb zero, 0(a0)
    addi a0, a0, 1

    call atoi
    mv t2, a0

    call atoi
    mv t3, a0

    add t4, t2, t3

    la a0, fmt_num
    mv a1, t4
    call printf

    li a7, 10
    li a0, 0
    ecall

read_fail:
    li a7, 64
    li a0, 2
    la a1, msg_read_fail
    li a2, 18
    ecall

    li a7, 93
    li a0, 1
    ecall

args_fail:
    li a7, 64
    li a0, 2
    la a1, msg_args_fail
    li a2, 21
    ecall

    li a7, 93
    li a0, 1
    ecall
