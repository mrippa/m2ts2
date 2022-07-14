
MODCFLAGS := -Wall -g

a.out:		apcommon.o cnfg323.o rsts323.o ain323.o mccd323.o rwcc323.o drvr323.o

			cc -o a.out apcommon.o  cnfg323.o rsts323.o \
			ain323.o mccd323.o rwcc323.o drvr323.o -lm

apcommon.o:	../apcommon/apcommon.c ../apcommon/apcommon.h
			$(CC) $(MODCFLAGS) -c ../apcommon/apcommon.c

drvr323.o:		drvr323.c AP323.h
			$(CC) $(MODCFLAGS) -c drvr323.c

cnfg323.o:		cnfg323.c AP323.h
			$(CC) $(MODCFLAGS) -c cnfg323.c

rsts323.o:		rsts323.c AP323.h
			$(CC) $(MODCFLAGS) -c rsts323.c

mccd323.o:		mccd323.c AP323.h
			$(CC) $(MODCFLAGS) -c mccd323.c

ain323.o:		ain323.c AP323.h
			$(CC) $(MODCFLAGS) -c ain323.c

rwcc323.o:		rwcc323.c AP323.h
			$(CC) $(MODCFLAGS) -c rwcc323.c
clean:
			\rm -f *.o *~ a.out

