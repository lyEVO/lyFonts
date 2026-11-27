/*
 *  This file is part of lysis fonts.
 *  Copyright © 2026 lysis evolutions
 *
 *  https://github.com/lyEVO/lyFonts
 */
#include <setjmp.h>

#if defined(ARCH_ARM64)

__attribute__((naked, returns_twice)) int setjmp(jmp_buf env)
{
    __asm__ volatile (
        "str x19, [x0, #0]\n\t"
        "str x20, [x0, #8]\n\t"
        "str x21, [x0, #16]\n\t"
        "str x22, [x0, #24]\n\t"
        "str x23, [x0, #32]\n\t"
        "str x24, [x0, #40]\n\t"
        "str x25, [x0, #48]\n\t"
        "str x26, [x0, #56]\n\t"
        "str x27, [x0, #64]\n\t"
        "str x28, [x0, #72]\n\t"
        "str x29, [x0, #80]\n\t"
        "str x30, [x0, #88]\n\t"
        "mov x1, sp\n\t"
        "str x1, [x0, #96]\n\t"
        "str d8, [x0, #104]\n\t"
        "str d9, [x0, #112]\n\t"
        "str d10, [x0, #120]\n\t"
        "str d11, [x0, #128]\n\t"
        "str d12, [x0, #136]\n\t"
        "str d13, [x0, #144]\n\t"
        "str d14, [x0, #152]\n\t"
        "str d15, [x0, #160]\n\t"
        "mov x0, #0\n\t"
        "ret\n\t"
    );
}

__attribute__((naked, noreturn)) void longjmp(jmp_buf env, int val)
{
    __asm__ volatile (
        "ldr x19, [x0, #0]\n\t"
        "ldr x20, [x0, #8]\n\t"
        "ldr x21, [x0, #16]\n\t"
        "ldr x22, [x0, #24]\n\t"
        "ldr x23, [x0, #32]\n\t"
        "ldr x24, [x0, #40]\n\t"
        "ldr x25, [x0, #48]\n\t"
        "ldr x26, [x0, #56]\n\t"
        "ldr x27, [x0, #64]\n\t"
        "ldr x28, [x0, #72]\n\t"
        "ldr x29, [x0, #80]\n\t"
        "ldr x30, [x0, #88]\n\t"
        "ldr x2, [x0, #96]\n\t"
        "ldr d8, [x0, #104]\n\t"
        "ldr d9, [x0, #112]\n\t"
        "ldr d10, [x0, #120]\n\t"
        "ldr d11, [x0, #128]\n\t"
        "ldr d12, [x0, #136]\n\t"
        "ldr d13, [x0, #144]\n\t"
        "ldr d14, [x0, #152]\n\t"
        "ldr d15, [x0, #160]\n\t"
        "mov sp, x2\n\t"
        "cmp w1, #0\n\t"
        "mov w2, #1\n\t"
        "csel w0, w1, w2, ne\n\t"
        "ret\n\t"
    );
}

#elif defined(ARCH_X64)

__attribute__((naked, returns_twice)) int setjmp(jmp_buf env)
{
    __asm__ volatile (
        "movq %rbx, 0(%rdi)\n\t"
        "movq %rbp, 8(%rdi)\n\t"
        "movq %r12, 16(%rdi)\n\t"
        "movq %r13, 24(%rdi)\n\t"
        "movq %r14, 32(%rdi)\n\t"
        "movq %r15, 40(%rdi)\n\t"
        "leaq 8(%rsp), %rax\n\t"
        "movq %rax, 48(%rdi)\n\t"
        "movq (%rsp), %rax\n\t"
        "movq %rax, 56(%rdi)\n\t"
        "xorl %eax, %eax\n\t"
        "ret\n\t"
    );
}

__attribute__((naked, noreturn)) void longjmp(jmp_buf env, int val)
{
    __asm__ volatile (
        "movq 0(%rdi), %rbx\n\t"
        "movq 8(%rdi), %rbp\n\t"
        "movq 16(%rdi), %r12\n\t"
        "movq 24(%rdi), %r13\n\t"
        "movq 32(%rdi), %r14\n\t"
        "movq 40(%rdi), %r15\n\t"
        "movq 56(%rdi), %rcx\n\t"
        "movq 48(%rdi), %rsp\n\t"
        "movl %esi, %eax\n\t"
        "movl $1, %edx\n\t"
        "testl %eax, %eax\n\t"
        "cmove %edx, %eax\n\t"
        "jmp *%rcx\n\t"
    );
}

#endif
