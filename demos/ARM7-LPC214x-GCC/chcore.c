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

#include <ch.h>

#include "lpc214x.h"
#include "vic.h"
#include "lpc214x_serial.h"
#include "lpc214x_ssp.h"
#include "mmcsd.h"

#include "buzzer.h"

extern void IrqHandler(void);
extern void T0IrqHandler(void);

#define VAL_TC0_PRESCALER 0

/*
 * Pins configuration for Olimex LPC-P2148.
 *
 * PINSEL0
 *  P0  P0  P0  P0  P0  P0  RXD TXD SSE MOS MIS SCK SDA SCL RXD TXD
 *  15  14  13  12  11  10  1   1   L0  I0  O0  0   0   0   0   0
 *  00  00  00  00  00  00  01  01  01  01  01  01  01  01  01  01
 *  IN  IN  OUT OUT OUT OUT --  --  --  --  --  --  --  --  --  --
 *  0   0   1   1   1   1   0   0   0   0   0   0   0   0   0   0
 *
 * PINSEL1
 *  P0  AD  P0  P0  --  --  AO  --  VB  P0  P0  P0  MOS MIS SCK P0
 *  31  03  29  28  --  --  UT  --  US  22  21  20  I1  O1  1   16
 *  00  01  00  00  00  00  10  00  01  00  00  00  10  10  10  00
 *  OUT --  OUT OUT --  --  --  --  --  OUT OUT OUT --  --  --  IN
 *  1   0   1   1   0   0   0   0   0   1   1   1   0   0   0   0
 *
 * PINSEL2
 *  --  --  --  --  --  --  --  --  --  --  --  --  --  --  GP  DBG --
 *  --  --  --  --  --  --  --  --  --  --  --  --  --  --  IO      --
 *  00  00  00  00  00  00  00  00  00  00  00  00  00  00  0   1   00
 *  --  --  --  --  --  --  --  --  --  --  --  --  --  --  IN  --  --
 */
#define VAL_PINSEL0 0x00055555
#define VAL_PINSEL1 0x100840A8
#define VAL_PINSEL2 0x00000004
#define VAL_FIO0DIR 0xB0703C00
#define VAL_FIO1DIR 0x00000000

/*
 * Hardware initialization goes here.
 * NOTE: Interrupts are still disabled.
 */
void hwinit(void) {

  /*
   * All peripherals clock disabled by default in order to save power.
   */
  PCONP = PCRTC | PCTIM0;

  /*
   * MAM setup.
   */
  MAMTIM = 0x3;                 /* 3 cycles for flash accesses. */
  MAMCR  = 0x2;                 /* MAM fully enabled. */

  /*
   * PLL setup for Fosc=12MHz and CCLK=48MHz.
   * P=2 M=3.
   */
  PLL *pll = PLLBase;
  pll->PLL0_CFG  = 0x23;        /* P and M values. */
  pll->PLL0_CON  = 0x1;         /* Enalbles the PLL 0. */
  pll->PLL0_FEED = 0xAA;
  pll->PLL0_FEED = 0x55;
  while (!(pll->PLL0_STAT & 0x400))
    ;                           /* Wait for PLL lock. */

  pll->PLL0_CON  = 0x3;         /* Connects the PLL. */
  pll->PLL0_FEED = 0xAA;
  pll->PLL0_FEED = 0x55;

  /*
   * VPB setup.
   * PCLK = CCLK / 4.
   */
  VPBDIV = VPD_D4;

  /*
   * I/O pins configuration.
   */
  PINSEL0 = VAL_PINSEL0;
  PINSEL1 = VAL_PINSEL1;
  PINSEL2 = VAL_PINSEL2;
  IO0DIR = VAL_FIO0DIR;
  IO0SET = 0xFFFFFFFF;
  IO1DIR = VAL_FIO1DIR;
  IO1SET = 0xFFFFFFFF;

  /*
   * Interrupt vectors assignment.
   */
  InitVIC();
  VICDefVectAddr = (IOREG32)IrqHandler;

  /*
   * System Timer initialization, 1ms intervals.
   */
  SetVICVector(T0IrqHandler, 0, SOURCE_Timer0);
  VICIntEnable = INTMASK(SOURCE_Timer0);
  TC *timer = T0Base;
  timer->TC_PR = VAL_TC0_PRESCALER;
  timer->TC_MR0 = (PCLK / CH_FREQUENCY) / (VAL_TC0_PRESCALER + 1);
  timer->TC_MCR = 3;    /* Interrupt and clear TC on match MR0. */
  timer->TC_TCR = 2;    /* Reset counter and prescaler. */
  timer->TC_TCR = 1;    /* Timer enabled. */

  /*
   * Other subsystems.
   */
  InitSerial(1, 2);
  InitSSP();
  InitMMC();
  InitBuzzer();
}

/*
 * System idle thread loop.
 */
void _IdleThread(void *p) {

  while (TRUE) {
// Note, it is disabled because it causes trouble with the JTAG probe.
// Enable it in the final code only.
//    PCON = 1;
  }
}

/*
 * System console message (not implemented).
 */
void chSysPuts(char *msg) {
}

/*
 * Non-vectored IRQs handling here.
 */
__attribute__((naked, weak))
void IrqHandler(void) {

  chSysIRQEnterI();

  /* nothing */

  chSysIRQExitI();
}

/*
 * Timer 0 IRQ handling here.
 */
__attribute__((naked, weak))
void T0IrqHandler(void) {

  chSysIRQEnterI();

  T0IR = 1;             /* Clear interrupt on match MR0. */
  chSysTimerHandlerI();

  chSysIRQExitI();
}

/*
 * Common IRQ exit code, \p chSysIRQExitI() just jumps here.
 *
 * System stack frame structure after a context switch in the
 * interrupt handler:
 *
 * High +------------+
 *      |   LR_USR   | -+
 *      |     R12    |  |
 *      |     R3     |  |
 *      |     R2     |  | External context: IRQ handler frame
 *      |     R1     |  |
 *      |     R0     |  |
 *      |   LR_IRQ   |  |   (user code return address)
 *      |    SPSR    | -+   (user code status)
 *      |    ....    | <- chSchDoRescheduleI() stack frame, optimize it for space
 *      |     LR     | -+   (system code return address)
 *      |     R11    |  |
 *      |     R10    |  |
 *      |     R9     |  |
 *      |     R8     |  | Internal context: chSysSwitchI() frame
 *      |    (R7)    |  |   (optional, see CH_CURRP_REGISTER_CACHE)
 *      |     R6     |  |
 *      |     R5     |  |
 * SP-> |     R4     | -+
 * Low  +------------+
 */
__attribute__((naked, weak))
void IrqCommon(void) {
  register BOOL b asm("r0");

  VICVectAddr = 0;
  b = chSchRescRequiredI();
#ifdef THUMB
  asm(".p2align 2,,                                             \n\t" \
      "mov      lr, pc                                          \n\t" \
      "bx       lr                                              \n\t" \
      ".code 32                                                 \n\t");
#endif
  /*
   * If a reschedulation is not required then just returns from the IRQ.
   */
  asm("cmp      r0, #0                                          \n\t" \
      "ldmeqfd  sp!, {r0-r3, r12, lr}                           \n\t" \
      "subeqs   pc, lr, #4                                      \n\t");
  /*
   * Reschedulation required, saves the external context on the
   * system/user stack and empties the IRQ stack.
   */
  asm(".set     MODE_IRQ, 0x12                                  \n\t" \
      ".set     MODE_SYS, 0x1F                                  \n\t" \
      ".set     I_BIT, 0x80                                     \n\t" \
      "ldmfd    sp!, {r0-r3, r12, lr}                           \n\t" \
      "msr      CPSR_c, #MODE_SYS | I_BIT                       \n\t" \
      "stmfd    sp!, {r0-r3, r12, lr}                           \n\t" \
      "msr      CPSR_c, #MODE_IRQ | I_BIT                       \n\t" \
      "mrs      r0, SPSR                                        \n\t" \
      "mov      r1, lr                                          \n\t" \
      "msr      CPSR_c, #MODE_SYS | I_BIT                       \n\t" \
      "stmfd    sp!, {r0, r1}                                   \n\t");

#ifdef THUMB_NO_INTERWORKING
  asm("add      r0, pc, #1                                      \n\t" \
      "bx       r0                                              \n\t" \
      ".code 16                                                 \n\t" \
      "bl       chSchDoRescheduleI                              \n\t" \
      ".p2align 2,,                                             \n\t" \
      "mov      lr, pc                                          \n\t" \
      "bx       lr                                              \n\t" \
      ".code 32                                                 \n\t");
#else
  asm("bl       chSchDoRescheduleI                              \n\t");
#endif

  /*
   * Restores the external context.
   */
  asm("ldmfd    sp!, {r0, r1}                                   \n\t" \
      "msr      CPSR_c, #MODE_IRQ | I_BIT                       \n\t" \
      "msr      SPSR_fsxc, r0                                   \n\t" \
      "mov      lr, r1                                          \n\t" \
      "msr      CPSR_c, #MODE_SYS | I_BIT                       \n\t" \
      "ldmfd    sp!, {r0-r3, r12, lr}                           \n\t" \
      "msr      CPSR_c, #MODE_IRQ | I_BIT                       \n\t" \
      "subs     pc, lr, #4                                      \n\t");

  /*
   * Threads entry/exit code. It is declared weak so you can easily replace it.
   * NOTE: It is always invoked in ARM mode, it does the mode switching.
   * NOTE: It is included into IrqCommon to make sure the symbol refers to
   *       32 bit code.
   */
  asm(".set    F_BIT, 0x40                                      \n\t" \
      ".weak threadstart                                        \n\t" \
      ".globl threadstart                                       \n\t" \
      "threadstart:                                             \n\t" \
      "msr      CPSR_c, #MODE_SYS                               \n\t");
#ifndef THUMB_NO_INTERWORKING
  asm("mov      r0, r5                                          \n\t" \
      "mov      lr, pc                                          \n\t" \
      "bx       r4                                              \n\t" \
      "bl       chThdExit                                       \n\t");
#else
  asm("add      r0, pc, #1                                      \n\t" \
      "bx       r0                                              \n\t" \
      ".code 16                                                 \n\t" \
      "mov      r0, r5                                          \n\t" \
      "bl       jmpr4                                           \n\t" \
      "bl       chThdExit                                       \n\t" \
      "jmpr4:                                                   \n\t" \
      "bx       r4                                              \n\t");
#endif
}

/*
 * System halt.
 */
__attribute__((naked, weak))
void chSysHalt(void) {

  asm(".set     F_BIT, 0x40                                     \n\t" \
      ".set     I_BIT, 0x80                                     \n\t");
#ifdef THUMB
  asm(".p2align 2,,                                             \n\t" \
      "mov      r0, pc                                          \n\t" \
      "bx       r0                                              \n\t");
#endif
  asm(".code 32                                                 \n\t" \
      ".weak _halt32                                            \n\t" \
      ".globl _halt32                                           \n\t" \
      "_halt32:                                                 \n\t" \
      "mrs      r0, CPSR                                        \n\t" \
      "orr      r0, #I_BIT | F_BIT                              \n\t" \
      "msr      CPSR_c, r0                                      \n\t" \
      ".loop:                                                   \n\t" \
      "b        .loop                                           \n\t");
}
