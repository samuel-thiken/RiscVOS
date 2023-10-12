#pragma once

#include "stdint.h"

#include "encoding.h"

#define csr_read(csr)														\
({																			\
	register uint64_t __v;													\
	__asm__ __volatile__ ("csrr %0, " #csr : "=r" (__v) : : "memory");		\
	__v;																	\
})

#define csr_write(csr, value)												\
({																			\
	register uint64_t __v = (uint64_t)(value);								\
	__asm__ __volatile__ ("csrw " #csr ", %0" : : "rK" (__v) : "memory");	\
})

// value is a bit mask that specifies bit position to be set (high bit)
#define csr_set(csr, value)													\
({																			\
	register uint64_t __v = (uint64_t)(value);								\
	__asm__ __volatile__ ("csrs " #csr ", %0" : : "rK" (__v) : "memory");	\
})

// value is a bit mask that specifies bit position to be cleared (high bit)
#define csr_clear(csr, value)												\
({																			\
	register uint64_t __v = (uint64_t)(value);								\
	__asm__ __volatile__ ("csrc " #csr ", %0" : : "rK" (__v) : "memory");	\
})

static inline void wfi()
{
   __asm__ __volatile__ ("wfi" ::: "memory");
}

__attribute__((noreturn)) static inline void mret()
{
	__asm__ __volatile__ ("mret");
	__builtin_unreachable();
}

__attribute__((noreturn)) static inline void sret()
{
	__asm__ __volatile__ ("sret");
	__builtin_unreachable();
}
