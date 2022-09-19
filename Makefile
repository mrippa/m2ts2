# Makefile at top of application tree
TOP = .
include $(TOP)/configure/CONFIG

# Directories to build, any order
DIRS += configure
DIRS += apCommon
DIRS += ap236Sup
#DIRS += ap323Sup
#DIRS += $(wildcard *Sup)
#DIRS += $(wildcard *App)
#DIRS += $(wildcard *Top)
#DIRS += $(wildcard iocBoot)

# The build order is controlled by these dependency rules:

# All dirs except configure depend on configure
$(foreach dir, $(filter-out configure, $(DIRS)), \
    $(eval $(dir)_DEPEND_DIRS += configure))

# All dirs except apCommonSup depend on apCommonSup
$(foreach dir, $(filter-out configure apCommon, $(DIRS)), \
    $(eval $(dir)_DEPEND_DIRS += apCommon))

# Any *App dirs depend on all *Sup dirs
$(foreach dir, $(filter %App, $(DIRS)), \
    $(eval $(dir)_DEPEND_DIRS += $(filter %Sup, $(DIRS))))

# Any *Top dirs depend on all *Sup and *App dirs
$(foreach dir, $(filter %Top, $(DIRS)), \
    $(eval $(dir)_DEPEND_DIRS += $(filter %Sup %App, $(DIRS))))

# iocBoot depends on all *App dirs
iocBoot_DEPEND_DIRS += $(filter %App,$(DIRS))

# Add any additional dependency rules here:

include $(TOP)/configure/RULES_TOP
