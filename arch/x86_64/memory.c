#include <kernel/memory.h>

#include <kernel/page-alloc.h>
#include <kernel/printf.h>
#include <kernel/initrd.h>
#include <kernel/align.h>

#include <arch/vmem-defs.h>
#include <arch/vmem.h>

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

enum {
	COMMAND_LINE_TAG	= 1,
	BOOT_LOADER_NAME_TAG	= 2,
	MODULES_TAG		= 3,
	MEMORY_INFO_TAG		= 4,
	BIOS_BOOT_DEVICE_TAG	= 5,
	MEMORY_MAP_TAG		= 6,
	ELF_SYMBOLS_TAG		= 9,
	ACPI_OLD_RSDP		= 14,
	ACPI_NEW_RSDP		= 15,
};

struct header {
	uint32_t total_size;
	uint32_t reserved;
};

struct tag {
	uint32_t tag;
	uint32_t size;
};

struct module {
	uint32_t mod_start;
	uint32_t mod_end;
	const char string[0];
};

struct memory_map {
	uint32_t entry_size;
	uint32_t entry_version;
};

struct memory_map_entry {
	uint64_t base_addr;
	uint64_t length;
	uint32_t type;
	uint32_t reserved;
};

enum {
	MEMORY_TYPE_AVAILABLE = 1,
	MEMORY_TYPE_RESERVED = 2,
	MEMORY_TYPE_ACPI_RECLAIMABLE = 3,
	MEMORY_TYPE_NVS = 4,
	MEMORY_TYPE_BADRAM = 5,
};

static const char *memory_types[] = {
	[MEMORY_TYPE_AVAILABLE]		= "available",
	[MEMORY_TYPE_RESERVED]		= "reserved",
	[MEMORY_TYPE_ACPI_RECLAIMABLE]	= "ACPI reclaimable",
	[MEMORY_TYPE_NVS]		= "NVS",
	[MEMORY_TYPE_BADRAM]		= "bad RAM",
};

struct memory_region mem_regions[MAX_MEM_REGIONS];
size_t nr_mem_regions;

static void parse_boot_loader_name(struct tag *tag, void *data)
{
	const char *boot_loader_name = data + sizeof(*tag);

	printf("Boot loader: '%s'\n", boot_loader_name);
}

static void parse_module(struct tag *tag, void *data)
{
	struct module *mod = data + sizeof(*tag);

	printf("Module: %08x-%08x %s\n", mod->mod_start, mod->mod_end, mod->string);

	initrd_start = paddr_to_ptr(mod->mod_start);
	initrd_end = paddr_to_ptr(mod->mod_end);
}

static void parse_memory_map(struct tag *tag, void *data)
{
	size_t offset = sizeof(*tag);

	struct memory_map *memory_map = data + offset;
	offset += sizeof(*memory_map);

	printf("Memory map:\n");

	for (; offset < tag->size; offset += sizeof(struct memory_map_entry)) {
		struct memory_map_entry *entry = data + offset;
		const char *memory_type = "unknown";
		if (entry->type < ARRAY_SIZE(memory_types)) {
			memory_type = memory_types[entry->type];
		}
		if (entry->length < 1024*1024) {
			printf("  %016lx-%016lx %4ld KiB [%s]\n", entry->base_addr, entry->base_addr + entry->length, entry->length / 1024, memory_type);
		} else {
			printf("  %016lx-%016lx %4ld MiB [%s]\n", entry->base_addr, entry->base_addr + entry->length, entry->length / 1024 / 1024, memory_type);
		}
		if (entry->type != MEMORY_TYPE_AVAILABLE) {
			continue;
		}
		struct memory_region *mem_region = &mem_regions[nr_mem_regions++];
		mem_region->base = entry->base_addr;
		mem_region->len = align_down(entry->length, PAGE_SIZE_SMALL);
		extern char _kernel_end;
		uint64_t kernel_end = (uint64_t) &_kernel_end;
		uint64_t base_addr = entry->base_addr + KERNEL_VMA;
		uint64_t length = entry->length;
		if (base_addr + length < kernel_end) {
			continue;
		}
		if (base_addr + length < (uint64_t) initrd_end) {
			continue;
		}
		if (base_addr < kernel_end) {
			length -= kernel_end - base_addr;
			base_addr = kernel_end;
		}
		if (base_addr < (uint64_t) initrd_end) {
			length -= (uint64_t) initrd_end - base_addr;
			base_addr = (uint64_t) initrd_end;
		}
		base_addr = align_up(base_addr, PAGE_SIZE_SMALL);
		length = align_down(length, PAGE_SIZE_SMALL);
		memory_add_span(base_addr, length);
	}
}

struct acpi_rsdp {
	char signature[8];
	uint8_t checksum;
	char oem_id[6];
	uint8_t revision;
	uint32_t rsdt_phys_addr;
} __attribute__((packed));

struct acpi_rsdp rsdp;
void *rsdp_ptr;

static void parse_acpi_old_rsdp(struct tag *tag, void *data)
{
	void *ptr = data + sizeof(*tag);
	size_t size = tag->size - sizeof(*tag);
	assert(size == sizeof(struct acpi_rsdp));
	memcpy(&rsdp, ptr, size);
	rsdp_ptr = &rsdp;
}

static void parse_acpi_new_rsdp(struct tag *tag, void *data)
{
	panic("parse_acpi_new_rsdp");
}

/*
 * Boot data section provided by the Multiboot 2 compatible boot loader:
 */
extern void *boot_data;

void init_memory_map(void)
{
	void *data = boot_data;
	size_t offset = 0;

	struct header *header = data + offset;
	offset += sizeof(*header);

	while (offset < header->total_size) {
		struct tag *tag = data + offset;
		switch (tag->tag) {
		case BOOT_LOADER_NAME_TAG:
			parse_boot_loader_name(tag, data + offset);
			break;
		case MODULES_TAG:
			parse_module(tag, data + offset);
			break;
		case MEMORY_MAP_TAG:
			parse_memory_map(tag, data + offset);
			break;
		case ACPI_OLD_RSDP:
			parse_acpi_old_rsdp(tag, data + offset);
			break;
		case ACPI_NEW_RSDP:
			parse_acpi_new_rsdp(tag, data + offset);
			break;
		default:
			break;
		}
		offset += tag->size;
		offset = align_up(offset, 8);
	}
}
