#include <arch/apic.h>

#include <assert.h>
#include <stdio.h>

#define IPI_INIT		0x000C4500U
#define IPI_SIPI		0x000C4600U
#define SIPI_VECTOR(addr)	(((unsigned long) addr) >> 12)

extern void start16(void);

char smp_stack[4096] __attribute__ ((aligned(16)));

unsigned long smp_stack_top = smp_stack + 4096;

static int nr_cpus;
static int nr_cpus_online;

void register_cpu(uint32_t cpu_id)
{
	__atomic_add_fetch(&nr_cpus, 1, __ATOMIC_SEQ_CST);
}

void cpu_bringup(void)
{
	__atomic_add_fetch(&nr_cpus_online, 1, __ATOMIC_SEQ_CST);
}

void smp_init(void)
{
	assert(apic_is_bsp());

	/* The BSP is already up, let the kernel know about it.  */
	cpu_bringup();

	// Intel Manuals: Section 8.4 ("Multiple-Processor (MP) initialization")

	// 8.4.4.1 Typical BSP Initialization Sequence

	printf("Found %d CPUs...\n", nr_cpus);

	apic_ipi_allbutself(IPI_INIT);
	apic_ipi_allbutself(IPI_SIPI | SIPI_VECTOR(start16));
	apic_ipi_allbutself(IPI_SIPI | SIPI_VECTOR(start16));

	while (__atomic_load_n(&nr_cpus, __ATOMIC_SEQ_CST) != __atomic_load_n(&nr_cpus_online, __ATOMIC_SEQ_CST))
		;

	printf("All CPUs are online.\n");
}
