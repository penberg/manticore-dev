#include "acpi.h"
#include "acpiosxf.h"
#include "acpixf.h"

#include <kernel/align.h>
#include <kernel/mmu.h>
#include <kernel/page-alloc.h>
#include <kernel/printf.h>

static bool acpi_late = false;

// --------------------------------------------------------------------------
// Environmental and ACPI Tables
// --------------------------------------------------------------------------

ACPI_STATUS AcpiOsInitialize(void)
{
	// printf("%s\n", __func__);
	return AE_OK;
}

ACPI_STATUS AcpiOsTerminate(void)
{
	panic("AcpiOsTerminate");
	return AE_OK;
}

extern void *rsdp_ptr;

ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer(void)
{
// printf("%s\n", __func__);
#if 0
	ACPI_PHYSICAL_ADDRESS Ret;
	Ret = 0;
	AcpiFindRootPointer(&Ret);
	return Ret;
#else
	return virt_to_phys(rsdp_ptr);
#endif
}

ACPI_STATUS AcpiOsPredefinedOverride(const ACPI_PREDEFINED_NAMES *InitVal, ACPI_STRING *NewVal)
{
	// printf("%s\n", __func__);
	*NewVal = NULL;
	return AE_OK;
}

ACPI_STATUS AcpiOsTableOverride(ACPI_TABLE_HEADER *ExistingTable, ACPI_TABLE_HEADER **NewTable)
{
	// printf("%s\n", __func__);
	*NewTable = NULL;
	return AE_OK;
}

ACPI_STATUS AcpiOsPhysicalTableOverride(ACPI_TABLE_HEADER *ExistingTable, ACPI_PHYSICAL_ADDRESS *NewAddress,
					UINT32 *NewTableLength)
{
	// printf("%s\n", __func__);
	*NewAddress = NULL;
	*NewTableLength = NULL;
	return AE_OK;
}

// --------------------------------------------------------------------------
// Memory Management
// --------------------------------------------------------------------------

void *AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS Where, ACPI_SIZE Length)
{
	printf("%s(Where=%x, Length=%ld)\n", __func__, Where, Length);
	if (acpi_late) {
		mmu_map_t mmap = mmu_current_map();
		phys_t addr = align_down(Where, PAGE_SIZE_SMALL);
		size_t size = align_up(Length + Where - addr, PAGE_SIZE_SMALL);
#if 1
		printf(" -> addr=%x, size=%d\n", addr, size);
#endif
		int err = mmu_map_range(mmap, phys_to_virt(addr), addr, size, MMU_PROT_READ, 0);
		if (err < 0) {
			panic("mmu_map_range");
		}
		mmu_invalidate_tlb();
	}
	return phys_to_virt(Where);
}

void AcpiOsUnmapMemory(void *LogicalAddress, ACPI_SIZE Size)
{
#if 0 // FIXME: mmu_unmap_range() is missing
	panic("AcpiOsUnmapMemory");
#endif
}

void *AcpiOsAllocate(ACPI_SIZE Size)
{
	// printf("%s\n", __func__);
	return kmem_alloc(Size);
}

void AcpiOsFree(void *Memory)
{
// printf("%s\n", __func__);
#if 0 // FIXME: no kmem that frees without knowing size!
	panic("AcpiOsFree");
#endif
}

BOOLEAN AcpiOsReadable(void *Pointer, ACPI_SIZE Length)
{
	panic("AcpiOsReadable");
	// printf("%s\n", __func__);
	return TRUE;
}

BOOLEAN AcpiOsWritable(void *Pointer, ACPI_SIZE Length)
{
	// printf("%s\n", __func__);
	return TRUE;
}

// --------------------------------------------------------------------------
// Multithreading and Scheduling Services
// --------------------------------------------------------------------------

ACPI_THREAD_ID AcpiOsGetThreadId(void)
{
	// printf("%s\n", __func__);
	return 1;
}

