/*

Memory management

The memory management unit (MMU) in the VMSAv8-64 address translation system is
responsible for address translation and access control. To perform these
functions, the MMU maintains a multi-level address translation table containing
descriptors for tables, blocks, and pages.

The MMU maintains two address translation tables per exception level, which are
configured via the TTBR0 and TTBR1 registers. The translation table specified
by TTRB0 is used when the higher bits of a VA are all set to zero. TTRB1 is
used when the bits are set to one. The TCR register controls the address
translation configuration.

The mmu_map_t represents the two address tables TTBR0 amnd TTBR1 where TTRB0
represents the lower half and TTRB1 represents the higher half of the
virtual address space. To mitigate against Meltdown, TTBR1 is
substituted with a trampoline address translation table when entering
user space.

TODO:

- TLB management
- How do we need to mitigate against speclative side-channel attacks such as
  Meltdown?

*/

#include <kernel/mmu.h>

#include <kernel/errno.h>
#include <arch/processor.h>

phys_t virt_to_phys(virt_t addr)
{
	return addr - KERNEL_VMA;
}

virt_t phys_to_virt(phys_t addr)
{
	return addr + KERNEL_VMA;
}

typedef struct {
	uint64_t table_desc;
} table_desc_t;

typedef struct {
	uint64_t block_desc;
} block_desc_t;

typedef struct {
	uint64_t page_desc;
} page_desc_t;

/* MCR: */
#define TBI1	(1ULL << 38)
#define TBI0	(1ULL << 37)
#define AS	(1ULL << 36)
/* TODO: IPS */
#define TG1_4K		(0b10ULL << 30)
#define SH1_INNER	(0b11ULL << 28)
#define ORGN1_		(0b01LLL << 26)
#define IRGN1_		(0b01ULL << 24)
#define T1SZ_		(25ULL << 16)
#define TG0_4K		(0b10ULL << 14)
#define SH0_INNER	(0b11ULL << 12)
#define ORGN0_		(0b01LLL << 10)
#define IRGN0_		(0b01ULL << 8)
#define T0SZ_		(25ULL << 0)

/* Translation table: */
#define UXN		(1ULL << 54) /* Unprivileged execute-never */
#define XN		(1ULL << 54) /* Execute-never */
#define PXN		(1ULL << 53) /* Privileged execute-never */
#define CONTIGUOUS	(1ULL << 52) /* Contiguous */
#define DBM		(1ULL << 51) /* Dirty bit modifier */
#define AF		(1ULL << 10) /* Access flag */
#define AP_RO		(1ULL << 7) /* Access permissions: read-only */
#define AP_EL0		(1ULL << 6) /* Access permissions: EL0 */

enum granule_size {
	GRANULE_SIZE_4KB,
	GRANULE_SIZE_16KB,
	GRANULE_SIZE_64KB,
};

static table_desc_t make_table_desc(phys_t paddr)
{
	return (table_desc_t){table_desc: paddr | 0b01};
}

static block_desc_t make_block_desc(phys_t paddr)
{
	return (block_desc_t){block_desc: paddr | 0b11};
}

static page_desc_t make_page_desc(phys_t paddr)
{
	return (page_desc_t){page_desc: paddr | 0b11};
}

mmu_map_t mmu_current_map(void)
{
	uint64_t ttbr0 = mrs(ttbr0_el1);
	uint64_t ttbr1 = mrs(ttbr1_el1);

	return (mmu_map_t){ttbr0: ttbr0, ttbr1: ttbr1};
}

void mmu_load_map(mmu_map_t map)
{
	msr(ttbr0_el1, map.ttbr0);
	msr(ttbr1_el1, map.ttbr1);
}

int mmu_map_small_page(mmu_map_t map, virt_t vaddr, phys_t paddr, mmu_prot_t prot, mmu_flags_t flags)
{
	/* FIXME: not implemented.  */
	return -EINVAL;
}

int mmu_map_large_page(mmu_map_t map, virt_t vaddr, phys_t paddr, mmu_prot_t prot, mmu_flags_t flags)
{
	/* FIXME: not implemented.  */
	return -EINVAL;
}

void mmu_map_dump(mmu_map_t map)
{
}
