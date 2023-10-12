/*
 * Projet PCSEA RISC-V
 *
 * Benoît Wallon <benoit.wallon@grenoble-inp.org> - 2019
 * Mathieu Barbe <mathieu@kolabnow.com> - 2019
 *
 * See license for license details.
 */

#include "stdlib.h"
#include "assert.h"
#include "riscv.h"
#include "timer.h"
#include "keyboard.h"
#include "bios/info.h"
#include "traps/trap.h"
#include "mem.h"

/*
 * Prototypes externes
 */
// kernel/start.c
extern int kernel_start();

// Specific machine setup: kernel/boot/$(MACHINE)/setup.c
extern void arch_setup();

extern void strap_entry();

static void delegate_traps()
{

	/*
	 * Délégations de certaines interruptions et exceptions vers le mode Supervisor.
	 * A compléter au fur et à mesure du projet celon les besoins.
	 * Rien à faire ici dans un premier temps!
	 * CSR concernés: mideleg et medeleg.
	 */
	csr_write(medeleg, 1UL << CAUSE_USER_ECALL);
	csr_set(mideleg, MIP_SSIP);
	csr_set(mideleg, MIP_STIP);
	csr_set(mideleg, MIP_SEIP);
}

static inline void setup_pmp(void)
{
	/*
	 * Désactivation de la protection de la mémoire physique (PMP).
	 *
	 * Configuration de la PMP pour donner  un accès complet à la mémoire.
	 * CSR concernés: pmpaddr0 and pmpcfg0.
	 */

	// Ignore the illegal-instruction trap if PMPs aren't supported.
	uintptr_t pmpc = PMP_NAPOT | PMP_R | PMP_W | PMP_X;
	uintptr_t pmpa = ((uintptr_t)1 << (__riscv_xlen == 32 ? 31 : 53)) - 1;
	__asm__ __volatile__("la t0, 1f\n\t"
						 "csrrw t0, mtvec, t0\n\t"
						 "csrw pmpaddr0, %1\n\t"
						 "csrw pmpcfg0, %0\n\t"
						 ".align 2\n\t"
						 "1: csrw mtvec, t0"
						 :
						 : "r"(pmpc), "r"(pmpa)
						 : "t0");
}

static inline void enter_supervisor_mode()
{

	// Il faut obligatoirement configurer la protection de la mémoire physique avant de passer en mode supervisor.
	setup_pmp();

	/*
	 * Configuration du mode à utiliser lors de l'instruction mret.
	 *
	 * CSR concernés: mepc et mstatus.
	 * Voir aussi riscv.h pour la macro mret().
	 */

	// Configuration du registre mepc qui indique ce que l'on va exécuter en mode supervisor,
	csr_write(mepc, kernel_start);

	// désactiver les interruptions dans le mode où l'on se rend (mstatus également),
	csr_clear(mstatus, MSTATUS_SIE);
	//  pas sûr que ça soit utile + utiliser DISABLE_SUPERVISOR_INTERRUPTS() à la place

	// indication du mode dans lequel on doit se rendre (mstatus),
	csr_clear(mstatus, MSTATUS_MPP);
	csr_set(mstatus, MSTATUS_MPP_S);

	csr_write(stvec, (uintptr_t)strap_entry);

	void *pile = malloc(1024);

	csr_write(sscratch, (uintptr_t)pile + 1024);

	// lancement du kernel kernel_start à l'aide de l'instruction mret.
	mret();
}

// memory_tables_t kernel_memory;
// void* KERNEL_TABLE_ADDR;
// extern char _free_memory_start[];

/*
 * boot_riscv
 *
 * Cette fonction est appelée depuis crtm.S
 * A ce stade, seul le vecteur de trap machine mtvec a été configuré.
 * La pile en cours d'utilisation est la pile machine qui a été alouée dans crtm.S.
 * Cette pile est également utilisée pour traiter les traps machine.
 *
 * Le processeur est toujours en mode machine
 */
__attribute__((noreturn)) void boot_riscv()
{
	// KERNEL_TABLE_ADDR = (void*) _free_memory_start;

	// // Création de la table du kernel
	// kernel_memory = KERNEL_TABLE_ADDR;
	// kernel_memory[0] = (table_entry_t){
	// 	.fields = {
	// 		.valid = 1,
	// 		.write = 1,
	// 		.read = 1,
	// 		.execute = 1,
	// 		.osef = 0,
	// 		.ppn0 = 0,
	// 		.ppn1 = 0,
	// 		.ppn2 = 0}};

	// // Mémoire vidéo
	// kernel_memory[2] = (table_entry_t){
	// 	.fields = {
	// 		.valid = 1,
	// 		.write = 1,
	// 		.read = 1,
	// 		.execute = 1,
	// 		.osef = 0,
	// 		.ppn0 = 0,
	// 		.ppn1 = 0,
	// 		.ppn2 = 2}};
	// change_satp(kernel_memory);
	// Configuration des composants spécifiques à la machine (uart / htif, timer et interruptions externes).
	arch_setup();

	display_info_proc();

	// Délégations des interruptions et des exceptions
	delegate_traps();
	init_clock(); // on init la clock machine avant de passer en mode supervisor
	init_keyboard();
	enter_supervisor_mode();
	exit(kernel_start());
	__builtin_unreachable();
}
