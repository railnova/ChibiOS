/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

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
 * @file    hal_sio.h
 * @brief   SIO Driver macros and structures.
 *
 * @addtogroup SIO
 * @{
 */

#ifndef HAL_SIO_H
#define HAL_SIO_H

#if (HAL_USE_SIO == TRUE) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @name    SIO status flags
 * @{
 */
#define SIO_NO_ERROR           0    /**< @brief No pending conditions.      */
#define SIO_PARITY_ERROR       4    /**< @brief Parity error happened.      */
#define SIO_FRAMING_ERROR      8    /**< @brief Framing error happened.     */
#define SIO_OVERRUN_ERROR      16   /**< @brief Overflow happened.          */
#define SIO_NOISE_ERROR        32   /**< @brief Noise on the line.          */
#define SIO_BREAK_DETECTED     64   /**< @brief Break detected.             */
/** @} */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    SIO configuration options
 * @{
 */

/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Driver state machine possible states.
 */
typedef enum {
  SIO_UNINIT = 0,                   /**< Not initialized.                   */
  SIO_STOP = 1,                     /**< Stopped.                           */
  SIO_READY = 2                     /**< Ready.                             */
} siostate_t;

#include "hal_sio_lld.h"

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/**
 * @brief   Reads data from the RX FIFO.
 * @details This function is non-blocking, data is read if present and the
 *          effective amount is returned.
 * @note    This function can be called from any context but it is meant to
 *          be called from the @p rxne_cb callback handler.
 *
 * @param[in] siop      pointer to the @p SIODriver object
 * @param[in] buffer    buffer for the received data
 * @param[in] size      maximum number of bytes to read
 * @return              The number of received bytes.
 *
 * @xclass
 */
#define sioReadX(siop, buffer, size) sio_lld_read(siop, buffer, size)

/**
 * @brief   Writes data into the TX FIFO.
 * @details This function is non-blocking, data is written if there is space
 *          in the FIFO and the effective amount is returned.
 * @note    This function can be called from any context but it is meant to
 *          be called from the @p txnf_cb callback handler.
 *
 * @param[in] siop      pointer to the @p SIODriver object
 * @param[out] buffer    buffer containing the data to be transmitted
 * @param[in] size      maximum number of bytes to read
 * @return              The number of transmitted bytes.
 *
 * @xclass
 */
#define sioWriteX(siop, buffer, size) sio_lld_write(siop, buffer, size)

/**
 * @brief   Control operation on a serial port.
 *
 * @param[in] siop      pointer to the @p SIODriver object
 * @param[in] operation control operation code
 * @param[in,out] arg   operation argument
 *
 * @return              The control operation status.
 * @retval MSG_OK       in case of success.
 * @retval MSG_TIMEOUT  in case of operation timeout.
 * @retval MSG_RESET    in case of operation reset.
 *
 * @xclass
 */
#define sioControlX(siop, operation, arg) sio_lld_control(siop, operation, arg)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void sioInit(void);
  void sioObjectInit(SIODriver *siop);
  void sioStart(SIODriver *siop, const SIOConfig *config);
  void sioStop(SIODriver *siop);
  msg_t sioControl(SIODriver *siop, unsigned int operation, void *arg);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_SIO == TRUE */

#endif /* HAL_SIO_H */

/** @} */
