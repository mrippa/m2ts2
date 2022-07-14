
MODCFLAGS := -Wall

a.out:			apcommon.o rsts236.o rwcc236.o cd236.o wro236.o cnfg236.o drvr236.o

			cc -o a.out apcommon.o rsts236.o rwcc236.o cd236.o wro236.o \
			cnfg236.o drvr236.o -lm

apcommon.o:		../apcommon/apcommon.c ../apcommon/apcommon.h
			$(CC) $(MODCFLAGS) -c ../apcommon/apcommon.c

rsts236.o:		rsts236.c AP236.h
			$(CC) $(MODCFLAGS) -c rsts236.c

rwcc236.o:		rwcc236.c AP236.h
			$(CC) $(MODCFLAGS) -c rwcc236.c

cd236.o:		cd236.c AP236.h
			$(CC) $(MODCFLAGS) -c cd236.c

wro236.o:		wro236.c AP236.h
			$(CC) $(MODCFLAGS) -c wro236.c

cnfg236.o:		cnfg236.c AP236.h
			$(CC) $(MODCFLAGS) -c cnfg236.c

drvr236.o:		drvr236.c AP236.h
			$(CC) $(MODCFLAGS) -c drvr236.c

clean:
			\rm -f *.o *~ a.out

