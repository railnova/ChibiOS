/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    i2s_lld.h
 * @brief   STM32 I2S subsystem low level driver header.
 *
 * @addtogroup I2S
 * @{
 */

#ifndef _I2S_LLD_H_
#define _I2S_LLD_H_

#if HAL_USE_I2S || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @name    Static I2S modes
 * @{
 */
#define STM32_I2S_MODE_SLAVE                0
#define STM32_I2S_MODE_MASTER               1
#define STM32_I2S_MODE_RX                   2
#define STM32_I2S_MODE_TX                   4
#define STM32_I2S_MODE_RXTX                 (STM32_I2S_MODE_RX |            \
                                             STM32_I2S_MODE_TX)
/** @} */

/**
 * @name    Mode checks
 * @{
 */
#define STM32_I2S_IS_MASTER(mode)           ((mode) & STM32_I2S_MODE_MASTER)
#define STM32_I2S_RX_ENABLED(mode)          ((mode) & STM32_I2S_MODE_RX)
#define STM32_I2S_TX_ENABLED(mode)          ((mode) & STM32_I2S_MODE_TX)
/** @} */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */
/**
 * @brief   I2S1 driver enable switch.
 * @details If set to @p TRUE the support for I2S1 is included.
 * @note    The default is @p TRUE.
 */
#if !defined(STM32_I2S_USE_SPI1) || defined(__DOXYGEN__)
#define STM32_I2S_USE_SPI1                  FALSE
#endif

/**
 * @brief   I2S2 driver enable switch.
 * @details If set to @p TRUE the support for I2S2 is included.
 * @note    The default is @p TRUE.
 */
#if !defined(STM32_I2S_USE_SPI2) || defined(__DOXYGEN__)
#define STM32_I2S_USE_SPI2                  FALSE
#endif

/**
 * @brief   I2S1 mode.
 */
#if !defined(STM32_I2S_SPI1_MODE) || defined(__DOXYGEN__)
#define STM32_I2S_SPI1_MODE                 (STM32_I2S_MODE_MASTER |        \
                                             STM32_I2S_MODE_RX)
#endif

/**
 * @brief   I2S2 mode.
 */
#if !defined(STM32_I2S_SPI2_MODE) || defined(__DOXYGEN__)
#define STM32_I2S_SPI2_MODE                 (STM32_I2S_MODE_MASTER |        \
                                             STM32_I2S_MODE_RX)
#endif

/**
 * @brief   I2S1 interrupt priority level setting.
 */
#if !defined(STM32_I2S_SPI1_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define STM32_I2S_SPI1_IRQ_PRIORITY         2
#endif

/**
 * @brief   I2S2 interrupt priority level setting.
 */
#if !defined(STM32_I2S_SPI2_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define STM32_I2S_SPI2_IRQ_PRIORITY         2
#endif

/**
 * @brief   I2S1 DMA priority (0..3|lowest..highest).
 */
#if !defined(STM32_I2S_SPI1_DMA_PRIORITY) || defined(__DOXYGEN__)
#define STM32_I2S_SPI1_DMA_PRIORITY         1
#endif

/**
 * @brief   I2S2 DMA priority (0..3|lowest..highest).
 */
#if !defined(STM32_I2S_SPI2_DMA_PRIORITY) || defined(__DOXYGEN__)
#define STM32_I2S_SPI2_DMA_PRIORITY         1
#endif

/**
 * @brief   I2S DMA error hook.
 */
#if !defined(STM32_I2S_DMA_ERROR_HOOK) || defined(__DOXYGEN__)
#define STM32_I2S_DMA_ERROR_HOOK(i2sp)      osalSysHalt("DMA failure")
#endif
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if STM32_I2S_RX_ENABLED(STM32_I2S_SPI1_MODE) &&                            \
    STM32_I2S_TX_ENABLED(STM32_I2S_SPI1_MODE)
#error "I2S1 RX and TX mode not supported in this driver implementation"
#endif

#if STM32_I2S_RX_ENABLED(STM32_I2S_SPI2_MODE) &&                            \
    STM32_I2S_TX_ENABLED(STM32_I2S_SPI2_MODE)
#error "I2S2 RX and TX mode not supported in this driver implementation"
#endif

#if STM32_I2S_USE_SPI1 && !STM32_HAS_SPI1
#error "SPI1 not present in the selected device"
#endif

#if STM32_I2S_USE_SPI2 && !STM32_HAS_SPI2
#error "SPI2 not present in the selected device"
#endif

#if !STM32_I2S_USE_SPI1 && !STM32_I2S_USE_SPI2
#error "I2S driver activated but no SPI peripheral assigned"
#endif

#if STM32_I2S_USE_SPI1 &&                                                   \
    !OSAL_IRQ_IS_VALID_PRIORITY(STM32_I2S_SPI1_IRQ_PRIORITY)
#error "Invalid IRQ priority assigned to SPI1"
#endif

#if STM32_I2S_USE_SPI2 &&                                                   \
    !OSAL_IRQ_IS_VALID_PRIORITY(STM32_I2S_SPI2_IRQ_PRIORITY)
#error "Invalid IRQ priority assigned to SPI2"
#endif

#if STM32_I2S_USE_SPI1 &&                                                   \
    !STM32_DMA_IS_VALID_PRIORITY(STM32_I2S_SPI1_DMA_PRIORITY)
#error "Invalid DMA priority assigned to SPI1"
#endif

#if STM32_I2S_USE_SPI2 &&                                                   \
    !STM32_DMA_IS_VALID_PRIORITY(STM32_I2S_SPI2_DMA_PRIORITY)
