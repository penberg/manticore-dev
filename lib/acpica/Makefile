includes += -isystem lib/acpica/source/include

acpi-source += $(shell find lib/acpica/source/components/debugger -type f -name '*.c')
acpi-source += $(shell find lib/acpica/source/components/dispatcher -type f -name '*.c')
acpi-source += $(shell find lib/acpica/source/components/events -type f -name '*.c')
acpi-source += $(shell find lib/acpica/source/components/executer -type f -name '*.c')
acpi-source += $(shell find lib/acpica/source/components/hardware -type f -name '*.c')
acpi-source += $(shell find lib/acpica/source/components/namespace -type f -name '*.c')
acpi-source += $(shell find lib/acpica/source/components/parser -type f -name '*.c')
acpi-source += $(shell find lib/acpica/source/components/resources -type f -name '*.c')
acpi-source += $(shell find lib/acpica/source/components/tables -type f -name '*.c')
acpi-source += $(shell find lib/acpica/source/components/utilities -type f -name '*.c' | grep -v utprint)
acpi = $(patsubst %.c, %.o, $(acpi-source))
objs += $(acpi)

CFLAGS += -DACPI_MACHINE_WIDTH=64 -DACPI_USE_LOCAL_CACHE -DACPI_DEBUGGER -DACPI_DEBUG_OUTPUT=1
