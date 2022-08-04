TOP=..

include $(TOP)/configure/CONFIG

USR_INCLUDES += -Wall

INC += apCommon.h

LIBRARY_IOC += apCommon

apCommon_SRCS += apCommon.c

apCommon_LIBS += $(EPICS_BASE_IOC_LIBS)

#---------------------
# Generic EPICS build rules
#
include $(TOP)/configure/RULES
