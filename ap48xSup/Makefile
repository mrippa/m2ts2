TOP=..

include $(TOP)/configure/CONFIG

USR_INCLUDES += -Wall
INC += AP48X.h

LIBRARY_IOC += ap48x

ap48x_SRCS += cnfgct.c
ap48x_SRCS += rmid48x.c
ap48x_SRCS += rpntdio.c
ap48x_SRCS += rprtdio.c
ap48x_SRCS += wpntdio.c
ap48x_SRCS += wprtdio.c

ap48x_LIBS += apCommon $(EPICS_BASE_IOC_LIBS)

#---------------------
# Generic EPICS build rules
#
include $(TOP)/configure/RULES

