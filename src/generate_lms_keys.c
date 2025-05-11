/* generate_lms_keys.c */

/*
 * COMP 4900 E - RTOS PQC Project
 * Program to generate an LMS key pair using OQS.
 * Compile with:
 *   gcc generate_lms_keys.c -loqs -lcrypto -o generate_lms_keys
 *   or
 *   ex. gcc generate_lms_keys.c -I./oqs-install/include -L./oqs-install/lib -loqs -lcrypto -o generate_lms_keys
 *   depending on your dependency locations
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <oqs/oqs.h>
 
 int main(void) {
     /* For demo purposes, MAYO is used. Replace with LMS_SHA256_H10_W. */
     OQS_SIG *lms = OQS_SIG_new("MAYO-1");
     if (lms == NULL) {
         fprintf(stderr, "Failed to initialize OQS'\n");
         return EXIT_FAILURE;
     }
 
     uint8_t *public_key = malloc(lms->length_public_key);
     uint8_t *secret_key = malloc(lms->length_secret_key);
     if (public_key == NULL || secret_key == NULL) {
         fprintf(stderr, "Memory allocation failed\n");
         OQS_SIG_free(lms);
         return EXIT_FAILURE;
     }
 
     if (OQS_SIG_keypair(lms, public_key, secret_key) != OQS_SUCCESS) {
         fprintf(stderr, "Key generation failed\n");
         free(public_key);
         free(secret_key);
         OQS_SIG_free(lms);
         return EXIT_FAILURE;
     }
 
     printf("Public Key (%zu bytes):\n", lms->length_public_key);
     for (size_t i = 0; i < lms->length_public_key; i++) {
         printf("0x%02X, ", public_key[i]);
         if ((i + 1) % 8 == 0)
             printf("\n");
     }
     printf("\n\nSecret Key (%zu bytes):\n", lms->length_secret_key);
     for (size_t i = 0; i < lms->length_secret_key; i++) {
         printf("0x%02X, ", secret_key[i]);
         if ((i + 1) % 8 == 0)
             printf("\n");
     }
     printf("\n");
 
     /* Save the keys to files */
     FILE *pub_file = fopen("lms_pub_key.bin", "wb");
     FILE *priv_file = fopen("lms_priv_key.bin", "wb");
     if (!pub_file || !priv_file) {
         fprintf(stderr, "Error opening output file(s)\n");
     } else {
         fwrite(public_key, 1, lms->length_public_key, pub_file);
         fwrite(secret_key, 1, lms->length_secret_key, priv_file);
         fclose(pub_file);
         fclose(priv_file);
         printf("Keys saved to lms_pub_key.bin and lms_priv_key.bin\n");
     }
 
     free(public_key);
     free(secret_key);
     OQS_SIG_free(lms);
     return EXIT_SUCCESS;
 }
 