ACPI_STATUS AcpiOsExecute(ACPI_EXECUTE_TYPE Type, ACPI_OSD_EXEC_CALLBACK Function, void *Context)
{
	panic("AcpiOsExecute");
	return AE_NOT_IMPLEMENTED;
}

void AcpiOsSleep(UINT64 Milliseconds)
{
	panic("AcpiOsSleep");
}

void AcpiOsStall(UINT32 Microseconds)
{
	panic("AcpiOsStall");
}

void AcpiOsWaitEventsComplete(void)
{
	panic("AcpiOsWaitEventsComplete");
}

// --------------------------------------------------------------------------
// Mutual Exclusion and Synchronization
// --------------------------------------------------------------------------

ACPI_STATUS AcpiOsCreateSemaphore(UINT32 MaxUnits, UINT32 InitialUnits, ACPI_SEMAPHORE *OutHandle)
{
	// printf("%s(MaxUnits=%d, InitialUnits=%d)\n", __func__, MaxUnits, InitialUnits);
	*OutHandle = 1; // FIXME
	return AE_OK;
}

ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_SEMAPHORE Handle)
{
	panic("AcpiOsDeleteSemaphore");
	if (!Handle)
		return AE_BAD_PARAMETER;
	return AE_OK;
}

ACPI_STATUS AcpiOsWaitSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units, UINT16 Timeout)
{
// printf("%s\n", __func__);
// FIXME
#if 0
	if (!Handle) {
		return AE_BAD_PARAMETER;
	}
#endif
	return AE_OK;
}

ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units)
{
	// printf("%s\n", __func__);
	if (!Handle)
		return AE_BAD_PARAMETER;
	return AE_OK;
}

ACPI_STATUS AcpiOsCreateLock(ACPI_SPINLOCK *OutHandle)
{
	// printf("%s\n", __func__);
	*OutHandle = 1; // FIXME
	return AE_OK;
}

void AcpiOsDeleteLock(ACPI_SPINLOCK Handle)
{
	panic("AcpiOsDeleteLock");
}

ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_SPINLOCK Handle)
{
	// printf("%s\n", __func__);
	return 0;
}

void AcpiOsReleaseLock(ACPI_SPINLOCK Handle, ACPI_CPU_FLAGS Flags)
{
	// printf("%s\n", __func__);
}

// --------------------------------------------------------------------------
// Interrupt Handling
// --------------------------------------------------------------------------

ACPI_STATUS AcpiOsInstallInterruptHandler(UINT32 InterruptNumber, ACPI_OSD_HANDLER ServiceRoutine, void *Context)
{
	panic("AcpiOsInstallInterruptHandler");
}

ACPI_STATUS
AcpiOsRemoveInterruptHandler(UINT32 InterruptNumber, ACPI_OSD_HANDLER ServiceRoutine)
{
	panic("AcpiOsRemoveInterruptHandler");
}

// --------------------------------------------------------------------------
// Memory Access and Memory Mapped I/O
// --------------------------------------------------------------------------

ACPI_STATUS
AcpiOsReadMemory(ACPI_PHYSICAL_ADDRESS Address, UINT64 *Value, UINT32 Width)
{
	panic("AcpiOsReadMemory");
}

ACPI_STATUS
AcpiOsWriteMemory(ACPI_PHYSICAL_ADDRESS Address, UINT64 Value, UINT32 Width)
{
	panic("AcpiOsWriteMemory");
}

// --------------------------------------------------------------------------
// Port Input/Output
// --------------------------------------------------------------------------

ACPI_STATUS AcpiOsReadPort(ACPI_IO_ADDRESS Address, UINT32 *Value, UINT32 Width)
{
	panic("AcpiOsReadPort");
}

ACPI_STATUS AcpiOsWritePort(ACPI_IO_ADDRESS Address, UINT32 Value, UINT32 Width)
{
	panic("AcpiOsWritePort");
}

// --------------------------------------------------------------------------
// PCI Configuration Space Access
// --------------------------------------------------------------------------

ACPI_STATUS
AcpiOsReadPciConfiguration(ACPI_PCI_ID *PciId, UINT32 Reg, UINT64 *Value, UINT32 Width)
{
	panic("AcpiOsReadPciConfiguration");
}

ACPI_STATUS
AcpiOsWritePciConfiguration(ACPI_PCI_ID *PciId, UINT32 Reg, UINT64 Value, UINT32 Width)
{
	panic("AcpiOsWritePciConfiguration");
}

// --------------------------------------------------------------------------
// Formatted Output
// --------------------------------------------------------------------------

void ACPI_INTERNAL_VAR_XFACE AcpiOsPrintf(const char *Format, ...)
{
	va_list va;
	va_start(va, Format);
	AcpiOsVprintf(Format, va);
	va_end(va);
}

void console_putc(void *unused, char ch);

void AcpiOsVprintf(const char *Format, va_list Args)
{
	vprintf(Format, Args);
}

// --------------------------------------------------------------------------
// Miscellaneous
// --------------------------------------------------------------------------

UINT64 AcpiOsGetTimer(void)
{
	panic("AcpiOsGetTimer");
	return 0;
}

ACPI_STATUS AcpiOsSignal(UINT32 Function, void *Info)
{
	panic("AcpiOsSignal");
}

ACPI_STATUS
AcpiOsEnterSleep(UINT8 SleepState, UINT32 RegaValue, UINT32 RegbValue)
{
	panic("AcpiOsWaitCommandReady");
	return (AE_OK);
}

void AcpiOsTerminateDebugger(void)
{
	panic("AcpiOsWaitCommandReady");
}

ACPI_STATUS
AcpiOsWaitCommandReady(void)
{
	panic("AcpiOsWaitCommandReady");
}

ACPI_STATUS AcpiOsNotifyCommandComplete(void)
{
	panic("AcpiOsNotifyCommandComplete");
}

ACPI_STATUS
AcpiOsInitializeDebugger(void)
{
	panic("AcpiOsInitializeDebugger");
}

void AcpiDmDisassemble(void)
{
	panic("AcpiDmDisassemble");
}

#define ACPI_MAX_INIT_TABLES 16

static ACPI_TABLE_DESC TableArray[ACPI_MAX_INIT_TABLES];

void acpi_early_init(void)
{
	ACPI_STATUS Status;
#if 1
	AcpiDbgLevel = ACPI_DEBUG_ALL;
	AcpiDbgLayer = ACPI_TRACE_LAYER_ALL;
#endif
	Status = AcpiInitializeTables(TableArray, ACPI_MAX_INIT_TABLES, TRUE);
	if (ACPI_FAILURE(Status)) {
		panic("AcpiInitializeTables");
	}
}

void acpi_late_init(void)
{
	acpi_late = true;
	ACPI_STATUS Status;
	Status = AcpiInitializeSubsystem();
	if (ACPI_FAILURE(Status)) {
		panic("AcpiInitializeSubsystem");
	}
	Status = AcpiReallocateRootTable();
	if (ACPI_FAILURE(Status)) {
		panic("AcpiReallocateRootTable");
	}
	Status = AcpiLoadTables();
	if (ACPI_FAILURE(Status)) {
		panic("AcpiLoadTables");
	}
#if 0 // TODO
	Status = InstallHandlers();
	if (ACPI_FAILURE(Status)) {
		panic("InstallHandlers");
	}
	Status = AcpiEnableSubsystem(ACPI_FULL_INITIALIZATION);
	if (ACPI_FAILURE(Status)) {
		panic("AcpiEnableSubsystem");
	}

	Status = AcpiInitializeObjects(ACPI_FULL_INITIALIZATION);
	if (ACPI_FAILURE(Status)) {
		panic("AcpiInitializeObjects");
	}
#endif
}
