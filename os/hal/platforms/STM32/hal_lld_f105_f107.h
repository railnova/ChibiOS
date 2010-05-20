/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010 Giovanni Di Sirio.

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

/**
 * @file    STM32/hal_lld_f105_f107.h
 * @brief   STM32F10x Connectivity Line HAL subsystem low level driver header.
 *
 * @addtogroup STM32F10X_CL_HAL
 * @{
 */

#ifndef _HAL_LLD_F105_F107_H_
#define _HAL_LLD_F105_F107_H_

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

#define STM32_HSICLK            8000000     /**< High speed internal clock. */
#define STM32_LSICLK            40000       /**< Low speed internal clock.  */

/* RCC_CFGR register bits definitions.*/
#define STM32_SW_HSI            (0 << 0)    /**< SYSCLK source is HSI.      */
#define STM32_SW_HSE            (1 << 0)    /**< SYSCLK source is HSE.      */
#define STM32_SW_PLL            (2 << 0)    /**< SYSCLK source is PLL.      */

#define STM32_HPRE_DIV1         (0 << 4)    /**< SYSCLK divided by 1.       */
#define STM32_HPRE_DIV2         (8 << 4)    /**< SYSCLK divided by 2.       */
#define STM32_HPRE_DIV4         (9 << 4)    /**< SYSCLK divided by 4.       */
#define STM32_HPRE_DIV8         (10 << 4)   /**< SYSCLK divided by 8.       */
#define STM32_HPRE_DIV16        (11 << 4)   /**< SYSCLK divided by 16.      */
#define STM32_HPRE_DIV64        (12 << 4)   /**< SYSCLK divided by 64.      */
#define STM32_HPRE_DIV128       (13 << 4)   /**< SYSCLK divided by 128.     */
#define STM32_HPRE_DIV256       (14 << 4)   /**< SYSCLK divided by 256.     */
#define STM32_HPRE_DIV512       (15 << 4)   /**< SYSCLK divided by 512.     */

#define STM32_PPRE1_DIV1        (0 << 8)    /**< HCLK divided by 1.         */
#define STM32_PPRE1_DIV2        (4 << 8)    /**< HCLK divided by 2.         */
#define STM32_PPRE1_DIV4        (5 << 8)    /**< HCLK divided by 4.         */
#define STM32_PPRE1_DIV8        (6 << 8)    /**< HCLK divided by 8.         */
#define STM32_PPRE1_DIV16       (7 << 8)    /**< HCLK divided by 16.        */

#define STM32_PPRE2_DIV1        (0 << 11)   /**< HCLK divided by 1.         */
#define STM32_PPRE2_DIV2        (4 << 11)   /**< HCLK divided by 2.         */
#define STM32_PPRE2_DIV4        (5 << 11)   /**< HCLK divided by 4.         */
#define STM32_PPRE2_DIV8        (6 << 11)   /**< HCLK divided by 8.         */
#define STM32_PPRE2_DIV16       (7 << 11)   /**< HCLK divided by 16.        */

#define STM32_ADCPRE_DIV2       (0 << 14)   /**< HCLK divided by 2.         */
#define STM32_ADCPRE_DIV4       (1 << 14)   /**< HCLK divided by 4.         */
#define STM32_ADCPRE_DIV6       (2 << 14)   /**< HCLK divided by 6.         */
#define STM32_ADCPRE_DIV8       (3 << 14)   /**< HCLK divided by 8.         */

#define STM32_PLLSRC_HSI        (0 << 16)   /**< PLL clock source is HSI.   */
#define STM32_PLLSRC_PREDIV1    (1 << 16)   /**< PLL clock source is
                                                 PREDIV1.                   */

#define STM32_OTGFSPRE_DIV2     (1 << 22)   /**< HCLK*2 divided by 2.       */
#define STM32_OTGFSPRE_DIV3     (0 << 22)   /**< HCLK*2 divided by 3.       */

