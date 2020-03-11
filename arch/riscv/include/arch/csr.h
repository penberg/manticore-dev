#ifndef __MANTICORE_RISCV_CSR_H
#define __MANTICORE_RISCV_CSR_H

#define SR_SIE 0 /* FIXME */

#define read_csr(reg)			\
({					\
 	unsigned long __val;		\
	asm volatile(			\
		"csrr %0, " #reg	\
		: "=r" (__val)	 	\
		:			\
		: "memory");		\
	__val;				\
})

#define set_csr_bits(reg, bits)

#define clear_csr_bits(reg, bits)

#define read_and_clear_csr(reg, bits)	\
({					\
 	0;				\
})

#endif
