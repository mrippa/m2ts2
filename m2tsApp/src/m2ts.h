#ifndef M2TS_H
#define M2TS_H


#include <stdio.h>
#include <string.h>

#include "m2ts323.h"
//#include "m2ts471.h"
//#include "m2ts48x.h"
//#include "m2ts236.h"

/* Should move to M2TS Status codes or similar */
//#define ERROR -1

int M2TSInitAP323(int cardNumber );
//int InitAP236(void );
//int InitAP471(void );
//int InitAP48x(void );

/* 1. Mirror Control Task*/

/* 2. Vibration Control Task*/

/* 3. Communication (SynchroBus) Control Task*/

/* 3.1 AutoGuider Control Task*/

/* 3.2 Command Control Task*/

/* 4. Supervisor Control Task Group*/

/* 4.1 Safety Shutdown Task */

/* 4.2 Exception Management Task */

/* 4.3 Configuration Manager Task */

/* 4.3.1 Prime Filter Configurator Task */

/* 4.3.2 Trajectory Configurator Task */

/* 5. XY Positioner Control Task */

/* 6. Deployable Baffle Control Task */

/* 7. Periscope Baffle Control Task */

/* 8. Status Management Task */

#endif