#define STM32_MCO_NOCLOCK       (0 << 24)   /**< No clock on MCO pin.       */
#define STM32_MCO_SYSCLK        (4 << 24)   /**< SYSCLK on MCO pin.         */
#define STM32_MCO_HSI           (5 << 24)   /**< HSI clock on MCO pin.      */
#define STM32_MCO_HSE           (6 << 24)   /**< HSE clock on MCO pin.      */
#define STM32_MCO_PLLDIV2       (7 << 24)   /**< PLL/2 clock on MCO pin.    */
#define STM32_MCO_PLL2          (8 << 24)   /**< PLL2 clock on MCO pin.     */
#define STM32_MCO_PLL3DIV2      (9 << 24)   /**< PLL3/2 clock on MCO pin.   */
#define STM32_MCO_XT1           (10 << 24)  /**< XT1 clock on MCO pin.      */
#define STM32_MCO_PLL3          (11 << 24)  /**< PLL3 clock on MCO pin.     */

/* RCC_CFGR2 register bits definitions.*/
#define STM32_PREDIV1SRC_HSE    (0 << 16)   /**< PREDIV1 source is HSE.     */
#define STM32_PREDIV1SRC_PLL2   (1 << 16)   /**< PREDIV1 source is PLL2.    */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @brief   Main clock source selection.
 * @note    If the selected clock source is not the PLL then the PLL is not
 *          initialized and started.
 * @note    The default value is calculated for a 72MHz system clock from
 *          a 25MHz crystal using both PLL and PLL2.
 */
#if !defined(STM32_SW) || defined(__DOXYGEN__)
#define STM32_SW                    STM32_SW_PLL
#endif

/**
 * @brief   Clock source for the PLL.
 * @note    This setting has only effect if the PLL is selected as the
 *          system clock source.
 * @note    The default value is calculated for a 72MHz system clock from
 *          a 25MHz crystal using both PLL and PLL2.
 */
#if !defined(STM32_PLLSRC) || defined(__DOXYGEN__)
#define STM32_PLLSRC                STM32_PLLSRC_PREDIV1
#endif

/**
 * @brief   PREDIV1 clock source.
 * @note    This setting has only effect if the PLL is selected as the
 *          system clock source.
 * @note    The default value is calculated for a 72MHz system clock from
 *          a 25MHz crystal using both PLL and PLL2.
 */
#if !defined(STM32_PREDIV1SRC) || defined(__DOXYGEN__)
#define STM32_PREDIV1SRC            STM32_PREDIV1SRC_PLL2
#endif

/**
 * @brief   PREDIV1 division factor.
 * @note    This setting has only effect if the PLL is selected as the
 *          system clock source.
 * @note    The allowed range is 1...16.
 * @note    The default value is calculated for a 72MHz system clock from
 *          a 25MHz crystal using both PLL and PLL2.
 */
#if !defined(STM32_PREDIV1_VALUE) || defined(__DOXYGEN__)
#define STM32_PREDIV1_VALUE         5
#endif

/**
 * @brief   PLL multiplier value.
 * @note    The allowed range is 4...9.
 * @note    The default value is calculated for a 72MHz system clock from
 *          a 25MHz crystal using both PLL and PLL2.
 */
#if !defined(STM32_PLLMUL_VALUE) || defined(__DOXYGEN__)
#define STM32_PLLMUL_VALUE          9
#endif

/**
 * @brief   PREDIV2 division factor.
 * @note    This setting has only effect if the PLL2 is selected as the
 *          clock source for the PLL.
 * @note    The allowed range is 1...16.
 * @note    The default value is calculated for a 72MHz system clock from
 *          a 25MHz crystal using both PLL and PLL2.
 */
#if !defined(STM32_PREDIV2_VALUE) || defined(__DOXYGEN__)
#define STM32_PREDIV2_VALUE         5
#endif

