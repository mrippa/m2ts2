#!../../bin/linux-x86_64/m2ts

#- You may have to change m2ts to something else
#- everywhere it appears in this file

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/m2ts.dbd"
m2ts_registerRecordDeviceDriver pdbbase

## Load record instances
#dbLoadTemplate "db/user.substitutions"
#dbLoadRecords "db/m2tsVersion.db", "user=mrippa"
dbLoadRecords "db/AP323.db", "user=mrippa"

#- Set this to see messages from mySub
#var mySubDebug 1

#- Run this to trace the stages of iocInit
traceIocInit

M2TSStartup

cd "${TOP}/iocBoot/${IOC}"
#iocInit

## Start any sequence programs
#seq sncExample, "user=mrippa"
