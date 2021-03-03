#ifndef __MANTICORE_X86_SMP_H
#define __MANTICORE_X86_SMP_H

#include <stdint.h>

void bringup_cpu(uint32_t cpu_id);

void register_cpu(uint32_t cpu_id);

#endif
