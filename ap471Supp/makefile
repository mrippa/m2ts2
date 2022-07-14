
MODCFLAGS := -Wall

LIBS := -lpthread

a.out:		apcommon.o rpnt471.o rprt471.o rsts471.o\
			 wpnt471.o wprt471.o drvr471.o cnfg471.o
			cc -o a.out apcommon.o rpnt471.o rprt471.o rsts471.o\
			 wpnt471.o wprt471.o drvr471.o cnfg471.o $(LIBS)

apcommon.o:	../apcommon/apcommon.c ../apcommon/apcommon.h
			$(CC) $(MODCFLAGS) -c ../apcommon/apcommon.c

drvr471.o:		drvr471.c AP471.h
			$(CC) $(MODCFLAGS) -c drvr471.c

cnfg471.o:		cnfg471.c AP471.h
			$(CC) $(MODCFLAGS) -c cnfg471.c

rpnt471.o:		rpnt471.c AP471.h
			$(CC) $(MODCFLAGS) -c rpnt471.c

rprt471.o:		rprt471.c AP471.h
			$(CC) $(MODCFLAGS) -c rprt471.c

rsts471.o:		rsts471.c AP471.h
			$(CC) $(MODCFLAGS) -c rsts471.c

wpnt471.o:		wpnt471.c AP471.h
			$(CC) $(MODCFLAGS) -c wpnt471.c

wprt471.o:		wprt471.c AP471.h
			$(CC) $(MODCFLAGS) -c wprt471.c


clean:
			\rm -f *.o *~ a.out


