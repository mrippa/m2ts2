
#
# Select one of the TARGET boards (by removing the #) from the following:
#

TARGET := ap482
#TARGET := ap483
#TARGET := ap484

NAMES := dev48x
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
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
endif

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean



