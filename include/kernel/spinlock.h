#ifndef __MANTICORE_KERNEL_SPINLOCK_H
#define __MANTICORE_KERNEL_SPINLOCK_H

typedef unsigned long spinlock_t;

#define SPIN_LOCK_UNLOCKED	0
#define SPIN_LOCK_LOCKED	1

#define DEFINE_SPIN_LOCK(x)	spinlock_t x = SPIN_LOCK_UNLOCKED;

static inline void spin_lock(spinlock_t *lock)
{
	while (!__sync_bool_compare_and_swap(lock, SPIN_LOCK_UNLOCKED, SPIN_LOCK_LOCKED))
		;;
}

static inline void spin_unlock(spinlock_t *lock)
{
	__sync_bool_compare_and_swap(lock, SPIN_LOCK_LOCKED, SPIN_LOCK_UNLOCKED);
}

#endif
