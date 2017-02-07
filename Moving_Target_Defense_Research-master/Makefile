obj-m := MTD_mod.o

KERNEL_DIR = /lib/modules/3.13.0-44-generic/build
	PWD := $(shell pwd)
all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order *~
