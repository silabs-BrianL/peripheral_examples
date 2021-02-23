/***************************************************************************//**
 * @file main_s0.c
 * @brief This project uses the CFB (Cipher Feedback) mode of AES encryption
 * to encrypt the user's input data and then decrypt it.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Evaluation Quality
 * This code has been minimally tested to ensure that it builds and is suitable 
 * as a demonstration for evaluation purposes only. This code will be maintained
 * at the sole discretion of Silicon Labs.
 ******************************************************************************/

#include "em_device.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_aes.h"

// Note: change this to change the number of bytes to encrypt
//       (must be a multiple of 16)
#define DATA_SIZE 64

// Note: change this to change the data to encrypt
const static uint8_t originalData[DATA_SIZE] = {
  0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
  0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A,
  0xAE, 0x2D, 0x8A, 0x57, 0x1E, 0x03, 0xAC, 0x9C,
  0x9E, 0xB7, 0x6F, 0xAC, 0x45, 0xAF, 0x8E, 0x51,
  0x30, 0xC8, 0x1C, 0x46, 0xA3, 0x5C, 0xE4, 0x11,
  0xE5, 0xFB, 0xC1, 0x19, 0x1A, 0x0A, 0x52, 0xEF,
  0xF6, 0x9F, 0x24, 0x45, 0xDF, 0x4F, 0x9B, 0x17,
  0xAD, 0x2B, 0x41, 0x7B, 0xE6, 0x6C, 0x37, 0x10,
};

// A buffer to hold the encrypted data
static uint8_t encryptedData[DATA_SIZE];

// A buffer to hold the decrypted data
static uint8_t decryptedData[DATA_SIZE];

// Number of bytes to make up a 128 bit key (16 * 8 = 128)
#define KEY_SIZE 16

// Note: change this to change the key
// Note: must be 128 bits
// Note: the same key is used for encryption and decryption in CFB mode
const static uint8_t key[KEY_SIZE] = {
  0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
  0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};

// Note: change this to change the initialization vector used for Cipher Feedback Mode
// Note: must be 128 bits
const static uint8_t initVector[KEY_SIZE] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

// A flag indicating whether the encryption/decryption process succeeded
// Note: This is only volatile to ensure that it doesn't get optimized out by
// the compiler before the user checks its value. It's not actually necessary.
static volatile bool isError;

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Enable AES clock
  CMU_ClockEnable(cmuClock_AES, true);

  // Encrypt data using AES-128 CFB
  AES_CFB128(encryptedData, // Pointer to buffer where encrypted/decrypted data will be put
             originalData,  // Pointer to buffer holding data to encrypt/decrypt
             DATA_SIZE,     // Number of bytes to encrypt (must be a multiple of 16)
             key,           // A 128 bit encryption/decryption key
             initVector,    // A 128 bit initialization vector
             true);         // Use encryption mode

  // Decrypt data using AES-128 CFB
  AES_CFB128(decryptedData, // Pointer to buffer where encrypted/decrypted data will be put
             encryptedData, // Pointer to buffer holding data to encrypt/decrypt
             DATA_SIZE,     // Number of bytes to encrypt (must be a multiple of 16)
             key,           // A 128 bit encryption/decryption key
             initVector,    // A 128 bit initialization vector
             false);        // Use decryption mode

  // Check whether decrypted result is identical to the original data
  isError = false;
  for (uint32_t i = 0; i < DATA_SIZE; i++) {
    if (decryptedData[i] != originalData[i]) {
      isError = true;
    }
  }

  // Pause the debugger here to check if the isError variable is true/false
  while (1) {
    EMU_EnterEM1();
  }
}

