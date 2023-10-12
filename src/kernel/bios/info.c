#include "info.h"

#include "stdio.h"
#include "riscv.h"

void display_info_proc()
{
	display_misa_info();
}

void display_misa_info()
{
	uint64_t isa = csr_read(misa);
	uint32_t bits;

	printf("RISC-V ");
	if (isa & ((uint64_t) 3 << 62)) {
		bits = 64;
	} else if (isa & (3 << 30)) {
		bits = 32;
	} else {
		bits = 128;
	}
		printf("%d bits", bits);
	
	printf(" Extension RV%d", bits);
	if (isa & (1 << 8)) {
		printf("I");
	}
	if (isa & (1 << 12)) {
		printf("M");
	}
	if (isa & (1 << 0)) {
		printf("A");
	}
	if (isa & (1 << 1)) {
		printf("B");
	}
	if (isa & (1 << 2)) {
		printf("C");
	}
	if (isa & (1 << 3)) {
		printf("D");
	}
	if (isa & (1 << 4)) {
		printf("E");
	}
	if (isa & (1 << 5)) {
		printf("F");
	}
	if (isa & (1 << 6)) {
		printf("G");
	}
	if (isa & (1 << 7)) {
		printf("H");
	}
	if (isa & (1 << 9)) {
		printf("J");
	}
	if (isa & (1 << 10)) {
		printf("K");
	}
	if (isa & (1 << 11)) {
		printf("L");
	}
	if (isa & (1 << 13)) {
		printf("N");
	}
	if (isa & (1 << 14)) {
		printf("O");
	}
	if (isa & (1 << 15)) {
		printf("P");
	}
	if (isa & (1 << 16)) {
		printf("Q");
	}
	if (isa & (1 << 17)) {
		printf("R");
	}
	if (isa & (1 << 18)) {
		printf("S");
	}
	if (isa & (1 << 19)) {
		printf("T");
	}
	if (isa & (1 << 20)) {
		printf("U");
	}
	if (isa & (1 << 21)) {
		printf("V");
	}
	if (isa & (1 << 22)) {
		printf("W");
	}
	if (isa & (1 << 23)) {
		printf("X");
	}
	if (isa & (1 << 24)) {
		printf("Y");
	}
	if (isa & (1 << 25)) {
		printf("Z");
	}

	printf("\n");
}
