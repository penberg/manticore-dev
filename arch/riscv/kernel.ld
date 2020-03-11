ENTRY(start)
SECTIONS
{
    . = 0x40000000;
    .boot . : { arch/riscv/boot.o(.text) }
    .text : { *(.text) }
    .rodata : { *(.rodata*) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    /DISCARD/ : {
         *(.comment)
         *(.debug*)
    }
    _kernel_end = .;
}
