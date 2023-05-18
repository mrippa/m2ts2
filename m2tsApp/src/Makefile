TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS BELOW HERE

# Use typed rset structure (see 3.16.1 release notes)
USR_CPPFLAGS += -DUSE_TYPED_RSET
#USR_CPPFLAGS += -DMATT_TEST

INC += m2ts.h
INC += m2ts323.h
INC += m2ts236.h
INC += m2ts471.h
INC += m2ts48x.h

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord

# Install xxxSupport.dbd into <top>/dbd
#DBD += xxxSupport.dbd

# Build an IOC support library
LIBRARY_IOC += m2tsSupport

# Compile and add code to the support library

# Link locally-provided code into the support library,
# rather than directly into the IOC application, that
# causes problems on Windows DLL builds
#m2tsSupport_SRCS += dbSubExample.c
m2tsSupport_SRCS += devm2tsVersion.c
#m2tsSupport_SRCS += m2tsHello.c
#m2tsSupport_SRCS += devAP323data.c
#m2tsSupport_SRCS += devAP323async.c
#m2tsSupport_SRCS += devAP323intr.c

m2tsSupport_SRCS += m2tsInit.c
m2tsSupport_SRCS += InitAP323.c
# m2tsSupport_SRCS += InitAP236.c
# m2tsSupport_SRCS += InitAP471.c
# m2tsSupport_SRCS += InitAP48x.c
m2tsSupport_SRCS += initTrace.c

# m2tsSupport_SRCS += ConfigAP471.c
# m2tsSupport_SRCS += ConfigAP48x.c
# m2tsSupport_SRCS += ConfigAP236.c
# m2tsSupport_SRCS += ConfigAP323.c

m2tsSupport_LIBS += ap323 
#m2tsSupport_LIBS += ap236 
#m2tsSupport_LIBS += ap471 
#m2tsSupport_LIBS += ap48x 
m2tsSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

# Auto-generate a header file containing a version string.
# Version comes from the VCS if available, else date+time.
GENVERSION = m2tsVersion.h
# Macro name
GENVERSIONMACRO = m2tsVERSION

# Build the IOC application
PROD_IOC = m2ts

# m2ts.dbd will be created and installed
DBD += m2ts.dbd

# m2ts.dbd will include these files:
m2ts_DBD += base.dbd
#m2ts_DBD += xxxSupport.dbd
#m2ts_DBD += dbSubExample.dbd
m2ts_DBD += devm2tsVersion.dbd
#m2ts_DBD += m2tsHello.dbd
m2ts_DBD += m2tsInit.dbd
m2ts_DBD += initTrace.dbd
#m2ts_DBD += AP323dev.dbd

#m2ts_SRCS += devMyThread.c

# m2ts_registerRecordDeviceDriver.cpp derives from m2ts.dbd
m2ts_SRCS += m2ts_registerRecordDeviceDriver.cpp

# Build the main IOC entry point where needed
m2ts_SRCS_DEFAULT += m2tsMain.cpp
m2ts_SRCS_vxWorks += -nil-

# Link in the code from our support library
m2ts_LIBS += m2tsSupport

# Finally link IOC to the EPICS Base libraries
m2ts_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD EXTRA GNUMAKE RULES BELOW HERE

# Explicit dependency needed for generated header file
devm2tsVersion$(DEP): $(COMMON_DIR)/$(GENVERSION)
