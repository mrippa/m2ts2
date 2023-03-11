#!../../bin/linux-x86_64/m2ts

#- You may have to change m2ts to something else
#- everywhere it appears in this file

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/m2ts.dbd"
m2ts_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadTemplate "db/user.substitutions"
dbLoadRecords "db/m2tsVersion.db", "user=mrippa"
dbLoadRecords "db/dbSubExample.db", "user=mrippa"
#dbLoadRecords("db/AP323.db","P=test:ap323data,D=Random,S=324235")
#dbLoadRecords("db/AP323.db","P=test:ap323dataA,D=RandomAsync,S=324235")
#dbLoadRecords("db/AP323.db","P=test:ap323dataI,D=RandomIntr,S=324235")
dbLoadRecords("db/AP323.db","P=test:ap323dataI2,D=RandomIntr,S=324235,SCAN=I/O Intr")
dbLoadRecords("db/AP323.db","P=test:ap323dataI3,D=RandomIntr,S=324235,SCAN=I/O Intr")

#- Set this to see messages from mySub
#var mySubDebug 1

#- Run this to trace the stages of iocInit
#traceIocInit

initM2TS

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncExample, "user=mrippa"
