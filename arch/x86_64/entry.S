#include <arch/segment.h>

.macro pushq_cfi reg
        pushq \reg
        .cfi_adjust_cfa_offset 8
        .cfi_rel_offset \reg, 0
.endm

.macro popq_cfi reg
        popq \reg
        .cfi_adjust_cfa_offset -8
        .cfi_restore \reg
.endm

.macro EXCEPTION_ENTRY name, has_error_code
.globl \name
.type \name, @function
\name:
        .cfi_startproc simple
        .cfi_signal_frame
	.if \has_error_code == 0
	pushq	$0
	.endif
        .cfi_def_cfa %rsp, 0
//        .cfi_offset %rip, 8
//        .cfi_offset %rsp, 32
	pushq_cfi	%r15
	pushq_cfi	%r14
	pushq_cfi	%r13
	pushq_cfi	%r12
	pushq_cfi	%r11
	pushq_cfi	%r10
	pushq_cfi	%r9
	pushq_cfi	%r8
	pushq_cfi	%rdi
	pushq_cfi	%rsi
	pushq_cfi	%rbp
	pushq_cfi	%rdx
	pushq_cfi	%rcx
	pushq_cfi	%rbx
	pushq_cfi	%rax

	mov	%rsp, %rdi
	subq	$8, %rsp	# Keep %rsp 16-byte aligned
        .cfi_adjust_cfa_offset 8

	call	do_\name

	addq	$8, %rsp
        .cfi_adjust_cfa_offset -8
	popq_cfi	%rax
	popq_cfi	%rbx
	popq_cfi	%rcx
	popq_cfi	%rdx
	popq_cfi	%rbp
	popq_cfi	%rsi
	popq_cfi	%rdi
	popq_cfi	%r8
	popq_cfi	%r9
	popq_cfi	%r10
	popq_cfi	%r11
	popq_cfi	%r12
	popq_cfi	%r13
	popq_cfi	%r14
	popq_cfi	%r15
	add	$8, %rsp

	iretq
        .cfi_endproc
.endm

.macro EXCEPTION_NOERROR_ENTRY name
	EXCEPTION_ENTRY \name, 0
.endm

.macro EXCEPTION_ERROR_ENTRY name
	EXCEPTION_ENTRY \name, 1
.endm

.cfi_sections .eh_frame,  .debug_frame

.text

EXCEPTION_NOERROR_ENTRY	x86_divide_error_exception
EXCEPTION_NOERROR_ENTRY	x86_debug_exception
EXCEPTION_NOERROR_ENTRY	x86_nmi_interrupt
EXCEPTION_NOERROR_ENTRY	x86_breakpoint_exception
EXCEPTION_NOERROR_ENTRY	x86_overflow_exception
EXCEPTION_NOERROR_ENTRY	x86_bound_range_exceeded_exception
EXCEPTION_NOERROR_ENTRY	x86_invalid_opcode_exception
EXCEPTION_NOERROR_ENTRY	x86_device_not_available_exception
EXCEPTION_ERROR_ENTRY	x86_double_fault_exception
EXCEPTION_ERROR_ENTRY	x86_invalid_tss_exception
EXCEPTION_ERROR_ENTRY	x86_segment_not_present
EXCEPTION_ERROR_ENTRY	x86_stack_fault_exception
EXCEPTION_ERROR_ENTRY	x86_general_protection_exception
EXCEPTION_ERROR_ENTRY	x86_page_fault_exception
EXCEPTION_NOERROR_ENTRY	x86_x87_fpu_floating_point_error
EXCEPTION_ERROR_ENTRY	x86_alignment_check_exception
EXCEPTION_NOERROR_ENTRY	x86_machine_check_exception
EXCEPTION_NOERROR_ENTRY	x86_simd_floating_point_exception
EXCEPTION_NOERROR_ENTRY	x86_virtualization_exception

.align 16
.globl syscall_entry
.type syscall_entry, @function
syscall_entry:
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%r11
	pushq	%r10
	pushq	%r9
	pushq	%r8
	pushq	%rdi
	pushq	%rsi
	pushq	%rbp
	pushq	%rdx
	pushq	%rcx
	pushq	%rbx

	movq	%rax, %rdi
	callq	syscall

	popq	%rbx
	popq	%rcx
	popq	%rdx
	popq	%rbp
	popq	%rsi
	popq	%rdi
	popq	%r8
	popq	%r9
	popq	%r10
	popq	%r11
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15

	sysretq

.align 16
.globl switch_to_userspace
.type switch_to_userspace, @function
switch_to_userspace:
	movq $X86_USER_DS, %rax
	mov %ax, %ds
	mov %ax, %es

	push %rax # %ss
	push %rsi # %rsp
	pushf
	pushq $X86_USER_CS # %cs
	push %rdi # rip

	iretq
