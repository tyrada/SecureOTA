# u-boot/lib/lms/Makefile
obj-$(CONFIG_LMS) += lms.o lms-verify.o

EXTRA_CFLAGS += -I$(srctree)/../oqs-install/include
EXTRA_LDFLAGS += -loqs -lcrypto
