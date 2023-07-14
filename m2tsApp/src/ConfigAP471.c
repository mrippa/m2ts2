#include "m2ts471.h"
#include "m2ts471Utils.h"

extern AP471Card m2tsAP471Card[NUM_AP471_CARDS];

int M2TSConfigAP471(int cardNumber)
{

       APSTATUS status = 0;
       AP471Card *p471Card = &m2tsAP471Card[cardNumber];

       if (!p471Card->c_block.bInitialized)
       {
              printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
              return (ERROR);
       }

       /**
        * 
        * 
        * TODO:
        * @brief Configure the AP482
        *
        * ....
        */

       printf("Config AP471 done! 0x%x\n", status);

       return status;
}
