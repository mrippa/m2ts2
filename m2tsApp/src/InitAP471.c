 #include <epicsExport.h>
#include <iocsh.h>

#include "m2ts471.h"

int InitAP471(void ) {


hflag = 0;        /* indicate interrupt handler not installed yet */

/*
    Initialize the Configuration Parameter Block to default values.
*/

    memset(&c_block471, 0, sizeof(c_block471));
    memset(&s_block471, 0, sizeof(s_block471));
    c_block471.bAP = FALSE;          /* indicate no AP initialized and set up yet */
    c_block471.bInitialized = FALSE; /* indicate not ready to talk to AP */
    c_block471.sblk_ptr = (struct sblk471*)&s_block471; /* set address of status structure */
    
/*
	Initialize the AP library
*/
    if(InitAPLib() != S_OK)
    {
      printf("\nUnable to initialize the AP library. Exiting program.\n");
      exit(0);
    }

/*
	Open an instance of a device
	Other device instances can be obtained
	by changing parameter 1 of APOpen()
*/


    if(APOpen(ap_instance, &c_block471.nHandle, DEVICE_NAME ) != S_OK)
    {
      printf("\nUnable to Open instance of %s.\n",DEVICE_NAME );
      finished = 1;	 /* indicate finished with program */
    }
    else
    {
      if(APInitialize(c_block471.nHandle) == S_OK)/* Initialize */
      {
        GetAPAddress(c_block471.nHandle, &addr);	/* Read back address */
        c_block471.brd_ptr = (struct map471 *)addr;
        c_block471.bInitialized = TRUE;
        c_block471.bAP = TRUE;

        /* Create the interrupt processing thread */
        //pthread_create(&IHandler, NULL, (void *)&InterruptHandlerThread, (struct cblk471 *) &c_block471);
      }
    }

} 
