#ifndef RISCV_INTERRUPTS_H
#define RISCV_INTERRUPTS_H

#include <arch/csr.h>

#include <stdbool.h>

/// Disables local interrupts.
static inline void arch_local_interrupt_disable(void)
{
	clear_csr_bits(sstatus, SR_SIE);
}

/// Enables local interrupts.
static inline void arch_local_interrupt_enable(void)
{
	set_csr_bits(sstatus, SR_SIE);
}

/// Returns the current local interrupt mask.
///
/// \return the current local interrupt mask.
static inline unsigned long arch_local_interrupt_mask(void)
{
	return read_csr(sstatus);
}

/// Disables local interrupts and returns prior interrupt mask.
///
/// \return the prior local interrupt mask (before interrupts were disabled).
static inline unsigned long arch_local_interrupt_save(void)
{
	return read_and_clear_csr(sstatus, SR_SIE);
}

/// Restores local interrupt mask from \flags.
///
/// This function either enables or disabled local interrupts depending on the
/// interrupt mask specified in \flags.
///
/// \param flags Interrupt mask.
static inline void arch_local_interrupt_restore(unsigned long flags)
{
	set_csr_bits(sstatus, flags & SR_SIE);
}

/// Returns `true` if local interrupts are disabled; otherwise returns `false`.
///
/// \param flags The interrupt mask.
/// \return `true` if local interrupts are disabled; otherwise returns `false`.
static inline bool arch_local_interrupt_is_disabled(unsigned long flags)
{
	return !(flags & SR_SIE);
}

#endif
