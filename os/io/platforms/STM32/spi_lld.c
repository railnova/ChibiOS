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

/**
 * @file templates/spi_lld.c
 * @brief SPI Driver subsystem low level driver source template
 * @addtogroup SPI_LLD
 * @{
 */

#include <ch.h>
#include <spi.h>
#include <stm32_dma.h>
#include <nvic.h>

#if USE_STM32_SPI1 || defined(__DOXYGEN__)
/** @brief SPI1 driver identifier.*/
SPIDriver SPID1;
#endif

#if USE_STM32_SPI2 || defined(__DOXYGEN__)
/** @brief SPI2 driver identifier.*/
SPIDriver SPID2;
#endif

static uint16_t dummyrx;
static uint16_t dummytx;

/*===========================================================================*/
/* Low Level Driver local functions.                                         */
/*===========================================================================*/

static void spi_stop(SPIDriver *spip, msg_t msg) {

  /* Stops RX and TX DMA channels.*/
  spip->spd_dmarx->CCR = 0;
  spip->spd_dmatx->CCR = 0;

  /* Stops SPI operations.*/
  spip->spd_spi->CR1 &= ~SPI_CR1_SPE;

  chSysLockFromIsr();
  chSchReadyI(spip->spd_thread)->p_msg = msg;
  chSysUnlockFromIsr();
}

static void dma_start(SPIDriver *spip, size_t n, void *rxbuf, void *txbuf) {
  uint32_t ccr;

  /* Common DMA setup.*/
  ccr = spip->spd_dmaprio;
  if ((spip->spd_config->spc_cr1 & SPI_CR1_DFF) != 0)
    ccr |= DMA_CCR1_MSIZE_0 | DMA_CCR1_PSIZE_0; /* 16 bits transfer.*/

  /* RX DMA setup.*/
  spip->spd_dmarx->CMAR = (uint32_t)rxbuf;
  spip->spd_dmarx->CNDTR = (uint32_t)n;
  spip->spd_dmarx->CCR |= ccr;

  /* TX DMA setup.*/
  spip->spd_dmatx->CMAR = (uint32_t)txbuf;
  spip->spd_dmatx->CNDTR = (uint32_t)n;
  spip->spd_dmatx->CCR |= ccr;

  /* DMAs start.*/
  spip->spd_dmarx->CCR |= DMA_CCR1_EN;
  spip->spd_dmatx->CCR |= DMA_CCR1_EN;
}

static msg_t spi_start_wait(SPIDriver *spip) {
  msg_t msg;

  chSysLock();
  spip->spd_spi->CR1 |= SPI_CR1_SPE;    /* SPI enable.*/
  spip->spd_thread = currp;
  chSchGoSleepS(PRSUSPENDED);           /* Wait for completion event.*/
  spip->spd_thread = NULL;
  msg = currp->p_rdymsg;
  chSysUnlock();
  return msg;
}

/*===========================================================================*/
/* Low Level Driver interrupt handlers.                                      */
/*===========================================================================*/

#if USE_STM32_SPI1 || defined(__DOXYGEN__)
/**
 * @brief SPI1 RX DMA interrupt handler (channel 2).
 */
CH_IRQ_HANDLER(Vector70) {

  CH_IRQ_PROLOGUE();

  if ((DMA1->ISR & DMA_ISR_TCIF2) != 0)
    spi_stop(&SPID1, RDY_OK);
  else
    spi_stop(&SPID1, RDY_RESET);
  DMA1->IFCR |= DMA_IFCR_CGIF2  | DMA_IFCR_CTCIF2 |
                DMA_IFCR_CHTIF2 | DMA_IFCR_CTEIF2;

  CH_IRQ_EPILOGUE();
}

/**
 * @brief SPI1 TX DMA interrupt handler (channel 3).
 */
CH_IRQ_HANDLER(Vector74) {

  CH_IRQ_PROLOGUE();

  spi_stop(&SPID1, RDY_RESET);
  DMA1->IFCR |= DMA_IFCR_CGIF3  | DMA_IFCR_CTCIF3 |
                DMA_IFCR_CHTIF3 | DMA_IFCR_CTEIF3;

  CH_IRQ_EPILOGUE();
}
#endif

#if USE_STM32_SPI2 || defined(__DOXYGEN__)
/**
 * @brief SPI2 RX DMA interrupt handler (channel 4).
 */
