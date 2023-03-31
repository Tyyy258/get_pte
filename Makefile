obj-m := get_pte.o
EXTRA_CFLAGS = -O2
KERNEL_DIR = /home/ubuntu/op-tee/linux
CROSS_COMPILE = /home/ubuntu/op-tee/toolchains/aarch64/bin/aarch64-linux-gnu-
all:
	make -C $(KERNEL_DIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) ARCH=arm64 CC=$(CROSS_COMPILE)-gcc clean
	