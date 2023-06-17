#include <epicsPrint.h>
#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts482.h"

AP482Card m2tsAP482Card[NUM_AP482_CARDS];
int m2tsAP482InitFirst = 1;

int M2TSInitAP482(int cardNumber)
{

  AP482Card *p482Card;
  APSTATUS status = 0;
  int i;
  char *MyName = "M2TSInitAP482";
  /*
      Initialize the Configuration Parameter Block to default values.
  */

  /*
   * Check if the cards have already been intialized before.
   */

  if (m2tsAP482InitFirst == 1)
  {
    for (i = 0; i < NUM_AP482_CARDS; i++)
    {
      m2tsAP482Card[i].initialized = FALSE;
      m2tsAP482Card[i].configured = FALSE;
      m2tsAP482Card[i].card = i;
      m2tsAP482Card[i].hflag = 0; /* indicate interrupt handler not installed yet */

      m2tsAP482Card[i].c_block.nHandle = 0;
      printf("Set AP482 Card %d handle to %d \n ", i, m2tsAP482Card[i].c_block.nHandle);
    }
    m2tsAP482InitFirst = 0;
  }

  if ((cardNumber < 0) | (cardNumber >= NUM_AP482_CARDS))
  {
    errlogPrintf("%s: Card number %d invalid -- must be 0 to %d.\n",
                 MyName, cardNumber, NUM_AP482_CARDS - 1);
    return (ERROR);
  }

  p482Card = &m2tsAP482Card[cardNumber];
  memset(&(p482Card->c_block), 0, sizeof(p482Card->c_block)); /* Initialize the Configuration Parameter Block.*/

  p482Card->c_block.bAP = FALSE;          /* indicate not initialized and set up yet */
  p482Card->c_block.bInitialized = FALSE; /* indicate not ready to talk to AP48x */
  p482Card->c_block.nHandle = 0;          /* make handle to a closed AP48x board */

  if (InitAPLib() != S_OK)
  {
    printf("\nUnable to initialize the AP library. Exiting program.\n");
    exit(0);
  }

  /*
    Open an instance of a AP device
    Other device instances can be obtained
    by changing parameter 1 of APOpen()
  */

  status = APOpen(p482Card->card, &(p482Card->c_block).nHandle, AP48X_DEVICENAME);
  printf("AP323 card %d has handle %d\n", p482Card->card, p482Card->c_block.nHandle);
  if (status != S_OK)
  {
    printf("\nUnable to Open instance of AP323 with status %d \n", status);
  }
  else
  {
    status = APInitialize(p482Card->c_block.nHandle);
    if (status == S_OK) /* Initialize */
    {
      GetAPAddress(p482Card->c_block.nHandle, &(p482Card->addr)); /* Read back address */
      p482Card->c_block.brd_ptr = (struct map48x *) (p482Card->addr);
      p482Card->c_block.bInitialized = TRUE;
      p482Card->c_block.bAP = TRUE;
    }
    else {
      printf("AP482: Initialize is false with status 0x%X\n", status);
      return (ERROR);
    }
  }

  printf("Init AP482 done!\n");
  return status;
}