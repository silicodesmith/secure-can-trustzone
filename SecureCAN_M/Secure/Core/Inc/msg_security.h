/*
 * msg_security.h
 *
 *  Created on: Apr 20, 2025
 *      Author: Naveen S
 */

#ifndef INC_MSG_SECURITY_H_
#define INC_MSG_SECURITY_H_

//#define NONCE_DEBUG


// Function declarations

/**
 * @brief Initializes the cryptographic library.
 * @return void
 */
void msg_security_init(void);

/**
 * @brief Encrypts a message using ChaCha20-Poly1305.
 * @param Plaintext Pointer to the plaintext data to encrypt.
 * @param p_sz Size of the plaintext data.
 * @param AddData Pointer to additional authenticated data (AAD).
 * @param AddData_sz Size of the AAD.
 * @param ctext Pointer to the buffer to store the ciphertext and tag.
 * @param c_sz Pointer to the size of the ciphertext buffer.
 * @return 0 on success, -1 on failure.
 */
int msg_encrypt(uint8_t *Plaintext, size_t p_sz, uint8_t *AddData, size_t AddData_sz, uint8_t *ctext, size_t *c_sz);

/**
 * @brief Decrypts a message using ChaCha20-Poly1305.
 * @param ctext Pointer to the ciphertext data to decrypt.
 * @param c_sz Size of the ciphertext data.
 * @param AddData Pointer to additional authenticated data (AAD).
 * @param AddData_sz Size of the AAD.
 * @param Plaintext Pointer to the buffer to store the decrypted plaintext.
 * @param p_sz Pointer to the size of the plaintext buffer.
 * @return 0 on success, -1 on authentication failure.
 */
int msg_decrypt(uint8_t *ctext, size_t c_sz, uint8_t *AddData, size_t AddData_sz, uint8_t *Plaintext, size_t *p_sz);

/**
 * @brief Updates the nonce and sends it via CAN.
 * @param nonce The new nonce value to update.
 * @return 0 on success, -1 on failure.
 */
int send_update_nonce(int nonce);

/**
 * @brief Sets the nonce directly.
 * @param tmp_IV Pointer to the new IV value.
 * @return void
 */
void set_nonce(uint8_t *tmp_IV);

#ifdef NONCE_DEBUG
/**
 * @brief Prints the current nonce (for debugging purposes).
 * @return void
 */
void print_nonce(void);
#endif

#endif /* INC_MSG_SECURITY_H_ */
