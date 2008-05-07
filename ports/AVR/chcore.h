/*
    ChibiOS/RT - Copyright (C) 2006-2007 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CHCORE_H_
#define _CHCORE_H_

#define CH_ARCHITECTURE_AVR

/*
 * Interrupt saved context.
 */
struct extctx {
  uint8_t       _next;
  uint8_t       r31;
  uint8_t       r30;
  uint8_t       r27;
  uint8_t       r26;
  uint8_t       r25;
  uint8_t       r24;
  uint8_t       r23;
  uint8_t       r22;
  uint8_t       r21;
  uint8_t       r20;
  uint8_t       r19;
  uint8_t       r18;
  uint8_t       sr;
  uint8_t       r1;
  uint8_t       r0;
  uint16_t      pc;
};

/*
 * System saved context.
 */
struct intctx {
  uint8_t       _next;
  uint8_t       r29;
  uint8_t       r28;
  uint8_t       r17;
  uint8_t       r16;
  uint8_t       r15;
  uint8_t       r14;
  uint8_t       r13;
  uint8_t       r12;
  uint8_t       r11;
  uint8_t       r10;
#ifndef CH_CURRP_REGISTER_CACHE
  uint8_t       r9;
  uint8_t       r8;
#endif
  uint8_t       r7;
  uint8_t       r6;
  uint8_t       r5;
  uint8_t       r4;
  uint8_t       r3;
  uint8_t       r2;
  uint8_t       pcl;
  uint8_t       pch;
};

/*
 * Port dependent part of the Thread structure, you may add fields in
 * this structure.
 */
typedef struct {
  struct intctx *sp;
} Context;

/**
 * Platform dependent part of the \p chThdCreate() API.
 */
#define SETUP_CONTEXT(workspace, wsize, pf, arg) {                      \
  tp->p_ctx.sp = (struct intctx*)((uint8_t *)workspace + wsize - 1 -    \
                                  (sizeof(struct intctx) - 1));         \
  tp->p_ctx.sp->r2  = (int)pf;                                          \
  tp->p_ctx.sp->r3  = (int)pf >> 8;                                     \
  tp->p_ctx.sp->r4  = (int)arg;                                         \
  tp->p_ctx.sp->r5  = (int)arg >> 8;                                    \
  tp->p_ctx.sp->pcl = (int)threadstart >> 8;                            \
  tp->p_ctx.sp->pch = (int)threadstart;                                 \
}

#define INT_REQUIRED_STACK 8
#define StackAlign(n) (n)
#define UserStackSize(n) StackAlign(sizeof(Thread) +                    \
                                    (sizeof(struct intctx) - 1) +       \
                                    (sizeof(struct extctx) - 1) +       \
                                    (n) + (INT_REQUIRED_STACK))
#define WorkingArea(s, n) uint8_t s[UserStackSize(n)];

#define chSysLock() asm volatile ("cli")

#define chSysUnlock() asm volatile ("sei")

#define chSysIRQEnterI()                                                \
  asm ("" : : : "r18", "r19", "r20", "r21", "r22", "r23", "r24",        \
                "r25", "r26", "r27", "r30", "r31");


#define chSysIRQExitI() {                                               \
  if (chSchRescRequiredI())                                             \
    chSchDoRescheduleI();                                               \
}

#define IDLE_THREAD_STACK_SIZE 8
void _IdleThread(void *p) __attribute__((noreturn));

void chSysHalt(void) __attribute__((noreturn)) ;
void chSysSwitchI(Thread *otp, Thread *ntp);
void chSysPuts(char *msg);
void threadstart(void) __attribute__((naked));

#endif /* _CHCORE_H_ */
