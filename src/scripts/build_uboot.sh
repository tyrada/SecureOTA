#!/bin/bash
# Exit immediately on any error
set -e

echo "Starting U-Boot build and boot folder preparation for Raspberry Pi 4..."

sudo apt update
sudo apt install -y git dosfstools gcc-aarch64-linux-gnu make flex bison bc libssl-dev libgnutls28-dev 

# Clone the U-Boot repository if it doesn't already exist.
if [ ! -d "u-boot" ]; then
    echo "Cloning U-Boot repository..."
    git clone --depth=1 --branch=v2025.01 https://source.denx.de/u-boot/u-boot.git
fi

# Build U-Boot for Raspberry Pi 4.
echo "Building U-Boot for Raspberry Pi 4..."
cd u-boot
# Use the default configuration for Raspberry Pi 4.
CROSS_COMPILE=aarch64-linux-gnu- make rpi_4_defconfig
# scripts/kconfig/merge_config.sh .config extra.config
CROSS_COMPILE=aarch64-linux-gnu- make -j$(nproc)
cd ..

# Prepare the boot folder.
echo "Preparing boot folder..."
rm -rf boot
mkdir boot
# Copy the compiled U-Boot binary.
cp u-boot/u-boot.bin boot/

# Clone the Raspberry Pi firmware repository if it doesn't exist.
if [ ! -d "firmware" ]; then
    echo "Cloning Raspberry Pi firmware repository..."
    git clone --depth=1 --branch=1.20230106 https://github.com/raspberrypi/firmware.git
fi

# Copy required boot files from the firmware repository.
cp firmware/boot/{bcm2711-rpi-4-b.dtb,fixup4.dat,fixup4cd.dat,fixup4db.dat,fixup4x.dat,start4.elf,start4cd.elf,start4db.elf,start4x.elf} boot/

# Create the config.txt file.
cat << EOF > boot/config.txt
arm_64bit=1
cmdline=startup.txt
device_tree=bcm2711-rpi-4-b.dtb
# enable_jtag_gpio=1
enable_uart=1
force_turbo=1
gpu_mem=16
max_framebuffers=2
kernel=u-boot.bin
EOF

echo "Boot folder created successfully. Its contents are:"
ls -l boot/

echo "Now copy the contents of the boot folder to your SD card manually."

