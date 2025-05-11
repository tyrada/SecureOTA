/* u-boot/lib/lms/lms.c */
/* SPDX-License-Identifier: GPL-2.0+ */

#include <errno.h>
#include <malloc.h>
#include <oqs/oqs.h>
#include "../lib/lms/lms.h"

/**
 * lms_verify_signature() - Basic LMS signature verification using OQS
 *
 * This function creates an OQS_SIG object for LMS, then calls the OQS
 * verification function on the provided data (which may be the complete message
 * or its hash, as agreed by the signing process).
 */
int lms_verify_signature(const void *data, size_t data_len,
                         const struct lms_pubkey *pubkey,
                         const uint8_t *sig, size_t sig_len)
{
    int ret;

    printf("LMS: Enter lms_verify_signature()\n");
    printf("LMS: data=%p, data_len=%zu, sig_len=%zu\n", data, data_len, sig_len);

    if (!pubkey || !pubkey->key_data || pubkey->key_len == 0) {
        printf("LMS: Invalid public key provided\n");
        return -EINVAL;
    }

    /* Create a new LMS signature object via OQS. 
       For demo purposes, MAYO-1 is put in place */
    OQS_SIG *lms = OQS_SIG_new("MAYO-1");
    if (lms == NULL) {
        printf("LMS: Failed to initialize OQS LMS\n");
        return -ENOMEM;
    }

    /* OQS_SIG_verify returns OQS_SUCCESS (0) for a valid signature */
    ret = OQS_SIG_verify(lms, sig, sig_len, data, data_len, pubkey->key_data);

    OQS_SIG_free(lms);

    if (ret == OQS_SUCCESS) {
        printf("LMS: Signature verification succeeded\n");
        return 0;
    } else {
        printf("LMS: Signature verification failed\n");
        return -EINVAL;
    }
}
