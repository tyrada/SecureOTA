/* u-boot/lib/lms/lms-sign.c */
/* SPDX-License-Identifier: GPL-2.0+ */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <oqs/oqs.h>
#include <image.h>
#include "../lib/lms/lms.h"

/**
 * lms_sign_data() - Create an LMS signature using OQS.
 * @data:        Pointer to the data (or its hash) to be signed.
 * @data_len:    Length of the data in bytes.
 * @privkey_file:Path to a file containing the LMS private key.
 * @sig:         Pointer to the pointer that will be allocated with the signature.
 * @sig_len:     Pointer to store the signature length.
 *
 * Returns 0 on success or a negative error code.
 */
int lms_sign_data(const void *data, size_t data_len,
                  const char *privkey_file,
                  uint8_t **sig, size_t *sig_len)
{
    OQS_SIG *lms = OQS_SIG_new("LMS");
    if (lms == NULL) {
        fprintf(stderr, "LMS-Sign: Failed to initialize OQS LMS\n");
        return -1;
    }

    /* Load the private key from file */
    FILE *f = fopen(privkey_file, "rb");
    if (f == NULL) {
        fprintf(stderr, "LMS-Sign: Cannot open private key file %s\n", privkey_file);
        OQS_SIG_free(lms);
        return -ENOENT;
    }
    fseek(f, 0, SEEK_END);
    long keylen = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *privkey = malloc(keylen);
    if (privkey == NULL) {
        fclose(f);
        OQS_SIG_free(lms);
        return -ENOMEM;
    }
    if (fread(privkey, 1, keylen, f) != (size_t)keylen) {
        fprintf(stderr, "LMS-Sign: Failed to read private key\n");
        fclose(f);
        free(privkey);
        OQS_SIG_free(lms);
        return -EIO;
    }
    fclose(f);

    /* Allocate buffer for the signature */
    size_t signature_len = lms->length_signature;
    uint8_t *signature = malloc(signature_len);
    if (signature == NULL) {
        free(privkey);
        OQS_SIG_free(lms);
        return -ENOMEM;
    }

    int ret = OQS_SIG_sign(lms, signature, &signature_len,
                           data, data_len, privkey);
    free(privkey);
    OQS_SIG_free(lms);

    if (ret != OQS_SUCCESS) {
        fprintf(stderr, "LMS-Sign: Signature generation failed\n");
        free(signature);
        return -1;
    }

    *sig = signature;
    *sig_len = signature_len;
    return 0;
}