CH_IRQ_HANDLER(Vector78) {

  CH_IRQ_PROLOGUE();

  if ((DMA1->ISR & DMA_ISR_TCIF2) != 0)
    spi_stop(&SPID2, RDY_OK);
  else
    spi_stop(&SPID2, RDY_RESET);
  DMA2->IFCR |= DMA_IFCR_CGIF4  | DMA_IFCR_CTCIF4 |
                DMA_IFCR_CHTIF4 | DMA_IFCR_CTEIF4;

  CH_IRQ_EPILOGUE();
}

/**
 * @brief SPI2 TX DMA interrupt handler (channel 5).
 */
CH_IRQ_HANDLER(Vector7C) {

  CH_IRQ_PROLOGUE();

  spi_stop(&SPID2, RDY_RESET);
  DMA2->IFCR |= DMA_IFCR_CGIF5  | DMA_IFCR_CTCIF5 |
                DMA_IFCR_CHTIF5 | DMA_IFCR_CTEIF5;

  CH_IRQ_EPILOGUE();
}
#endif

/*===========================================================================*/
/* Low Level Driver exported functions.                                      */
/*===========================================================================*/

/**
 * @brief Low level SPI driver initialization.
 */
void spi_lld_init(void) {

  dummytx = 0xFFFF;

#if USE_STM32_SPI1
  spiObjectInit(&SPID1);
  SPID1.spd_thread  = NULL;
  SPID1.spd_spi     = SPI1;
  SPID1.spd_dmarx   = DMA1_Channel2;
  SPID1.spd_dmatx   = DMA1_Channel3;
  SPID1.spd_dmaprio = STM32_SPI1_DMA_PRIORITY << 12;
  GPIOA->CRH = (GPIOA->CRH & 0x000FFFFF) | 0xB4B00000;
#endif

#if USE_STM32_SPI2
  spiObjectInit(&SPID2);
  SPID2.spd_thread  = NULL;
  SPID2.spd_spi     = SPI2;
  SPID2.spd_dmarx   = DMA1_Channel4;
  SPID2.spd_dmatx   = DMA1_Channel5;
  SPID2.spd_dmaprio = STM32_SPI2_DMA_PRIORITY << 12;
  GPIOB->CRL = (GPIOB->CRL & 0x000FFFFF) | 0xB4B00000;
#endif
}

/**
 * @brief Configures and activates the SPI peripheral.
 *
 * @param[in] spip pointer to the @p SPIDriver object
 */
void spi_lld_start(SPIDriver *spip) {

  /* If in stopped state then enables the SPI and DMA clocks.*/
  if (spip->spd_state == SPI_STOP) {
#if USE_STM32_SPI1
    if (&SPID1 == spip) {
      NVICEnableVector(DMA1_Channel2_IRQn, STM32_SPI1_IRQ_PRIORITY);
      NVICEnableVector(DMA1_Channel3_IRQn, STM32_SPI1_IRQ_PRIORITY);
      dmaEnable(DMA1_ID);
      RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    }
#endif
#if USE_STM32_SPI2
    if (&SPID2 == spip) {
      NVICEnableVector(DMA1_Channel4_IRQn, STM32_SPI2_IRQ_PRIORITY);
      NVICEnableVector(DMA1_Channel5_IRQn, STM32_SPI2_IRQ_PRIORITY);
      dmaEnable(DMA1_ID);
      RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    }
#endif
  }

  /* SPI setup.*/
  spip->spd_spi->CR1 = spip->spd_config->spc_cr1 | SPI_CR1_MSTR;
  spip->spd_spi->CR2 = SPI_CR2_SSOE | SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN;

  /* DMA setup.*/
  spip->spd_dmarx->CPAR = (uint32_t)&spip->spd_spi->DR;
  spip->spd_dmatx->CPAR = (uint32_t)&spip->spd_spi->DR;

  /*
   * If specified in the configuration then emits a pulses train on
   * the SPI clock line without asserting any slave.
   */
  if (spip->spd_config->spc_initcnt > 0) {
    spip->spd_dmarx->CCR = DMA_CCR1_TCIE | DMA_CCR1_TEIE;
    spip->spd_dmatx->CCR = DMA_CCR1_DIR  | DMA_CCR1_TEIE;
    dma_start(spip, (size_t)spip->spd_config->spc_initcnt,
              &dummyrx, &dummytx);
    (void) spi_start_wait(spip);
  }
}

/**
 * @brief Deactivates the SPI peripheral.
 *
 * @param[in] spip pointer to the @p SPIDriver object
 */
