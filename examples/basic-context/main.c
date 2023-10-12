/*
 * Projet PCSEA RISC-V
 * Mathieu Barbe <mathieu@kolabnow.com> - 2019
 * Frédéric Pétrot <frederic.petrot@univ-grenoble-alpes.fr> - 2022
 * See license for license details.
 */

#include <stdio.h>
#include "context.h"

#define MKTHREAD(x) \
void thread_##x(void); \
uint64_t stack_##x[256]; \
context_t context_##x = {(uint64_t)(stack_##x + 256), (uint64_t)thread_##x, (uint64_t)"I'm thread " #x, 0xdeadbeef};

#define UART_TX_ADDR (volatile uint32_t *)0x10013000

void outs(char *s)
{
    while (*s) {
        *UART_TX_ADDR = *s++;
    }
    *UART_TX_ADDR = '\n';
}

MKTHREAD(x)
MKTHREAD(y)
MKTHREAD(z)

void thread_x(void)
{
    int x = 0;
    register char *str __asm__("s0");
    while (1) {
        x += 1;
        outs(str);
        context_switch(&context_x, &context_y);
    }
}

void thread_y(void)
{
    int x = 1;
    char *str;
    while (1) {
        x *= 2;
        __asm__("mv %0, s0":"=r"(str));
        outs(str);
        context_switch(&context_y, &context_z);
    }
}

void thread_z(void)
{
    int x = 0;
    char *str;
    while (1) {
        x = x & 1 ? x * 2 : x + 1  ;
        __asm__("mv %0, s0":"=r"(str));
        outs(str);
        context_switch(&context_z, &context_x);
    }
}

int main(void)
{
    context_t main_ctx;
    context_switch(&main_ctx, &context_x);
    /* De fait jamais atteint */
    return 0;
}
