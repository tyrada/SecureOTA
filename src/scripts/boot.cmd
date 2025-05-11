# If no boot_img is specified, default to ifs-rpi4.bin
if test -z "${boot_img}"; then
    setenv boot_img ifs-rpi4.bin
    setenv filesize 0x200000
fi

echo "DEBUG: boot_img is set to ${boot_img}"
echo "DEBUG: Attempting to load ${boot_img} from MMC..."

if fatload mmc 0 0x80000 ${boot_img}; then
    echo "DEBUG: Successfully loaded ${boot_img}"
    echo "Detected signature algorithm 'lms' in FIT configuration"

    secureboot 0x80000 ${filesize}

    if test $? -ne 0; then
        echo "Secureboot verification FAILED; aborting boot."
        reset
    fi

    echo "Secureboot verification PASSED; booting firmware..."
    echo "DEBUG: Pausing for 5 seconds..."
    sleep 5

    go 0x80000 
else
    echo "DEBUG: Failed to load ${boot_img}"
    if test "${boot_img}" = "ifs-rpi4.bin"; then
        echo "DEBUG: Primary image failed, trying backup..."
        if fatload mmc 0 0x80000 ifs-rpi4.bin.bak; then
            echo "DEBUG: Successfully loaded backup image ifs-rpi4.bin.bak"
            echo "Detected signature algorithm 'lms' in FIT configuration"

            secureboot 0x80000 ${filesize}

            if test $? -ne 0; then
                echo "Secureboot verification FAILED; aborting boot."
                reset
            fi

            echo "Secureboot verification PASSED; booting firmware..."
            echo "DEBUG: Pausing for 5 seconds..."
            sleep 5

            go 0x80000 
        else
            echo "DEBUG: No valid image found (backup failed)"
        fi
    else
        echo "DEBUG: boot_img is not the default and load failed: ${boot_img}"
    fi
fi
