#include "m2ts482.h"
extern AP482Card m2tsAP482Card[NUM_AP482_CARDS];

int M2TSConfigAP482(int cardNumber)
{

    APSTATUS status = 0;
    AP482Card *p482Card = &m2tsAP482Card[cardNumber];
    int i;

    if (!p482Card->c_block.bInitialized) {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return (ERROR);
    }

    p482Card->c_block.counter_num = 0;

    for (i = 0; i < MAX_CNTR; i++)
    {
        p482Card->c_block.m_CounterConstantA1[i] = 0xFFFFFFFF; /* constant registers */
        p482Card->c_block.m_CounterConstantA2[i] = 0xFFFFFFFF; /* constant registers */
        p482Card->c_block.m_CounterConstantB1[i] = 0xFFFFFFFF; /* constant registers */
        p482Card->c_block.m_CounterConstantB2[i] = 0xFFFFFFFF; /* constant registers */

        p482Card->c_block.m_Mode[i] = None;                       /* the counter mode */
        p482Card->c_block.m_OutputPolarity[i] = OutPolLow;        /* output polarity */
        p482Card->c_block.m_InputAPolarity[i] = InABCPolDisabled; /* input A polarity */
        p482Card->c_block.m_InputBPolarity[i] = InABCPolDisabled; /* input B polarity */
        p482Card->c_block.m_InputCPolarity[i] = InABCPolDisabled; /* input C polarity */
        p482Card->c_block.m_ClockSource[i] = InC1_1953125Mhz;     /* clock source */
        p482Card->c_block.m_SpecialIC[i] = Nosis;                 /* No Output or Special Interrupt Selected */
        p482Card->c_block.m_InterruptEnable[i] = IntDisable;      /* interrupt enable */
        p482Card->c_block.m_Debounce[i] = DebounceOff;            /* Debounce disabled */
    }

    printf("Config AP482 done! 0x%x\n", status);

    return status;
}
