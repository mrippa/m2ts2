#include "m2ts471.h"
#include "m2ts471Utils.h"

extern AP471Card m2tsAP471Card[NUM_AP471_CARDS];

int M2TSConfigAP471(int cardNumber)
{

    APSTATUS status = 0;
    AP471Card *p471Card = &m2tsAP471Card[cardNumber];

    if (! p471Card->c_block.bInitialized) {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return (ERROR);
    }    

    printf("Configuration Parameters\n\n");
    printf(" 1. Return to Previous Menu\n");
    printf(" 2. Parameter Mask:    0x%02X\n", p471Card->c_block.param);
    printf(" 3. Debounce Clock:    0x%02X\n", p471Card->c_block.deb_clock);
    printf(" 4. Debounce Duration: R0 = 0x%08X R1 = 0x%08X R2 = 0x%08X\n",
           p471Card->c_block.deb_duration[0], p471Card->c_block.deb_duration[1], p471Card->c_block.deb_duration[2]);
    printf(" 5. Debounce Control:  R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X\n",
           p471Card->c_block.deb_control[0], p471Card->c_block.deb_control[1], p471Card->c_block.deb_control[2]);
    printf(" 6. Write Mask:        R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X\n",
           p471Card->c_block.WriteMask[0], p471Card->c_block.WriteMask[1], p471Card->c_block.WriteMask[2]);
    printf(" 7. Event Type:        R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X\n",
           p471Card->c_block.EventType[0], p471Card->c_block.EventType[1], p471Card->c_block.EventType[2]);
    printf(" 8. Event Polarity:    R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X\n",
           p471Card->c_block.EventPolarity[0], p471Card->c_block.EventPolarity[1], p471Card->c_block.EventPolarity[2]);
    printf(" 9. Event Enable:      R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X\n",
           p471Card->c_block.EventEnable[0], p471Card->c_block.EventEnable[1], p471Card->c_block.EventEnable[2]);

    printf("Config AP471 done! 0x%x\n", status);

    return status;
}