/**
 * @brief   PLL2 multiplier value.
 * @note    The default value is calculated for a 72MHz system clock from
 *          a 25MHz crystal using both PLL and PLL2.
 */
#if !defined(STM32_PLL2MUL_VALUE) || defined(__DOXYGEN__)
#define STM32_PLL2MUL_VALUE         8
#endif

/**
 * @brief   AHB prescaler value.
 * @note    The default value is calculated for a 72MHz system clock from
 *          a 25MHz crystal using both PLL and PLL2.
 */
#if !defined(STM32_HPRE) || defined(__DOXYGEN__)
#define STM32_HPRE                  STM32_HPRE_DIV1
#endif

/**
 * @brief   APB1 prescaler value.
 */
#if !defined(STM32_PPRE1) || defined(__DOXYGEN__)
#define STM32_PPRE1                 STM32_PPRE1_DIV2
#endif

/**
 * @brief   APB2 prescaler value.
 */
#if !defined(STM32_PPRE2) || defined(__DOXYGEN__)
#define STM32_PPRE2                 STM32_PPRE2_DIV2
#endif

/**
 * @brief   ADC prescaler value.
 */
#if !defined(STM32_ADCPRE) || defined(__DOXYGEN__)
#define STM32_ADCPRE                STM32_ADCPRE_DIV4
#endif

/**
 * @brief   MCO pin setting.
 */
#if !defined(STM32_MCO) || defined(__DOXYGEN__)
#define STM32_MCO                   STM32_MCO_NOCLOCK
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/**
 * @brief   PREDIV1 field.
 */
#if (STM32_PREDIV1_VALUE >= 1) && (STM32_PREDIV1_VALUE <= 16) ||             \
    defined(__DOXYGEN__)
#define STM32_PREDIV1               ((STM32_PREDIV1_VALUE - 1) << 0)
#else
#error "invalid STM32_PREDIV1_VALUE value specified"
#endif

/**
 * @brief   PREDIV2 field.
 */
#if (STM32_PREDIV2_VALUE >= 1) && (STM32_PREDIV2_VALUE <= 16) ||             \
    defined(__DOXYGEN__)
#define STM32_PREDIV2               ((STM32_PREDIV2_VALUE - 1) << 4)
#else
#error "invalid STM32_PREDIV2_VALUE value specified"
#endif

/**
 * @brief   PLLMUL field.
 */
#if ((STM32_PLLMUL_VALUE >= 4) && (STM32_PLLMUL_VALUE <= 9)) ||             \
    defined(__DOXYGEN__)
#define STM32_PLLMUL                ((STM32_PLLMUL_VALUE - 2) << 18)
#else
#error "invalid STM32_PLLMUL_VALUE value specified"
#endif

/**
 * @brief   PLL2MUL field.
 */
#if ((STM32_PLL2MUL_VALUE >= 8) && (STM32_PLL2MUL_VALUE <= 14)) ||          \
    defined(__DOXYGEN__)
#define STM32_PLL2MUL               ((STM32_PLLMUL_VALUE - 2) << 8)
#elif (STM32_PLL2MUL_VALUE == 16)
#define STM32_PLL2MUL               (14 << 8)
#elif (STM32_PLL2MUL_VALUE == 20)
#define STM32_PLL2MUL               (15 << 8)
#else
#error "invalid STM32_PLL2MUL_VALUE value specified"
#endif

/* The following values are only used if PLL2 clock is selected as source
   for the PLL clock */
#if (STM32_PREDIV1SRC == STM32_PREDIV1SRC_PLL2) || defined(__DOXYGEN__)
/**
 * @brief   PLL2 input frequency.
 */
#define STM32_PLL2CLKIN             (STM32_HSECLK / STM32_PREDIV2_VALUE)

/* PLL2 input frequency range check.*/
#if (STM32_PLL2CLKIN < 3000000) || (STM32_PLL2CLKIN > 5000000)
#error "STM32_PLL2CLKIN outside acceptable range (3...5MHz)"
#endif

