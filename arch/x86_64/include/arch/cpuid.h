#ifndef ARCH_CPUID_H
#define ARCH_CPUID_H

#include <kernel/const.h>

#define X86_CPUID_FEATURE			0x00000001
#define X86_CPUID_FEATURE_EBX_APIC_ID_SHIFT	24
#define X86_CPUID_FEATURE_ECX_SSE3		_UL_BIT(0)
#define X86_CPUID_FEATURE_ECX_PCLMULQDQ		_UL_BIT(1)
#define X86_CPUID_FEATURE_ECX_DTES64		_UL_BIT(2)
#define X86_CPUID_FEATURE_ECX_MONITOR		_UL_BIT(3)
#define X86_CPUID_FEATURE_ECX_DS_CPL		_UL_BIT(4)
#define X86_CPUID_FEATURE_ECX_VMX		_UL_BIT(5)
#define X86_CPUID_FEATURE_ECX_SMX		_UL_BIT(6)
#define X86_CPUID_FEATURE_ECX_EIST		_UL_BIT(7)
#define X86_CPUID_FEATURE_ECX_TM2		_UL_BIT(8)
#define X86_CPUID_FEATURE_ECX_SSSE3		_UL_BIT(9)
#define X86_CPUID_FEATURE_ECX_CNXT_ID		_UL_BIT(10)
#define X86_CPUID_FEATURE_ECX_SDBG		_UL_BIT(11)
#define X86_CPUID_FEATURE_ECX_FMA		_UL_BIT(12)
#define X86_CPUID_FEATURE_ECX_CMPXCHG16B	_UL_BIT(13)
#define X86_CPUID_FEATURE_ECX_XTPR		_UL_BIT(14)
#define X86_CPUID_FEATURE_ECX_PDCM		_UL_BIT(15)
#define X86_CPUID_FEATURE_ECX_PCID		_UL_BIT(17)
#define X86_CPUID_FEATURE_ECX_DCA		_UL_BIT(18)
#define X86_CPUID_FEATURE_ECX_SSE4_1		_UL_BIT(19)
#define X86_CPUID_FEATURE_ECX_SSE4_2		_UL_BIT(20)
#define X86_CPUID_FEATURE_ECX_X2APIC		_UL_BIT(21)
#define X86_CPUID_FEATURE_ECX_MOVBE		_UL_BIT(22)
#define X86_CPUID_FEATURE_ECX_POPCNT		_UL_BIT(23)
#define X86_CPUID_FEATURE_ECX_TSC_DEADLINE	_UL_BIT(24)
#define X86_CPUID_FEATURE_ECX_AESNI		_UL_BIT(25)
#define X86_CPUID_FEATURE_ECX_XSAVE		_UL_BIT(26)
#define X86_CPUID_FEATURE_ECX_OSXSAVE		_UL_BIT(27)
#define X86_CPUID_FEATURE_ECX_AVX		_UL_BIT(28)
#define X86_CPUID_FEATURE_ECX_F16C		_UL_BIT(29)
#define X86_CPUID_FEATURE_ECX_RDRAND		_UL_BIT(30)
#define X86_CPUID_FEATURE_EDX_FPU		_UL_BIT(0)
#define X86_CPUID_FEATURE_EDX_VME		_UL_BIT(1)
#define X86_CPUID_FEATURE_EDX_DE		_UL_BIT(2)
#define X86_CPUID_FEATURE_EDX_PSE		_UL_BIT(3)
#define X86_CPUID_FEATURE_EDX_TSC		_UL_BIT(4)
#define X86_CPUID_FEATURE_EDX_MSR		_UL_BIT(5)
#define X86_CPUID_FEATURE_EDX_PAE		_UL_BIT(6)
#define X86_CPUID_FEATURE_EDX_MCE		_UL_BIT(7)
#define X86_CPUID_FEATURE_EDX_CX8		_UL_BIT(8)
#define X86_CPUID_FEATURE_EDX_APIC		_UL_BIT(9)
#define X86_CPUID_FEATURE_EDX_SEP		_UL_BIT(11)
#define X86_CPUID_FEATURE_EDX_MTRR		_UL_BIT(12)
#define X86_CPUID_FEATURE_EDX_PGE		_UL_BIT(13)
#define X86_CPUID_FEATURE_EDX_MCA		_UL_BIT(14)
#define X86_CPUID_FEATURE_EDX_CMOV		_UL_BIT(15)
#define X86_CPUID_FEATURE_EDX_PAT		_UL_BIT(16)
#define X86_CPUID_FEATURE_EDX_PSE36		_UL_BIT(17)
#define X86_CPUID_FEATURE_EDX_PSN		_UL_BIT(18)
#define X86_CPUID_FEATURE_EDX_CLFSH		_UL_BIT(19)
#define X86_CPUID_FEATURE_EDX_DS		_UL_BIT(21)
#define X86_CPUID_FEATURE_EDX_ACPI		_UL_BIT(22)
#define X86_CPUID_FEATURE_EDX_MMX		_UL_BIT(23)
#define X86_CPUID_FEATURE_EDX_FXSR		_UL_BIT(24)
#define X86_CPUID_FEATURE_EDX_SSE		_UL_BIT(25)
#define X86_CPUID_FEATURE_EDX_SSE2		_UL_BIT(26)
#define X86_CPUID_FEATURE_EDX_SS		_UL_BIT(27)
#define X86_CPUID_FEATURE_EDX_HTT		_UL_BIT(28)
#define X86_CPUID_FEATURE_EDX_TM		_UL_BIT(29)
#define X86_CPUID_FEATURE_EDX_PBE		_UL_BIT(31)

#endif
