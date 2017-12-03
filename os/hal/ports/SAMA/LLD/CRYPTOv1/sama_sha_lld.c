/*
    ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio

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
#include "hal.h"
#include <string.h>
#if (HAL_USE_CRY == TRUE) || defined(__DOXYGEN__)

#include "sama_crypto_lld.h"


static inline uint32_t min_u32(uint32_t a, uint32_t b)
{
	return a < b ? a : b;
}
struct sha_data {
	uint32_t remaining;
	uint32_t processed;
	uint32_t block_size;
	uint32_t output_size;
	shadalgo_t algo;
	uint8_t hmac;
};


static uint32_t shaOutputSize(shadalgo_t algo);
static uint32_t shadPaddedMessSize(uint8_t mode, uint32_t len);
uint8_t shaBlockSize(shadalgo_t algo);
static void loadData(const uint8_t* data, int len);
static void readData(const uint8_t* data, int len);
static uint32_t processBlockPolling(CRYDriver *cryp, uint32_t len, uint32_t block_size);
static uint32_t processBlockDMA(CRYDriver *cryp, uint32_t len, uint32_t block_size);
static void update(CRYDriver *cryp,struct sha_data *shadata,const uint8_t* data, uint32_t data_size);

static uint32_t fillPadding(struct sha_data *shadata, uint32_t len, uint8_t* buffer);



uint8_t shaDigestSize(shadalgo_t algo);

uint8_t shaDigestSize(shadalgo_t algo)
{
	switch(algo)
	{
		case CRY_SHA_1:
			return 20;
			break;
		case CRY_SHA_224:
			return 28;
			break;
		case CRY_SHA_256:
			return 32;
			break;
		case CRY_SHA_384:
			return 48;
			break;
		case CRY_SHA_512:
					return 64;
					break;
		default:
			return 0;
	}
}




int sha_finish(CRYDriver *cryp,struct sha_data *shadata,const uint8_t* buffer,uint32_t buffer_size)
{

	uint32_t padding_len=0;

	if (buffer_size < shadata->output_size)
		return -1;

	//pad data for the end of the buffer
	padding_len = fillPadding(shadata,
									shadata->processed + shadata->remaining,
									&cryp->sha_buffer[shadata->remaining]
									);

	if (cryp->config->transfer_mode == TRANSFER_POLLING)
		processBlockPolling(cryp, shadata->remaining + padding_len, shadata->block_size);
	else
		processBlockDMA(cryp, shadata->remaining + padding_len, shadata->block_size);

	readData(buffer, buffer_size);

	return 0;
}

cryerror_t sama_sha_lld_process(CRYDriver *cryp,
										shaparams_t *params,
										const uint8_t *in,
										uint8_t *out,
										size_t indata_len
										)
{
	uint32_t algoregval;
	struct sha_data shadata;

	if (!(cryp->enabledPer & SHA_PER)) {
		cryp->enabledPer |= SHA_PER;
		pmcEnableSHA();
	}

	osalMutexLock(&cryp->mutex);

	shadata.processed = 0;
	shadata.remaining = 0;
	shadata.output_size = shaOutputSize(params->algo);
	shadata.block_size = shaBlockSize(params->algo);
	shadata.algo = params->algo;

	if (shadata.output_size == 0) {
		return CRY_ERR_INV_ALGO;
	}

	switch (params->algo) {
	case CRY_SHA_1:
		algoregval = SHA_MR_ALGO_SHA1;
		break;
	case CRY_SHA_224:
		algoregval = SHA_MR_ALGO_SHA224;
		break;
	case CRY_SHA_256:
		algoregval = SHA_MR_ALGO_SHA256;
		break;
#ifdef SHA_MR_ALGO_SHA384
	case CRY_SHA_384:
		algoregval = SHA_MR_ALGO_SHA384;
		break;
#endif
#ifdef SHA_MR_ALGO_SHA512
	case CRY_SHA_512:
		algoregval = SHA_MR_ALGO_SHA512;
		break;
#endif
	default:
		return CRY_ERR_INV_ALGO;
	}

	//soft reset
	SHA->SHA_CR = SHA_CR_SWRST;

	if (cryp->config->transfer_mode == TRANSFER_POLLING) {
		algoregval |= SHA_MR_SMOD_MANUAL_START;
	} else {
		algoregval |= SHA_MR_SMOD_IDATAR0_START;

		cryp->dmawith = DMA_DATA_WIDTH_WORD;
		cryp->dmachunksize = DMA_CHUNK_SIZE_16;

		cryp->txdmamode = XDMAC_CC_TYPE_PER_TRAN |
		XDMAC_CC_PROT_SEC |
		XDMAC_CC_MBSIZE_SINGLE |
		XDMAC_CC_DSYNC_MEM2PER | XDMAC_CC_CSIZE(cryp->dmachunksize) |
		XDMAC_CC_DWIDTH(cryp->dmawith) |
		XDMAC_CC_SIF_AHB_IF0 |
		XDMAC_CC_DIF_AHB_IF1 |
		XDMAC_CC_SAM_INCREMENTED_AM |
		XDMAC_CC_DAM_FIXED_AM |
		XDMAC_CC_PERID(PERID_SHA_TX);

		cryp->rxdmamode = 0xFFFFFFFF;

		dmaChannelSetMode(cryp->dmatx, cryp->txdmamode);
	}

	//configure
	SHA->SHA_MR = algoregval | SHA_MR_PROCDLY_LONGEST;

	//enable interrupt
	SHA->SHA_IER = SHA_IER_DATRDY;

	uint32_t buf_in_size;
	const uint8_t *p = in;

	while (indata_len) {
		buf_in_size = min_u32(indata_len, SHA_UPDATE_LEN);

		//First block
		if (!shadata.processed) {
			SHA->SHA_CR = SHA_CR_FIRST;
		}

		update(cryp, &shadata, in, buf_in_size);

		p += buf_in_size;
		indata_len -= buf_in_size;
	}

	sha_finish(cryp, &shadata, out, shadata.output_size);

	osalMutexUnlock(&cryp->mutex);

	return CRY_NOERROR;
}


static uint32_t shaOutputSize(shadalgo_t algo)
{
	switch (algo) {
	case CRY_SHA_1:
		return 20;
	case CRY_SHA_224:
		return 28;
	case CRY_SHA_256:
		return 32;
	case CRY_SHA_384:
		return 48;
	case CRY_SHA_512:
		return 64;
	default:
		return 0;
	}
}

static uint32_t shadPaddedMessSize(uint8_t mode, uint32_t len)
{
	uint32_t k;

	switch (mode) {
	case CRY_SHA_1:
	case CRY_SHA_224:
	case CRY_SHA_256:
		k = (512 + 448 - (((len * 8) % 512) + 1)) % 512;
		len += (k - 7) / 8 + 9;
		break;
	case CRY_SHA_384:
	case CRY_SHA_512:
		k = (1024 + 896 - (((len * 8) % 1024) + 1)) % 1024;
		len += (k - 7) / 8 + 17;
		break;
	}
	return len;
}

uint8_t shaBlockSize(shadalgo_t algo)
{
	if ( (algo == CRY_SHA_384) || (algo == CRY_SHA_512) ) {
		return 128;
	}

		return 64;
}


static void loadData(const uint8_t* data, int len)
{
	int i;
	int32_t value;

	for (i = 0; i < (len / 4) && i < 32; i++) {
		memcpy(&value, &data[i * 4], 4);
		if (i < 16)
			SHA->SHA_IDATAR[i] = value;
		else
			SHA->SHA_IODATAR[i - 16] = value;
	}
}
static void readData(const uint8_t* data, int len)
{
	int i;
	int32_t value;

	for (i = 0; i < (len / 4) && i < 16; i++) {
		value = SHA->SHA_IODATAR[i];
		memcpy(&data[i * 4], &value, 4);
	}
}


static uint32_t processBlockPolling(CRYDriver *cryp,uint32_t len, uint32_t block_size)
{
	uint32_t processed = 0;

	while ((len - processed) >= block_size) {

		// load data in the sha input registers
		loadData(&cryp->sha_buffer[processed], block_size);

		SHA->SHA_CR = SHA_CR_START;

		// Wait for data ready
		while ((SHA->SHA_ISR & SHA_ISR_DATRDY) == 0);

		processed += block_size;
	}

	return processed;
}

static uint32_t processBlockDMA(CRYDriver *cryp, uint32_t len, uint32_t block_size)
{
	uint32_t processed = 0;

	while ((len - processed) >= block_size) {

		// load data in the sha input registers
		// Writing channel
		dmaChannelSetSource(cryp->dmatx, &cryp->sha_buffer[processed]);
		dmaChannelSetDestination(cryp->dmatx, SHA->SHA_IDATAR);
		dmaChannelSetTransactionSize(cryp->dmatx,
				(block_size / DMA_DATA_WIDTH_TO_BYTE(cryp->dmawith)));

		osalSysLock();

		dmaChannelEnable(cryp->dmatx);

		osalThreadSuspendS(&cryp->thread);

		osalSysUnlock();

		processed += block_size;
	}

	return processed;
}

static void update(CRYDriver *cryp,struct sha_data *shadata,const uint8_t* data, uint32_t data_size)
{
	uint32_t i;
	uint32_t processed;

	//check data from previous update
	if (shadata->remaining) {
		//complete previous data
		uint32_t complement = min_u32(data_size, shadata->block_size - shadata->remaining);
		memcpy(&cryp->sha_buffer[shadata->remaining], data, complement);
		shadata->remaining += complement;
		data += complement;
		data_size -= complement;

		//if data is complete process the block
		if (shadata->remaining == shadata->block_size) {
			if (cryp->config->transfer_mode == TRANSFER_POLLING )
				processBlockPolling(cryp,shadata->remaining, shadata->block_size);
			else
				processBlockDMA(cryp, shadata->remaining, shadata->block_size);

			shadata->processed += shadata->block_size;
			shadata->remaining = 0;
		} else {
			//complete processing in the next update/
			return;
		}
	}

	// Process blocks
	if (cryp->config->transfer_mode == TRANSFER_POLLING )
		processed = processBlockPolling(cryp, data_size, shadata->block_size);
	else
		processed = processBlockDMA(cryp, data_size, shadata->block_size);

	shadata->processed += processed;

	//check remaining data and process
	shadata->remaining = data_size - processed;
	if (shadata->remaining)
	{
		for (i=0;i<shadata->remaining;i++)
			cryp->sha_buffer[i] = data[processed+i];
	}
}

static uint32_t fillPadding(struct sha_data *shadata, uint32_t len, uint8_t* buffer)
{
	uint32_t padding_len = shadPaddedMessSize(shadata->algo, len) - len;
	uint32_t k = padding_len - 9;

	 osalDbgAssert( padding_len <= (SHA_MAX_PADDING_LEN - shadata->remaining),
	                "invalid buffer size");


	// Append "1" bit and seven "0" bits to the end of the message
	*buffer++ = 0x80;
	// Add k "0" bits
	memset(buffer, 0, k);
	buffer += k;
	// length is at the end of message (64-bit)
	*buffer++ = 0;
	*buffer++ = 0;
	*buffer++ = 0;
	*buffer++ = (len >> 29) & 0xFF;
	*buffer++ = (len >> 21) & 0xFF;
	*buffer++ = (len >> 13) & 0xFF;
	*buffer++ = (len >> 5) & 0xFF;
	*buffer++ = (len << 3) & 0xFF;
	return padding_len;
}
#endif /* HAL_USE_CRY */

/** @} */


