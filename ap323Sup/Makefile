
TOP=..

include $(TOP)/configure/CONFIG

USR_INCLUDES += -Wall

LIBRARY_IOC += ap323

ap323_SRCS += drvr323.c
ap323_SRCS += cnfg323.c

ap323_LIBS += apCommon $(EPICS_BASE_IOC_LIBS)

#---------------------
# Generic EPICS build rules
#
include $(TOP)/configure/RULES
