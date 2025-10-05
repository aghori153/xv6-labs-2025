#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "types.h"   // for uint

/* forward declaration so header doesn't need cpu.h */
struct cpu;

// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};

/* Spinlock interface (prototypes match kernel/ spinlock implementation) */
void initlock(struct spinlock *lk, char *name);
void acquire(struct spinlock *lk);
void release(struct spinlock *lk);
int holding(struct spinlock *lk);

/* helpers used by push_off/pop_off in proc.c/trap.c */
void push_off(void);
void pop_off(void);

#endif /* SPINLOCK_H */
