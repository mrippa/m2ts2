
TARGET := ap236

NAMES := dev236
MODULE := $(TARGET).ko
OBJS  := $(addsuffix .o, $(NAMES))

ifneq ($(KERNELRELEASE),)
obj-m := $(TARGET).o

$(TARGET)-objs := $(OBJS)

else
KDIR := /lib/modules/$(shell uname -r)/build # use this line for Fedora/CentOS 
#KDIR := /usr/src/linux-headers-$(shell uname -r) #use this line for Debian/Ubuntu
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
endif

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean


