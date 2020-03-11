#ifndef __MANTICORE_RISCV_VMEM_H
#define __MANTICORE_RISCV_VMEM_H

/// Physical address.
typedef unsigned long phys_t;

/// Virtual address.
typedef unsigned long virt_t;

/// Converts a virtual address to a physical address.
phys_t virt_to_phys(virt_t addr);

#endif
