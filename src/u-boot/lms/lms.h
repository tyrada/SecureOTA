/* u-boot/lib/lms/lms.h */
#ifndef __LMS_H__
#define __LMS_H__

#include <stddef.h>
#include <stdint.h>


struct lms_pubkey {
    const uint8_t *key_data;  /* Pointer to the public key bytes */
    size_t key_len;           /* Length (in bytes) of the public key */
};

/**
 * lms_verify_signature() - Verify an LMS signature on given data.
 * @data:      Pointer to the data (or its hash) over which the signature was computed.
 * @data_len:  Length of the data (or hash) in bytes.
 * @pubkey:    Pointer to an LMS public key structure.
 * @sig:       Pointer to the signature.
 * @sig_len:   Length of the signature in bytes.
 *
 * Returns 0 on success, or a negative error code on failure.
 */
int lms_verify_signature(const void *data, size_t data_len,
                         const struct lms_pubkey *pubkey,
                         const uint8_t *sig, size_t sig_len);

#endif /* __LMS_H__ */
