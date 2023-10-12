/*
 * Projet PCSEA RISC-V
 *
 * Benoît Wallon <benoit.wallon@grenoble-inp.org> - 2019
 * Mathieu Barbe <mathieu@kolabnow.com> - 2019
 *
 * See license for license details.
 */

#include "assert.h"
#include "riscv.h"

#include "traps/trap.h"
#include "timer.h"
#include "keyboard.h"

void strap_handler(uintptr_t scause, void *sepc, struct trap_frame *tf)
{
	DISABLE_SUPERVISOR_INTERRUPTS();
	if (scause & INTERRUPT_CAUSE_FLAG) {
		// Interruption cause
		uint8_t interrupt_number = scause & ~INTERRUPT_CAUSE_FLAG;
		switch (scause & ~INTERRUPT_CAUSE_FLAG) {
			case intr_s_timer:
				handle_stimer_interrupt();
				break;
			case intr_s_external: 
				handle_keyboard_interrupt();
				break;
			case intr_u_external:
				printf("User external interrupt");
			default:
				die(
						"supervisor mode: unhandlable interrupt trap %d : %s @ %p",
						interrupt_number, interruption_names[interrupt_number], sepc
				);
				break;
		}
	} else {
		// Exception cause
		switch (scause) {
			default:
				blue_screen(tf);
				// no return
		}
	}
}
