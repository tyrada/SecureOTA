# RPI4-OTA-with-PQC

This project demonstrates how to perform secure **Over-The-Air (OTA)** updates on a **Raspberry Pi 4** running **QNX**, using **Post-Quantum Cryptography (PQC)** for firmware signing and verification. It integrates a custom fork of U-Boot that verifies firmware images (e.g., `ifs-rpi4.bin`) using a PQC signing algorithm (in our demo, MAYO‑1) before booting QNX.

> **Video Tutorial:**  
> [Here's the link to the video tutorial](https://www.youtube.com/watch?v=S0OHd5dIjJk)  
> UPDATE: 04/14. Due to a YouTube account issue, the video is accessible [here](https://drive.google.com/file/d/1rSjWTyqoGvMyCYX5QNWVx4E82N49-IU2/view?usp=sharing) for the time being.  
> Skip to 24:39 for the demo.

---

## Overview

- **Hardware**: Raspberry Pi 4
- **Operating System**: QNX RTOS (target version 7.1/8.0)
- **Bootloader**: U-Boot (with added PQC support via a custom fork)
- **Signature Algorithm**: LMS

**Key Features:**
1. **Secure Boot**: U-Boot is extended to verify firmware signatures using LMS before booting QNX.
2. **OTA Updates**: The project includes scripts and tools for downloading, signing, and deploying firmware images.
3. **Academic Demo of PQC**: Illustrates how to integrate a post‑quantum signature algorithm into a secure OTA update process.

---

## Repository Structure
```
.
└── src
    ├── generate_lms_keys.c
    ├── lms-sign.c
    ├── scrips
    │   ├── boot.cmd
    │   ├── build_uboot.sh
    │   └── rpi4.build
    └── u-boot
        ├── lms
        │   ├── Kconfig
        │   ├── Makefile
        │   ├── lms-sign.c
        │   ├── lms-verify.c
        │   ├── lms.c
        │   └── lms.h
        └── secureboot.c
```
### `src/`
Holds source code and scripts.

- **`generate_lms_keys.c`**  
  Generate your key pairs (public and private keys).

- **`lms-sign.c`**  
  The signing tool that reads a firmware image, uses the private key (from `priv_key.bin`) to sign it, and appends the signature to produce a signed firmware image.

- **`scrips/`**  
  Contains:
  - **`boot.cmd`**: A U-Boot boot script to load and verify the firmware image.
  - **`build_uboot.sh`**: A shell script that clones the U-Boot fork, builds U-Boot for the Raspberry Pi 4, and prepares the boot folder.
  - **`rpi4.build`**: Example build configuration/instructions for the Raspberry Pi 4 environment.

- **`u-boot/`**  
  Contains the modifications to the U-Boot fork to support PQC verification:
  - **`lms/`**: This directory holds:
    - **`Kconfig`**: Adds the CONFIG_LMS option.
    - **`Makefile`**: Ensures the new modules are built.
    - **`lms-sign.c`**: For U-Boot or sandbox signing
    - **`lms-verify.c`**: Registers the algorithm and provides the verification adapter.
    - **`lms.c`** and **`lms.h`**: Core cryptographic logic using OQS.
  - **`cmd/`**: This directory holds:
    - **`secureboot.c`**: Register the secureboot command within U-Boot

---
## U-Boot Fork

This repository only contains the custom modifications for PQC support (the `lms` folder and related files). The complete U-Boot source—with all the modifications for secure OTA booting is maintained in our custom fork. You can view the full U-Boot fork here:

[Custom U-Boot Fork Repository](https://github.com/JerickLiu/u-boot)

---

## Quickstart Guide

1. **Generate Keys**  
   Compile and run the key generation program to produce your MAYO‑1 key pair:
   ```bash
   gcc generate_lms_keys.c -loqs -o generate_lms_keys
   ./generate_lms_keys
   ```
   This will produce pub_key.bin and priv_key.bin.

   Public Key:
    Hardcode the key bytes from pub_key.bin into the U-Boot source (for example, in your custom command file).

   Private Key:
    Keep it secure and use it only during the signing process.

2. **Build U-Boot**
    Run the build script to clone, build, and prepare U-Boot:
    ```bash
    ./scrips/build_uboot.sh
    ```
    This script will:
    - Clone the U-Boot fork,
    - Build U-Boot for the Raspberry Pi 4 with the added PQC support,
    - Prepare the boot folder for deployment.

3. **Sign the Firmware**
    Use the signing tool to generate a signature and append it to your firmware image:
    ```bash
    gcc lms-sign.c -loqs -o lms-sign
    ./lms-sign ifs-rpi4.bin priv_key.bin ifs-rpi4.bin.signed
    ```
    This command:
    - Reads the original ifs-rpi4.bin firmware image,
    - Signs it with the private key from priv_key.bin,
    - Produces a new image (ifs-rpi4.bin.signed) that contains the payload followed by the fixed-length signature.

4. **Deploy & Boot**
    - Copy ifs-rpi4.bin.signed into the boot folder on your SD card (rename it to ifs-rpi4.bin or change the boot_img env var within U-Boot).
    - Insert the SD card into your Raspberry Pi 4 and power it on.
    - U-Boot will load the firmware, extract and verify the appended signature using the hardcoded public key, and then boot QNX if the signature is valid.
    
    OTA Update Flow (High-Level)
    - Firmware Update Download:
    - An OTA process downloads the new signed firmware image (ifs-rpi4.bin.signed) from a server.
    - Firmware Replacement:
    - The new signed firmware replaces the existing firmware in the boot partition.

    Boot Verification:
    - Upon reboot, U-Boot:
    - Loads the firmware image,
    - Extracts the appended signature,
    - Computes the SHA256 hash over the payload,
    - Verifies the signature using the embedded public key.

    Boot Outcome:
    - If verification succeeds, QNX boots normally.
    - If verification fails, U-Boot aborts the boot process (or falls back to a backup image).



