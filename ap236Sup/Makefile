TOP=..

include $(TOP)/configure/CONFIG

USR_INCLUDES += -Wall
INC += AP236.h

LIBRARY_IOC += ap236

#ap236_SRCS += drvr236.c
ap236_SRCS += cnfg236.c
ap236_SRCS += wro236.c
ap236_SRCS += cd236.c
ap236_SRCS += rwcc236.c
ap236_SRCS += rsts236.c

ap236_LIBS += apCommon $(EPICS_BASE_IOC_LIBS)

#---------------------
# Generic EPICS build rules
#
include $(TOP)/configure/RULES

