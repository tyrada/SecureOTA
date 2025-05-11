/* lms-sign.c */

/*
 * COMP 4900 E - RTOS PQC Project
 * This tool reads a firmware binary file, signs it and
 * a given private key, and appends the signature
 * to the firmware image for demo purposes.
 * 
 * In a real system, store in a secure, isolated environment, like 
 * a Hardware Security Module
 *
 * Usage:
 *   ./lms-sign <firmware.bin> <priv_key.bin> <output_file>
 *
 * Compile with:
 *   gcc lms-sign.c -loqs -o lms-sign
 *   or
 *   ex. gcc lms-sign.c -I./oqs-install/include -L./oqs-install/lib -loqs -lcrypto -o lms-sign
 *   depending on your dependency locations
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <oqs/oqs.h>

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <firmware.bin> <priv_key.bin> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *firmware_file = argv[1];
    const char *priv_key_file = argv[2];
    const char *output_file = argv[3];

    /* Open and read the firmware file */
    FILE *f_fw = fopen(firmware_file, "rb");
    if (!f_fw) {
        perror("Error opening firmware file");
        return EXIT_FAILURE;
    }
    fseek(f_fw, 0, SEEK_END);
    long fw_length = ftell(f_fw);
    fseek(f_fw, 0, SEEK_SET);
    uint8_t *firmware = malloc(fw_length);
    if (!firmware) {
        perror("Error allocating memory for firmware");
        fclose(f_fw);
        return EXIT_FAILURE;
    }
    if (fread(firmware, 1, fw_length, f_fw) != (size_t)fw_length) {
        perror("Error reading firmware file");
        free(firmware);
        fclose(f_fw);
        return EXIT_FAILURE;
    }
    fclose(f_fw);

    /* Open and read the private key */
    FILE *f_priv = fopen(priv_key_file, "rb");
    if (!f_priv) {
        perror("Error opening private key file");
        free(firmware);
        return EXIT_FAILURE;
    }
    fseek(f_priv, 0, SEEK_END);
    long priv_length = ftell(f_priv);
    fseek(f_priv, 0, SEEK_SET);
    uint8_t *privkey = malloc(priv_length);
    if (!privkey) {
        perror("Error allocating memory for private key");
        fclose(f_priv);
        free(firmware);
        return EXIT_FAILURE;
    }
    if (fread(privkey, 1, priv_length, f_priv) != (size_t)priv_length) {
        perror("Error reading private key");
        free(privkey);
        free(firmware);
        fclose(f_priv);
        return EXIT_FAILURE;
    }
    fclose(f_priv);

    /* Initialize OQS signature object 
       For demo purposes, MAYO-1 is used */
    OQS_SIG *mayo = OQS_SIG_new("MAYO-1");
    if (mayo == NULL) {
        fprintf(stderr, "Failed to initialize OQS\n");
        free(privkey);
        free(firmware);
        return EXIT_FAILURE;
    }

    /* Allocate a signature buffer */
    size_t sig_len = mayo->length_signature;
    uint8_t *signature = malloc(sig_len);
    if (!signature) {
        perror("Error allocating memory for signature");
        OQS_SIG_free(mayo);
        free(privkey);
        free(firmware);
        return EXIT_FAILURE;
    }

    /* Generate the signature.
       Note: OQS_SIG_sign() expects the following parameters:
         - the OQS_SIG object
         - a buffer to hold the signature
         - pointer to a size_t with the signature length, which will be updated
         - the message (firmware data)
         - the message length
         - the private key  */
    int ret = OQS_SIG_sign(mayo, signature, &sig_len,
                           firmware, fw_length, privkey);
    if (ret != OQS_SUCCESS) {
        fprintf(stderr, "Signature generation failed (ret=%d)\n", ret);
        free(signature);
        OQS_SIG_free(mayo);
        free(privkey);
        free(firmware);
        return EXIT_FAILURE;
    }
    printf("Signature generated successfully (length: %zu bytes)\n", sig_len);

    /* For demo purposes, the signature binary is appended to to the firmware 
    image. In a real system, store in a secure, isolated environment like 
    a Hardware Security Module */
    FILE *f_out = fopen(output_file, "wb");
    if (!f_out) {
        perror("Error opening output file");
        free(signature);
        OQS_SIG_free(mayo);
        free(privkey);
        free(firmware);
        return EXIT_FAILURE;
    }
    if (fwrite(firmware, 1, fw_length, f_out) != (size_t)fw_length) {
        perror("Error writing firmware to output file");
        fclose(f_out);
        free(signature);
        OQS_SIG_free(mayo);
        free(privkey);
        free(firmware);
        return EXIT_FAILURE;
    }
    if (fwrite(signature, 1, sig_len, f_out) != sig_len) {
        perror("Error writing signature to output file");
        fclose(f_out);
        free(signature);
        OQS_SIG_free(mayo);
        free(privkey);
        free(firmware);
        return EXIT_FAILURE;
    }
    fclose(f_out);
    printf("Signed firmware written to %s\n", output_file);

    free(signature);
    OQS_SIG_free(mayo);
    free(privkey);
    free(firmware);
    return EXIT_SUCCESS;
}
 