/**
 * @brief   PLL2 output clock frequency.
 */
#define STM32_PLL2CLKOUT            (STM32_PLL2CLKIN * STM32_PLL2MUL_VALUE)

/* PLL2 output frequency range check.*/
#if (STM32_PLL2CLKOUT < 40000000) || (STM32_PLL2CLKOUT > 74000000)
#error "STM32_PLL2CLKOUT outside acceptable range (40...74MHz)"
#endif
#endif /* STM32_PREDIV1SRC == STM32_PREDIV1SRC_PLL2 */

/**
 * @brief   PREDIV1 input frequency.
 */
#if (STM32_PREDIV1SRC == STM32_PREDIV1SRC_PLL2) || defined(__DOXYGEN__)
#define STM32_PREDIV1CLK            STM32_PLL2CLKOUT
#elif STM32_PREDIV1SRC == STM32_PREDIV1SRC_HSE
#define STM32_PREDIV1CLK            STM32_HSECLK
#else
#error "invalid STM32_PREDIV1SRC value specified"
#endif

/**
 * @brief   PLL input clock frequency.
 */
#if (STM32_PLLSRC == STM32_PLLSRC_PREDIV1) || defined(__DOXYGEN__)
#define STM32_PLLCLKIN             (STM32_PREDIV1CLK / STM32_PREDIV1_VALUE)
#elif STM32_PLLSRC == STM32_PLLSRC_HSI
#define STM32_PLLCLKIN             (STM32_HSICLK / 2)
#else
#error "invalid STM32_PLLSRC value specified"
#endif

/* PLL input frequency range check.*/
#if (STM32_PLLCLKIN < 3000000) || (STM32_PLLCLKIN > 12000000)
#error "STM32_PLLCLKIN outside acceptable range (3...12MHz)"
#endif

/**
 * @brief   PLL output clock frequency.
 */
#define STM32_PLLCLKOUT            (STM32_PLLCLKIN * STM32_PLLMUL_VALUE)

/* PLL output frequency range check.*/
#if (STM32_PLLCLKOUT < 18000000) || (STM32_PLLCLKOUT > 72000000)
#error "STM32_PLLCLKOUT outside acceptable range (18...72MHz)"
#endif

/**
 * @brief   System clock source.
 */
#if (STM32_SW == STM32_SW_PLL) || defined(__DOXYGEN__)
#define STM32_SYSCLK                STM32_PLLCLKOUT
#elif (STM32_SW == STM32_SW_HSI)
#define STM32_SYSCLK                STM32_HSICLK
#elif (STM32_SW == STM32_SW_HSE)
#define STM32_SYSCLK                STM32_HSECLK
#else
#error "invalid STM32_SYSCLK_SW value specified"
#endif

/* Check on the system clock.*/
#if STM32_SYSCLK > 72000000
#error "STM32_SYSCLK above maximum rated frequency (72MHz)"
#endif

/**
 * @brief   AHB frequency.
 */
#if (STM32_HPRE == STM32_HPRE_DIV1) || defined(__DOXYGEN__)
#define STM32_HCLK                  (STM32_SYSCLK / 1)
#elif STM32_HPRE == STM32_HPRE_DIV2
#define STM32_HCLK                  (STM32_SYSCLK / 2)
#elif STM32_HPRE == STM32_HPRE_DIV4
#define STM32_HCLK                  (STM32_SYSCLK / 4)
#elif STM32_HPRE == STM32_HPRE_DIV8
#define STM32_HCLK                  (STM32_SYSCLK / 8)
#elif STM32_HPRE == STM32_HPRE_DIV16
#define STM32_HCLK                  (STM32_SYSCLK / 16)
#elif STM32_HPRE == STM32_HPRE_DIV64
#define STM32_HCLK                  (STM32_SYSCLK / 64)
#elif STM32_HPRE == STM32_HPRE_DIV128
#define STM32_HCLK                  (STM32_SYSCLK / 128)
#elif STM32_HPRE == STM32_HPRE_DIV256
#define STM32_HCLK                  (STM32_SYSCLK / 256)
#elif STM32_HPRE == STM32_HPRE_DIV512
#define STM32_HCLK                  (STM32_SYSCLK / 512)
#else
#error "invalid STM32_HPRE value specified"
#endif

