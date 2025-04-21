/*
 * msg_security.c
 *
 *  Created on: Apr 7, 2025
 *      Author: Naveen S
 */

#include "stm32h5xx_hal.h"
#include "stm32h5xx_nucleo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmox_crypto.h"



void Error_Handler(void);
int can_tx_msg(uint8_t *ptr, size_t size);

#define TAG_SIZE 16

__attribute__((section(".secure_flash"))) static const uint8_t Key[] =
{
		0x1c, 0x92, 0x40, 0xa5, 0xeb, 0x55, 0xd3, 0x8a, 0xf3, 0x33, 0x88, 0x86, 0x04, 0xf6, 0xb5, 0xf0,
		0x47, 0x39, 0x17, 0xc1, 0x40, 0x2b, 0x80, 0x09, 0x9d, 0xca, 0x5c, 0xbc, 0x20, 0x70, 0x75, 0xc0
};

static uint8_t IV[12] =
{
		0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
};

void msg_security_init()
{
	cmox_init_arg_t init_target = {CMOX_INIT_TARGET_H5, NULL};


	/* Initialize cryptographic library */
	if (cmox_initialize(&init_target) != CMOX_INIT_SUCCESS)
	{
		Error_Handler();
	}
}



int msg_encrypt(uint8_t *Plaintext,size_t p_sz,uint8_t *AddData,size_t AddData_sz,uint8_t *ctext,size_t *c_sz)
{
	cmox_cipher_retval_t retval;
	/* --------------------------------------------------------------------------
	 * SINGLE CALL USAGE
	 * --------------------------------------------------------------------------
	 */
	/* Compute directly the ciphertext and tag passing all the needed parameters */
	retval = cmox_aead_encrypt(CMOX_CHACHAPOLY_ENC_ALGO,               /* Use Chacha-Poly1305 algorithm */
			Plaintext, p_sz,           /* Plaintext to encrypt */
			TAG_SIZE,                   /* Authentication tag size */
			Key, sizeof(Key),                       /* ChaCha20-Poly1305 key to use */
			IV, sizeof(IV),                         /* Initialization vector */
			AddData, AddData_sz,               /* Additional authenticated data */
			ctext, c_sz);   /* Data buffer to receive generated ciphertext
                                                                        and authentication tag */
	/* Verify API returned value */
	if (retval != CMOX_CIPHER_SUCCESS)
	{
		Error_Handler();
		printf("Encrypt Error\n");
		return -1;
	}

	return 0;
}


int msg_decrypt(uint8_t *ctext,size_t c_sz,uint8_t *AddData,size_t AddData_sz,uint8_t *Plaintext,size_t *p_sz)
{
	cmox_cipher_retval_t retval;
	int ret = 0;

	/* Decrypt and verify directly ciphertext and tag passing all the needed parameters */
	retval = cmox_aead_decrypt(CMOX_CHACHAPOLY_DEC_ALGO,               /* Use Chacha20-Poly1305 algorithm */
			ctext, c_sz,     /* Ciphertext + tag to decrypt and verify */
			TAG_SIZE,                   /* Authentication tag size */
			Key, sizeof(Key),                       /* Chacha20-Poly1305 key to use */
			IV, sizeof(IV),                         /* Initialization vector */
			AddData, AddData_sz,               /* Additional authenticated data */
			Plaintext, p_sz);    /* Data buffer to receive generated plaintext */

	/* Verify API returned value */
	if (retval != CMOX_CIPHER_AUTH_SUCCESS)
	{
#ifdef NONCE_DEBUG
		print_nonce();
#endif
		// printf("Auth Failure Retval %d\n",(int)retval);
		//	     Error_Handler();
		ret = -1;
	}
	return ret;
}


int send_update_nonce(int nonce)
{
	int ret;
	int *ptr;
	uint8_t tmp_IV[12];

	memcpy((void *) tmp_IV, (const void *) IV, sizeof(tmp_IV));

	ptr = (int *) tmp_IV;

	*ptr = nonce;
	ret = can_tx_msg((uint8_t *) tmp_IV, 8);
	if(ret == 0) {
#ifdef NONCE_DEBUG
		printf("TX: Nonce Before: \n");
		print_nonce();
#endif
		memcpy((void *) IV,(const void *) tmp_IV,8);
#ifdef NONCE_DEBUG
		printf("TX: Nonce After: \n");
		print_nonce();
#endif
		printf("TX: Nonce Update Success\n");
	} else {
		printf("TX: Nonce Update Failed\n");
		return -1;
	}
	return 0;
}

void set_nonce(uint8_t *tmp_IV)
{
	memcpy((void *) IV, (void *) tmp_IV, 8);
}

#ifdef NONCE_DEBUG
void print_nonce()
{
	int i;
	for (i = 0 ; i < sizeof(IV); i++) {
		printf("%x ",IV[i]);
	}
	printf("\n");
}
#endif
