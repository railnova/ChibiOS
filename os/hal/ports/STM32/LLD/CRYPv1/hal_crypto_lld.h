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
 * @file    CRYPv1/hal_cry_lld.h
 * @brief   STM32 cryptographic subsystem low level driver header.
 *
 * @addtogroup CRYPTO
 * @{
 */

#ifndef HAL_CRYPTO_LLD_H
#define HAL_CRYPTO_LLD_H

#if (HAL_USE_CRY == TRUE) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    STM32 configuration options
 * @{
 */
/**
 * @brief   CRYP1 driver enable switch.
 * @details If set to @p TRUE the support for CRYP1 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(STM32_CRY_USE_CRYP1) || defined(__DOXYGEN__)
#define STM32_CRY_USE_CRYP1                 FALSE
#endif

/**
 * @brief   HASH1 driver enable switch.
 * @details If set to @p TRUE the support for CRYP1 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(STM32_CRY_USE_HASH1) || defined(__DOXYGEN__)
#define STM32_CRY_USE_HASH1                 FALSE
#endif

/**
 * @brief   CRYP1 interrupt priority level setting.
 */
#if !defined(STM32_CRY_CRYP1_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define STM32_CRY_CRYP1_IRQ_PRIORITY        9
#endif

/**
 * @brief   HASH1 interrupt priority level setting.
 */
#if !defined(STM32_CRY_HASH1_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define STM32_CRY_HASH1_IRQ_PRIORITY        9
#endif

/**
 * @brief   HASH1 DMA priority (0..3|lowest..highest).
 */
#if !defined(STM32_CRY_HASH1_DMA_PRIORITY) || defined(__DOXYGEN__)
#define STM32_CRY_HASH1_DMA_PRIORITY        0
#endif
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if (STM32_CRY_USE_CRYP1 == TRUE) || (STM32_CRY_USE_HASH1 == TRUE) ||       \
    defined (__DOXYGEN__)
#define STM32_CRY_ENABLED1                  TRUE
#else
#define STM32_CRY_ENABLED1                  FALSE
#endif

#if !defined (STM32_HAS_CRYP1)
#define STM32_HAS_CRYP1                     FALSE
#endif

#if !defined (STM32_HAS_HASH1)
#define STM32_HAS_HASH1                     FALSE
#endif

#if STM32_CRY_USE_CRYP1 && !STM32_HAS_CRYP1
#error "CRYP1 not present in the selected device"
#endif

#if STM32_CRY_USE_HASH1 && !STM32_HAS_HASH1
#error "HASH1 not present in the selected device"
#endif

#if !STM32_CRY_ENABLED1
#error "CRY driver activated but no CRYP nor HASH peripheral assigned"
#endif

#if STM32_CRY_USE_HASH1 &&                                                  \
    !OSAL_IRQ_IS_VALID_PRIORITY(STM32_CRY_HASH1_IRQ_PRIORITY)
#error "Invalid IRQ priority assigned to HASH1"
#endif

#if STM32_CRY_USE_CRYP1 &&                                                  \
    !OSAL_IRQ_IS_VALID_PRIORITY(STM32_CRY_CRYP1_IRQ_PRIORITY)
#error "Invalid IRQ priority assigned to CRYP1"
#endif

/* Devices with DMAMUX require a different kind of check.*/
#if STM32_DMA_SUPPORTS_DMAMUX

#if STM32_CRY_USE_HASH1
#if !defined(STM32_CRY_HASH1_DMA_CHANNEL)
#error "HASH1 DMA channel not defined"
#endif
#if !STM32_DMA_IS_VALID_CHANNEL(STM32_CRY_HASH1_DMA_CHANNEL)
#error "Invalid DMA channel assigned to HASH1"
#endif
#if !STM32_DMA_IS_VALID_PRIORITY(STM32_CRY_HASH1_DMA_PRIORITY)
#error "Invalid DMA priority assigned to HASH1"
#endif
#endif /* !STM32_CRY_USE_HASH1 */

#else /* !STM32_DMA_SUPPORTS_DMAMUX */

/* Sanity checks on DMA streams settings in mcuconf.h.*/
#if STM32_CRY_USE_HASH1
#if !defined(STM32_CRY_HASH1_DMA_STREAM)
#error "HASH1 DMA streams not defined"
#endif
#if !STM32_DMA_IS_VALID_ID(STM32_CRY_HASH1_DMA_STREAM, STM32_HASH1_DMA_MSK)
#error "invalid DMA stream associated to HASH1"
#endif
#if !STM32_DMA_IS_VALID_PRIORITY(STM32_CRY_HASH1_DMA_PRIORITY)
#error "Invalid DMA priority assigned to HASH1"
#endif
#endif /* !STM32_CRY_USE_HASH1 */

#endif /* !STM32_DMA_SUPPORTS_DMAMUX */

#if !defined(STM32_DMA_REQUIRED)
#define STM32_DMA_REQUIRED
#endif

/**
 * @name    Driver capability switches
 * @{
 */
#if STM32_CRY_USE_CRYP1 || defined (__DOXYGEN__)
#define CRY_LLD_SUPPORTS_AES                TRUE
#define CRY_LLD_SUPPORTS_AES_ECB            TRUE
#define CRY_LLD_SUPPORTS_AES_CBC            TRUE
#define CRY_LLD_SUPPORTS_AES_CFB            TRUE
#define CRY_LLD_SUPPORTS_AES_CTR            TRUE
#define CRY_LLD_SUPPORTS_AES_GCM            TRUE
#define CRY_LLD_SUPPORTS_DES                TRUE
#define CRY_LLD_SUPPORTS_DES_ECB            TRUE
#define CRY_LLD_SUPPORTS_DES_CBC            TRUE
#else
#define CRY_LLD_SUPPORTS_AES                FALSE
#define CRY_LLD_SUPPORTS_AES_ECB            FALSE
#define CRY_LLD_SUPPORTS_AES_CBC            FALSE
#define CRY_LLD_SUPPORTS_AES_CFB            FALSE
#define CRY_LLD_SUPPORTS_AES_CTR            FALSE
#define CRY_LLD_SUPPORTS_AES_GCM            FALSE
#define CRY_LLD_SUPPORTS_DES                FALSE
#define CRY_LLD_SUPPORTS_DES_ECB            FALSE
#define CRY_LLD_SUPPORTS_DES_CBC            FALSE
#endif
#if STM32_CRY_USE_HASH1 || defined (__DOXYGEN__)
#define CRY_LLD_SUPPORTS_SHA1               FALSE
#define CRY_LLD_SUPPORTS_SHA256             TRUE
#define CRY_LLD_SUPPORTS_SHA512             FALSE
#define CRY_LLD_SUPPORTS_HMAC_SHA256        TRUE
#define CRY_LLD_SUPPORTS_HMAC_SHA512        FALSE
#else
#define CRY_LLD_SUPPORTS_SHA1               FALSE
#define CRY_LLD_SUPPORTS_SHA256             FALSE
#define CRY_LLD_SUPPORTS_SHA512             FALSE
#define CRY_LLD_SUPPORTS_HMAC_SHA256        FALSE
#define CRY_LLD_SUPPORTS_HMAC_SHA512        FALSE
#endif
/** @} */

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   CRY key identifier type.
 */
typedef uint32_t crykey_t;

/**
 * @brief   Type of a structure representing an CRY driver.
 */
typedef struct CRYDriver CRYDriver;

/**
 * @brief   Driver configuration structure.
 * @note    It could be empty on some architectures.
 */
typedef struct {
  uint32_t                  dummy;
} CRYConfig;

/**
 * @brief   Structure representing an CRY driver.
 */
struct CRYDriver {
  /**
   * @brief   Driver state.
   */
  crystate_t                state;
  /**
   * @brief   Current configuration data.
   */
  const CRYConfig           *config;
#if defined(CRY_DRIVER_EXT_FIELDS)
  CRY_DRIVER_EXT_FIELDS
#endif
  /* End of the mandatory fields.*/
#if STM32_CRY_USE_CRYP1 || defined (__DOXYGEN__)
#endif
#if STM32_CRY_USE_HASH1 || defined (__DOXYGEN__)
  /**
   * @brief   Thread reference for hash operations.
   */
  thread_reference_t        hash_tr;
  /**
   * @brief   Hash DMA stream.
   */
  const stm32_dma_stream_t  *dma_hash;
#endif
};

#if (CRY_LLD_SUPPORTS_SHA1 == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   Type of a SHA1 context.
 */
typedef struct {
  uint32_t dummy;
} SHA1Context;
#endif

#if (CRY_LLD_SUPPORTS_SHA256 == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   Type of a SHA256 context.
 */
typedef struct {
  /**
   * @brief   Last data to be hashed on finalization.
   */
  uint32_t      last_data;
  /**
   * @brief   Size, in bits, of the last data.
   */
  uint32_t      last_size;
} SHA256Context;
#endif

#if (CRY_LLD_SUPPORTS_SHA512 == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   Type of a SHA512 context.
 */
typedef struct {
  uint32_t dummy;
} SHA512Context;
#endif

#if (CRY_LLD_SUPPORTS_HMAC_SHA256 == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   Type of a HMAC_SHA256 context.
 */
typedef struct {
  uint32_t dummy;
} HMACSHA256Context;
#endif

#if (CRY_LLD_SUPPORTS_HMAC_SHA512 == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   Type of a HMAC_SHA512 context.
 */
typedef struct {
  uint32_t dummy;
} HMACSHA512Context;
#endif

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if (STM32_CRY_ENABLED1 == TRUE) && !defined(__DOXYGEN__)
extern CRYDriver CRYD1;
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void cry_lld_init(void);
  void cry_lld_start(CRYDriver *cryp);
  void cry_lld_stop(CRYDriver *cryp);
#if (CRY_LLD_SUPPORTS_AES == TRUE) ||                                       \
    (CRY_LLD_SUPPORTS_AES_ECB == TRUE) ||                                   \
    (CRY_LLD_SUPPORTS_AES_CBC == TRUE) ||                                   \
    (CRY_LLD_SUPPORTS_AES_CFB == TRUE) ||                                   \
    (CRY_LLD_SUPPORTS_AES_CTR == TRUE) ||                                   \
    (CRY_LLD_SUPPORTS_AES_GCM == TRUE) ||                                   \
    defined(__DOXYGEN__)
  cryerror_t cry_lld_aes_loadkey(CRYDriver *cryp,
                                 size_t size,
                                 const uint8_t *keyp);
#endif
#if (CRY_LLD_SUPPORTS_AES == TRUE) || defined(__DOXYGEN__)
  cryerror_t cry_lld_encrypt_AES(CRYDriver *cryp,
                                 crykey_t key_id,
                                 const uint8_t *in,
                                 uint8_t *out);
  cryerror_t cry_lld_decrypt_AES(CRYDriver *cryp,
                                 crykey_t key_id,
                                 const uint8_t *in,
                                 uint8_t *out);
#endif
#if (CRY_LLD_SUPPORTS_AES_ECB == TRUE) || defined(__DOXYGEN__)
  cryerror_t cry_lld_encrypt_AES_ECB(CRYDriver *cryp,
                                     crykey_t key_id,
                                     size_t size,
                                     const uint8_t *in,
                                     uint8_t *out);
  cryerror_t cry_lld_decrypt_AES_ECB(CRYDriver *cryp,
                                     crykey_t key_id,
                                     size_t size,
                                     const uint8_t *in,
                                     uint8_t *out);
#endif
#if (CRY_LLD_SUPPORTS_AES_CBC == TRUE) || defined(__DOXYGEN__)
  cryerror_t cry_lld_encrypt_AES_CBC(CRYDriver *cryp,
                                     crykey_t key_id,
                                     size_t size,
                                     const uint8_t *in,
                                     uint8_t *out,
                                     const uint8_t *iv);
  cryerror_t cry_lld_decrypt_AES_CBC(CRYDriver *cryp,
                                     crykey_t key_id,
                                     size_t size,
                                     const uint8_t *in,
                                     uint8_t *out,
                                     const uint8_t *iv);
#endif
#if (CRY_LLD_SUPPORTS_AES_CFB == TRUE) || defined(__DOXYGEN__)
  cryerror_t cry_lld_encrypt_AES_CFB(CRYDriver *cryp,
                                     crykey_t key_id,
                                     size_t size,
                                     const uint8_t *in,
                                     uint8_t *out,
                                     const uint8_t *iv);
  cryerror_t cry_lld_decrypt_AES_CFB(CRYDriver *cryp,
                                     crykey_t key_id,
                                     size_t size,
                                     const uint8_t *in,
                                     uint8_t *out,
                                     const uint8_t *iv);
#endif
#if (CRY_LLD_SUPPORTS_AES_CTR == TRUE) || defined(__DOXYGEN__)
  cryerror_t cry_lld_encrypt_AES_CTR(CRYDriver *cryp,
                                     crykey_t key_id,
                                     size_t size,
                                     const uint8_t *in,
                                     uint8_t *out,
                                     const uint8_t *iv);
  cryerror_t cry_lld_decrypt_AES_CTR(CRYDriver *cryp,
                                     crykey_t key_id,
                                     size_t size,
                                     const uint8_t *in,
                                     uint8_t *out,
                                     const uint8_t *iv);
#endif
#if (CRY_LLD_SUPPORTS_AES_GCM == TRUE) || defined(__DOXYGEN__)
  cryerror_t cry_lld_encrypt_AES_GCM(CRYDriver *cryp,
                                     crykey_t key_id,
                                     size_t size,
                                     const uint8_t *in,
                                     uint8_t *out,
                                     const uint8_t *iv,
                                     size_t aadsize,
                                     const uint8_t *aad,
                                     uint8_t *authtag);
  cryerror_t cry_lld_decrypt_AES_GCM(CRYDriver *cryp,
                                     crykey_t key_id,
                                     size_t size,
                                     const uint8_t *in,
                                     uint8_t *out,
                                     const uint8_t *iv,
                                     size_t aadsize,
                                     const uint8_t *aad,
                                     uint8_t *authtag);
#endif
#if (CRY_LLD_SUPPORTS_DES == TRUE) ||                                       \
    (CRY_LLD_SUPPORTS_DES_ECB == TRUE) ||                                   \
    (CRY_LLD_SUPPORTS_DES_CBC == TRUE) ||                                   \
    defined(__DOXYGEN__)
  cryerror_t cry_lld_des_loadkey(CRYDriver *cryp,
                                 size_t size,
                                 const uint8_t *keyp);
#endif
#if (CRY_LLD_SUPPORTS_DES == TRUE) || defined(__DOXYGEN__)
  cryerror_t cry_lld_encrypt_DES(CRYDriver *cryp,
                                 crykey_t key_id,
                                 const uint8_t *in,
                                 uint8_t *out);
  cryerror_t cry_lld_decrypt_DES(CRYDriver *cryp,
                                 crykey_t key_id,
                                 const uint8_t *in,
                                 uint8_t *out);
#endif
#if (CRY_LLD_SUPPORTS_DES_ECB == TRUE) || defined(__DOXYGEN__)
  cryerror_t cry_lld_encrypt_DES_ECB(CRYDriver *cryp,
                                    crykey_t key_id,
                                    size_t size,
                                    const uint8_t *in,
                                    uint8_t *out);
  cryerror_t cry_lld_decrypt_DES_ECB(CRYDriver *cryp,
                                     crykey_t key_id,
                                     size_t size,
                                     const uint8_t *in,
                                     uint8_t *out);
#endif
#if (CRY_LLD_SUPPORTS_DES_CBC == TRUE) || defined(__DOXYGEN__)
  cryerror_t cry_lld_encrypt_DES_CBC(CRYDriver *cryp,
                                     crykey_t key_id,
                                     size_t size,
                                     const uint8_t *in,
                                     uint8_t *out,
                                     const uint8_t *iv);
  cryerror_t cry_lld_decrypt_DES_CBC(CRYDriver *cryp,
                                     crykey_t key_id,
                                     size_t size,
                                     const uint8_t *in,
                                     uint8_t *out,
                                     const uint8_t *iv);
#endif
#if (CRY_LLD_SUPPORTS_SHA1 == TRUE) || defined(__DOXYGEN__)
  cryerror_t cry_lld_SHA1_init(CRYDriver *cryp, SHA1Context *sha1ctxp);
  cryerror_t cry_lld_SHA1_update(CRYDriver *cryp, SHA1Context *sha1ctxp,
                                 size_t size, const uint8_t *in);
  cryerror_t cry_lld_SHA1_final(CRYDriver *cryp, SHA1Context *sha1ctxp,
                                uint8_t *out);
#endif
#if (CRY_LLD_SUPPORTS_SHA256 == TRUE) || defined(__DOXYGEN__)
  cryerror_t cry_lld_SHA256_init(CRYDriver *cryp, SHA256Context *sha256ctxp);
  cryerror_t cry_lld_SHA256_update(CRYDriver *cryp, SHA256Context *sha256ctxp,
                                   size_t size, const uint8_t *in);
  cryerror_t cry_lld_SHA256_final(CRYDriver *cryp, SHA256Context *sha256ctxp,
                                  uint8_t *out);
#endif
#if (CRY_LLD_SUPPORTS_SHA512 == TRUE) || defined(__DOXYGEN__)
  cryerror_t cry_lld_SHA512_init(CRYDriver *cryp, SHA512Context *sha512ctxp);
  cryerror_t cry_lld_SHA512_update(CRYDriver *cryp, SHA512Context *sha512ctxp,
                                   size_t size, const uint8_t *in);
  cryerror_t cry_lld_SHA512_final(CRYDriver *cryp, SHA512Context *sha512ctxp,
                                  uint8_t *out);
#endif
#if (CRY_LLD_SUPPORTS_HMAC_SHA256 == TRUE) ||                               \
    (CRY_LLD_SUPPORTS_HMAC_SHA512 == TRUE) ||                               \
    defined(__DOXYGEN__)
  cryerror_t cry_lld_hmac_loadkey(CRYDriver *cryp,
                                  size_t size,
                                  const uint8_t *keyp);
#endif
#if (CRY_LLD_SUPPORTS_HMAC_SHA256 == TRUE) || defined(__DOXYGEN__)
  cryerror_t cry_lld_HMACSHA256_init(CRYDriver *cryp,
                                     HMACSHA256Context *hmacsha256ctxp);
  cryerror_t cry_lld_HMACSHA256_update(CRYDriver *cryp,
                                       HMACSHA256Context *hmacsha256ctxp,
                                       size_t size, const uint8_t *in);
  cryerror_t cry_lld_HMACSHA256_final(CRYDriver *cryp,
                                      HMACSHA256Context *hmacsha256ctxp,
                                      uint8_t *out);
#endif
#if (CRY_LLD_SUPPORTS_HMAC_SHA512 == TRUE) || defined(__DOXYGEN__)
  cryerror_t cry_lld_HMACSHA512_init(CRYDriver *cryp,
                                     HMACSHA512Context *hmacsha512ctxp);
  cryerror_t cry_lld_HMACSHA512_update(CRYDriver *cryp,
                                       HMACSHA512Context *hmacsha512ctxp,
                                       size_t size, const uint8_t *in);
  cryerror_t cry_lld_HMACSHA512_final(CRYDriver *cryp,
                                      HMACSHA512Context *hmacsha512ctxp,
                                      uint8_t *out);
#endif
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_CRY == TRUE */

#endif /* HAL_CRYPTO_LLD_H */

/** @} */