void spi_lld_stop(SPIDriver *spip) {

  /* If in ready state then disables the SPI clock.*/
  if (spip->spd_state == SPI_READY) {
#if USE_STM32_SPI1
    if (&SPID1 == spip) {
      NVICDisableVector(DMA1_Channel2_IRQn);
      NVICDisableVector(DMA1_Channel3_IRQn);
      dmaDisable(DMA1_ID);
      RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
    }
#endif
#if USE_STM32_SPI2
    if (&SPID2 == spip) {
      NVICDisableVector(DMA1_Channel4_IRQn);
      NVICDisableVector(DMA1_Channel5_IRQn);
      dmaDisable(DMA1_ID);
      RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN;
    }
#endif
  }
}

/**
 * @brief Asserts the slave select signal and prepares for transfers.
 *
 * @param[in] spip pointer to the @p SPIDriver object
 */
void spi_lld_select(SPIDriver *spip) {

  palClearPad(spip->spd_config->spc_ssport, spip->spd_config->spc_sspad);
}

/**
 * @brief Deasserts the slave select signal.
 * @details The previously selected peripheral is unselected.
 *
 * @param[in] spip pointer to the @p SPIDriver object
 */
void spi_lld_unselect(SPIDriver *spip) {

  palSetPad(spip->spd_config->spc_ssport, spip->spd_config->spc_sspad);
}

/**
 * @brief Exchanges data on the SPI bus.
 * @details This function performs a simultaneous transmit/receive operation.
 *
 * @param[in] spip pointer to the @p SPIDriver object
 * @param n number of words to exchange
 * @param rxbuf the pointer to the receive buffer
 * @param txbuf the pointer to the transmit buffer
 * @return The operation status is returned.
 * @retval RDY_OK operation complete.
 * @retval RDY_RESET hardware failure.
 *
 * @note The buffers are organized as uint8_t arrays for data sizes below or
 *       equal to 8 bits else it is organized as uint16_t arrays.
 */
msg_t spi_lld_exchange(SPIDriver *spip, size_t n, void *rxbuf, void *txbuf) {

  spip->spd_dmarx->CCR = DMA_CCR1_TCIE | DMA_CCR1_MINC |
                         DMA_CCR1_TEIE | DMA_CCR1_EN;
  spip->spd_dmatx->CCR = DMA_CCR1_DIR  | DMA_CCR1_MINC |
                         DMA_CCR1_TEIE | DMA_CCR1_EN;
  dma_start(spip, n, rxbuf, txbuf);
  return spi_start_wait(spip);
}

/**
 * @brief Sends data ever the SPI bus.
 *
 * @param[in] spip pointer to the @p SPIDriver object
 * @param n number of words to send
 * @param txbuf the pointer to the transmit buffer
 * @return The operation status is returned.
 * @retval RDY_OK operation complete.
 * @retval RDY_RESET hardware failure.
 *
 * @note The buffers are organized as uint8_t arrays for data sizes below or
 *       equal to 8 bits else it is organized as uint16_t arrays.
 */
msg_t spi_lld_send(SPIDriver *spip, size_t n, void *txbuf) {

  spip->spd_dmarx->CCR = DMA_CCR1_TCIE |
                         DMA_CCR1_TEIE | DMA_CCR1_EN;
  spip->spd_dmatx->CCR = DMA_CCR1_DIR  | DMA_CCR1_MINC |
                         DMA_CCR1_TEIE | DMA_CCR1_EN;
  dma_start(spip, n, &dummyrx, txbuf);
  return spi_start_wait(spip);
}

/**
 * @brief Receives data from the SPI bus.
 *
 * @param[in] spip pointer to the @p SPIDriver object
 * @param n number of words to receive
 * @param rxbuf the pointer to the receive buffer
 * @return The operation status is returned.
 * @retval RDY_OK operation complete.
 * @retval RDY_RESET hardware failure.
 *
 * @note The buffers are organized as uint8_t arrays for data sizes below or
 *       equal to 8 bits else it is organized as uint16_t arrays.
 */
msg_t spi_lld_receive(SPIDriver *spip, size_t n, void *rxbuf) {

  spip->spd_dmarx->CCR = DMA_CCR1_TCIE | DMA_CCR1_MINC |
                         DMA_CCR1_TEIE | DMA_CCR1_EN;
  spip->spd_dmatx->CCR = DMA_CCR1_DIR  |
                         DMA_CCR1_TEIE | DMA_CCR1_EN;
  dma_start(spip, n, rxbuf, &dummytx);
  return spi_start_wait(spip);
}

/** @} */
