TOP=..

include $(TOP)/configure/CONFIG

USR_INCLUDES += -Wall
INC += AP471.h

LIBRARY_IOC += ap471

#ap471_SRCS += drvr471.c
ap471_SRCS += cnfg471.c
ap471_SRCS += rpnt471.c
ap471_SRCS += rprt471.c
ap471_SRCS += rsts471.c
ap471_SRCS += wpnt471.c
ap471_SRCS += wprt471.c

ap471_LIBS += apCommon $(EPICS_BASE_IOC_LIBS)

#---------------------
# Generic EPICS build rules
#
include $(TOP)/configure/RULES

