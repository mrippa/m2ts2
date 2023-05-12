#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts48x.h"

int InitAP48x(void)
{

  APSTATUS status = 0;
  hflag = 0; /* indicate interrupt handler not installed yet */

  ap_instance48x = 0;
  /*
      Initialize the Configuration Parameter Block to default values.
  */
  memset(&c_block48x, 0, sizeof(c_block48x));

  c_block48x.bAP = FALSE;          /* indicate not initialized and set up yet */
  c_block48x.bInitialized = FALSE; /* indicate not ready to talk to AP48x */
  c_block48x.nHandle = 0;          /* make handle to a closed AP48x board */

  for (i = 0; i < MAX_CNTR; i++)
  {
    c_block48x.m_CounterConstantA1[i] = 0xFFFFFFFF; /* constant registers */
    c_block48x.m_CounterConstantA2[i] = 0xFFFFFFFF; /* constant registers */
    c_block48x.m_CounterConstantB1[i] = 0xFFFFFFFF; /* constant registers */
    c_block48x.m_CounterConstantB2[i] = 0xFFFFFFFF; /* constant registers */

    c_block48x.m_Mode[i] = None;                       /* the counter mode */
    c_block48x.m_OutputPolarity[i] = OutPolLow;        /* output polarity */
    c_block48x.m_InputAPolarity[i] = InABCPolDisabled; /* input A polarity */
    c_block48x.m_InputBPolarity[i] = InABCPolDisabled; /* input B polarity */
    c_block48x.m_InputCPolarity[i] = InABCPolDisabled; /* input C polarity */
    c_block48x.m_ClockSource[i] = InC1_1953125Mhz;     /* clock source */
    c_block48x.m_SpecialIC[i] = Nosis;                 /* No Output or Special Interrupt Selected */
    c_block48x.m_InterruptEnable[i] = IntDisable;      /* interrupt enable */
    c_block48x.m_Debounce[i] = DebounceOff;            /* Debounce disabled */
  }
  c_block48x.counter_num = 0;

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
  status = APOpen(ap_instance48x, &c_block48x.nHandle, DEVICE_NAME);

  if (status != S_OK)
  {
    printf("\nUnable to Open instance of AP48X.\n");
  }
  else
  {
    if (APInitialize(c_block48x.nHandle) == S_OK) /* Initialize AP48x */
    {
      GetAPAddress(c_block48x.nHandle, &addr); /* Read back address */
      c_block48x.brd_ptr = (struct map48x *)addr;
      c_block48x.bInitialized = TRUE;
      c_block48x.bAP = TRUE;
    }
  }

  printf("Init AP482 done!\n");
  return status;
}