/* AHB frequency check.*/
#if STM32_HPRE > 72000000
#error "STM32_HPRE exceeding maximum frequency (72MHz)"
#endif

/**
 * @brief   APB1 frequency.
 */
#if (STM32_PPRE1 == STM32_PPRE1_DIV1) || defined(__DOXYGEN__)
#define STM32_PCLK1                 (STM32_HCLK / 1)
#elif STM32_PPRE1 == STM32_PPRE1_DIV2
#define STM32_PCLK1                 (STM32_HCLK / 2)
#elif STM32_PPRE1 == STM32_PPRE1_DIV4
#define STM32_PCLK1                 (STM32_HCLK / 4)
#elif STM32_PPRE1 == STM32_PPRE1_DIV8
#define STM32_PCLK1                 (STM32_HCLK / 8)
#elif STM32_PPRE1 == STM32_PPRE1_DIV16
#define STM32_PCLK1                 (STM32_HCLK / 16)
#else
#error "invalid STM32_PPRE1 value specified"
#endif

/* APB1 frequency check.*/
#if STM32_PCLK1 > 36000000
#error "STM32_PCLK1 exceeding maximum frequency (36MHz)"
#endif

/**
 * @brief   APB2 frequency.
 */
#if (STM32_PPRE2 == STM32_PPRE2_DIV1) || defined(__DOXYGEN__)
#define STM32_PCLK2                 (STM32_HCLK / 1)
#elif STM32_PPRE2 == STM32_PPRE2_DIV2
#define STM32_PCLK2                 (STM32_HCLK / 2)
#elif STM32_PPRE2 == STM32_PPRE2_DIV4
#define STM32_PCLK2                 (STM32_HCLK / 4)
#elif STM32_PPRE2 == STM32_PPRE2_DIV8
#define STM32_PCLK2                 (STM32_HCLK / 8)
#elif STM32_PPRE2 == STM32_PPRE2_DIV16
#define STM32_PCLK2                 (STM32_HCLK / 16)
#else
#error "invalid STM32_PPRE2 value specified"
#endif

/* APB2 frequency check.*/
#if STM32_PCLK2 > 72000000
#error "STM32_PCLK2 exceeding maximum frequency (72MHz)"
#endif

/**
 * @brief   ADC frequency.
 */
#if (STM32_ADCPRE == STM32_ADCPRE_DIV2) || defined(__DOXYGEN__)
#define STM32_ADCCLK                (STM32_PCLK2 / 2)
#elif STM32_ADCPRE == STM32_ADCPRE_DIV4
#define STM32_ADCCLK                (STM32_PCLK2 / 4)
#elif STM32_ADCPRE == STM32_ADCPRE_DIV6
#define STM32_ADCCLK                (STM32_PCLK2 / 6)
#elif STM32_ADCPRE == STM32_ADCPRE_DIV8
#define STM32_ADCCLK                (STM32_PCLK2 / 8)
#else
#error "invalid STM32_ADCPRE value specified"
#endif

/* ADC frequency check.*/
#if STM32_ADCCLK > 14000000
#error "STM32_ADCCLK exceeding maximum frequency (14MHz)"
#endif

/**
 * @brief   Flash settings.
 */
#if (STM32_HCLK <= 24000000) || defined(__DOXYGEN__)
#define STM32_FLASHBITS             0x00000010
#elif STM32_HCLK <= 48000000
#define STM32_FLASHBITS             0x00000011
#else
#define STM32_FLASHBITS             0x00000012
#endif

#endif /* _HAL_LLD_F105_F107_H_ */

/** @} */
