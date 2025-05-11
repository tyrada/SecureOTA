/* u-boot/lib/lms/lms-verify.c */
// SPDX-License-Identifier: GPL-2.0+
#include <image.h>
#include "../lib/lms/lms.h"

/*
 * This is a lightweight wrapper that conforms to the format expected by the
 * U_BOOT_CRYPTO_ALGO() registration.
 *
 * hash:      Pointer to the hash of the FIT image or configuration.
 * hash_len:  Length of the computed hash.
 * sig:       Pointer to the LMS signature data.
 * sig_len:   Length of the signature data.
 * pub_key:   Pointer to the public key data (as stored in the control FDT).
 * pub_key_len: Length of the public key data.
 */
static int lms_verify(const void *hash, size_t hash_len,
                      const void *sig, size_t sig_len,
                      const void *pub_key, size_t pub_key_len)
{
    printf("LMS: Starting verification: hash_len=%zu, sig_len=%zu, pub_key_len=%zu\n",
          hash_len, sig_len, pub_key_len);
    return lms_verify_signature(hash, hash_len, (const struct lms_pubkey *)pub_key,
                                sig, sig_len);
}

U_BOOT_CRYPTO_ALGO(lms) = {
    .name   = "lms",
    .verify = lms_verify,
};
