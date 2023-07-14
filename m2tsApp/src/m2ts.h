#ifndef M2TS_H
#define M2TS_H

#include <stdio.h>
#include <string.h>

/* AP323 */
int M2TSInitAP323(int cardNumber );
int M2TSConfigAP323(int cardNumber );

/* AP471 */
int M2TSInitAP471( int cardNumber);
int M2TSConfigAP471( int cardNumber);

/* AP482 */
int M2TSInitAP482(int cardNumber);
int M2TSConfigAP482(int cardNumber);

/* AP236 */
int M2TSInitAP236(int cardNumber);
int M2TSConfigAP236(int cardNumber);


/****
 * 
 * This section defines all the M2 Tip System Control Tasks.
 * 
 */ 

/* 1. Mirror Control Task*/
//void start323MainLoop(int cardNumber);

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