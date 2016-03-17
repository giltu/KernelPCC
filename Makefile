obj-m += tcp_TA.o
KVERSION := /usr/src/linux-headers-$(shell uname -r)	
PWD := $(shell pwd)
default:
	make -C $(KVERSION) SUBDIRS=$(PWD) modules
clean:
	make -C $(KVERSION) M=$(PWD) clean
