TOP=..

include $(TOP)/configure/CONFIG

USR_INCLUDES += -Wall
INC += AP323.h

LIBRARY_IOC += ap323

#ap323_SRCS += drvr323.c
ap323_SRCS += cnfg323.c
ap323_SRCS += rsts323.c
ap323_SRCS += ain323.c
ap323_SRCS += mccd323.c
ap323_SRCS += rwcc323.c

ap323_LIBS += apCommon $(EPICS_BASE_IOC_LIBS)

#---------------------
# Generic EPICS build rules
#
include $(TOP)/configure/RULES