#error "Invalid DMA priority assigned to SPI2"
#endif

/* The following checks are only required when there is a DMA able to
   reassign streams to different channels.*/
#if STM32_ADVANCED_DMA
/* Check on the presence of the DMA streams settings in mcuconf.h.*/
#if STM32_I2S_USE_SPI1 && (!defined(STM32_I2S_SPI1_RX_DMA_STREAM) ||        \
                           !defined(STM32_I2S_SPI1_TX_DMA_STREAM))
#error "SPI1 DMA streams not defined"
#endif

#if STM32_I2S_USE_SPI2 && (!defined(STM32_I2S_SPI2_RX_DMA_STREAM) ||        \
                           !defined(STM32_I2S_SPI2_TX_DMA_STREAM))
#error "SPI2 DMA streams not defined"
#endif

/* Check on the validity of the assigned DMA channels.*/
#if STM32_I2S_USE_SPI1 &&                                                   \
    !STM32_DMA_IS_VALID_ID(STM32_I2S_SPI1_RX_DMA_STREAM, STM32_SPI1_RX_DMA_MSK)
#error "invalid DMA stream associated to SPI1 RX"
#endif

#if STM32_I2S_USE_SPI1 &&                                                   \
    !STM32_DMA_IS_VALID_ID(STM32_I2S_SPI1_TX_DMA_STREAM, STM32_SPI1_TX_DMA_MSK)
#error "invalid DMA stream associated to SPI1 TX"
#endif

#if STM32_I2S_USE_SPI2 &&                                                   \
    !STM32_DMA_IS_VALID_ID(STM32_I2S_SPI2_RX_DMA_STREAM, STM32_SPI2_RX_DMA_MSK)
#error "invalid DMA stream associated to SPI2 RX"
#endif

#if STM32_I2S_USE_SPI2 &&                                                   \
    !STM32_DMA_IS_VALID_ID(STM32_I2S_SPI2_TX_DMA_STREAM, STM32_SPI2_TX_DMA_MSK)
#error "invalid DMA stream associated to SPI2 TX"
#endif
#endif /* STM32_ADVANCED_DMA */

#if !defined(STM32_DMA_REQUIRED)
#define STM32_DMA_REQUIRED
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Type of a structure representing an I2S driver.
 */
typedef struct I2SDriver I2SDriver;

/**
 * @brief   I2S notification callback type.
 *
 * @param[in] i2sp      pointer to the @p I2SDriver object
 * @param[in] offset    offset in buffers of the data to read/write
 * @param[in] n         number of samples to read/write
 */
typedef void (*i2scallback_t)(I2SDriver *i2sp, size_t offset, size_t n);

/**
 * @brief   Driver configuration structure.
 * @note    It could be empty on some architectures.
 */
typedef struct {
  /**
   * @brief   Transmission buffer pointer.
   * @note    Can be @p NULL if TX is not required.
   */
  const void                *tx_buffer;
  /**
   * @brief   Receive buffer pointer.
   * @note    Can be @p NULL if RX is not required.
   */
  void                      *rx_buffer;
  /**
   * @brief   TX and RX buffers size as number of samples.
   */
  size_t                    size;
  /**
   * @brief   Callback function called during streaming.
   */
  i2scallback_t             end_cb;
  /* End of the mandatory fields.*/
  /**
   * @brief   Configuration of the I2SCFGR register.
   * @details See the STM32 reference manual, this register is used for
   *          the I2S configuration, the following bits must not be
   *          specified because handled directly by the driver:
   *          - I2SMOD
   *          - I2SE
   *          - I2SCFG
   *          .
   */
  int16_t                   i2scfgr;
  /**
   * @brief   Configuration of the I2SPR register.
   * @details See the STM32 reference manual, this register is used for
   *          the I2S clock setup.
   */
  int16_t                   i2spr;
} I2SConfig;

/**
 * @brief   Structure representing an I2S driver.
 */
struct I2SDriver {
  /**
   * @brief   Driver state.
   */
  i2sstate_t                state;
  /**
   * @brief   Current configuration data.
   */
  const I2SConfig           *config;
  /* End of the mandatory fields.*/
  /**
   * @brief   Pointer to the SPIx registers block.
   */
  SPI_TypeDef               *spi;
  /**
   * @brief   Calculated part of the I2SCFGR register.
   */
  uint16_t                  cfg;
  /**
   * @brief   Receive DMA stream or @p NULL.
   */
  const stm32_dma_stream_t  *dmarx;
  /**
   * @brief   Transmit DMA stream or @p NULL.
   */
  const stm32_dma_stream_t  *dmatx;
  /**
   * @brief   RX DMA mode bit mask.
   */
  uint32_t                  rxdmamode;
  /**
   * @brief   TX DMA mode bit mask.
   */
  uint32_t                  txdmamode;
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if STM32_I2S_USE_SPI1 && !defined(__DOXYGEN__)
extern I2SDriver I2SD1;
#endif

#if STM32_I2S_USE_SPI2 && !defined(__DOXYGEN__)
extern I2SDriver I2SD2;
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void i2s_lld_init(void);
  void i2s_lld_start(I2SDriver *i2sp);
  void i2s_lld_stop(I2SDriver *i2sp);
  void i2s_lld_start_exchange(I2SDriver *i2sp);
  void i2s_lld_stop_exchange(I2SDriver *i2sp);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_I2S */

#endif /* _I2S_LLD_H_ */

/** @} */
