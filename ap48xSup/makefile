
MODCFLAGS := -Wall

a.out:		apcommon.o  rmid48x.o rpntdio.o\
			rprtdio.o wpntdio.o wprtdio.o cnfgct.o drvr48x.o

			cc -o a.out apcommon.o  rmid48x.o\
			rpntdio.o rprtdio.o wpntdio.o wprtdio.o cnfgct.o drvr48x.o

apcommon.o:	../apcommon/apcommon.c ../apcommon/apcommon.h
			$(CC) $(MODCFLAGS) -c ../apcommon/apcommon.c

drvr48x.o:		drvr48x.c AP48X.h
			$(CC) $(MODCFLAGS) -c drvr48x.c

cnfgct.o:		cnfgct.c AP48X.h
			$(CC) $(MODCFLAGS) -c cnfgct.c

rmid48x.o:		rmid48x.c AP48X.h
			$(CC) $(MODCFLAGS) -c rmid48x.c

rpntdio.o:		rpntdio.c AP48X.h
			$(CC) $(MODCFLAGS) -c rpntdio.c

rprtdio.o:		rprtdio.c AP48X.h
			$(CC) $(MODCFLAGS) -c rprtdio.c

wpntdio.o:		wpntdio.c AP48X.h
			$(CC) $(MODCFLAGS) -c wpntdio.c

wprtdio.o:		wprtdio.c AP48X.h
			$(CC) $(MODCFLAGS) -c wprtdio.c


clean:
			\rm -f *.o *~ a.out

