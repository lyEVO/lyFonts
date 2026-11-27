#ifndef LYSIS_SETJMP_H
#define LYSIS_SETJMP_H

/* room for the callee-saved registers on x86-64 and AArch64 */
typedef long jmp_buf[32];

int setjmp(jmp_buf env) __attribute__((returns_twice));
void longjmp(jmp_buf env, int val) __attribute__((noreturn));

#endif
