#CROSS_COMPILER=/home/nlbutts/projects/buildroot/output/host/usr/bin/arm-linux-
CROSS_COMPILER=/opt/crystal/gateway/R2.2.0.136/platform/crystal/release/toolchain/usr/bin/arm-as-linux-gnueabi-
MM_DIR=.

.PHONY: all
all:
	#make CROSS_COMPILE=$(CROSS_COMPILER) ARCH=arm -C kernel M=`pwd`/$(MM_DIR)
	$(CROSS_COMPILER)gcc -g -funsafe-math-optimizations -mcpu=cortex-a9 -mfpu=neon-fp16 -std=gnu11 -o $(MM_DIR)/us_mem_test $(MM_DIR)/mem_test_user.c $(MM_DIR)/memcpyf.S
	$(CROSS_COMPILER)objdump -S us_mem_test > dis.txt

.PHONY: dump
dump:
	$(CROSS_COMPILER)objdump -S $(MM_DIR)/mem_test_user|less

.PHONY: install
install: all
	#scp $(MM_DIR)/mem_test.ko default@cryhw:/home/default
	scp $(MM_DIR)/us_mem_test default@gw:/home/default

.PHONY: kernel
kernel:
	make CROSS_COMPILE=$(CROSS_COMPILER) ARCH=arm -C kernel -j4

.PHONY: menuconfig
menuconfig:
	make CROSS_COMPILE=$(CROSS_COMPILER) ARCH=arm -C kernel menuconfig

.PHONY: install_kernel
install_kernel:
	scp kernel/arch/arm/boot/uImage default@gw:/home/default

.PHONY: clean
clean:
	rm -f $(MM_DIR)/*.o
	rm -f $(MM_DIR)/*.cmd
	rm -f $(MM_DIR)/*.ko
	rm -f $(MM_DIR)/.mem*
	rm -f $(MM_DIR)/.built*
	rm -f $(MM_DIR)/modules.order
	rm -f $(MM_DIR)/Module*
	rm -f $(MM_DIR)/mem_test.mod.c
	rm -f $(MM_DIR)/mem_test_user
