.text
.globl context_switch
.align 2
context_switch:
	# Sauvegarde du contexte de l'ancien processus
	beqz a0, skip_save
	sd sp,    0(a0)
	sd ra,    8(a0)
	sd s0,   16(a0)
	sd s1,   24(a0)
	sd s2,   32(a0)
	sd s3,   40(a0)
	sd s4,   48(a0)
	sd s5,   56(a0)
	sd s6,   64(a0)
	sd s7,   72(a0)
	sd s8,   80(a0)
	sd s9,   88(a0)
	sd s10,  96(a0)
	sd s11, 104(a0)
	csrr t0, satp
	sd t0, 112(a0)
	csrr t0, sstatus
	sd t0, 120(a0)
	csrr t0, sepc
	sd t0, 128(a0)
	csrr t0, sscratch
	sd t0, 136(a0)

skip_save:
	# Restauration du contexte
	ld sp,    0(a1)
	ld ra,    8(a1)
	ld s0,   16(a1)
	ld s1,   24(a1)
	ld s2,   32(a1)
	ld s3,   40(a1)
	ld s4,   48(a1)
	ld s5,   56(a1)
	ld s6,   64(a1)
	ld s7,   72(a1)
	ld s8,   80(a1)
	ld s9,   88(a1)
	ld s10,  96(a1)
	ld s11, 104(a1)

	# restore sscratch
	ld t0, 136(a1)
	csrw sscratch, t0
	# restore sstatus
	ld t0, 120(a1)
	csrw sstatus, t0
	# restore sepc
	ld t0, 128(a1)
	csrw sepc, t0
	# restore satp
	ld t0, 112(a1)

	ld a0,   16(a1)		# On passe le nouveau processus en argument de start_process
	ld a1,   24(a1)		# On passe les arguments du processus en deuxième argument de start_process
	
	csrw satp, t0

	# sret # ne doit pas être là
	# On passe la main au nouveau processus
	jr ra
