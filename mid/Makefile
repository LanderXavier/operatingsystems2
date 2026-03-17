obj-m += sysinfo_chardev.o

KDIR ?= /home/lander/Lander/so2/mid/rpi-linux

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
