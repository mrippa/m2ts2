#include "m2ts482.h"
#include "m2ts482Utils.h"
extern AP482Card m2tsAP482Card[NUM_AP482_CARDS];

int AP482ShowClocks(int cardNumber)
{
    int counter; /* menu item variable */
    uint32_t value;
    AP482Card *p482Card = &m2tsAP482Card[cardNumber];

    if (!p482Card->c_block.bInitialized) {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return -1;
    }

    /*TODO: select counter from input*/
    p482Card->c_block.counter_num = (BYTE)(0);

    counter = p482Card->c_block.counter_num;
    printf("\n\nCurrent Counter:             %d\n", counter);
    GetCounterConstantAx(&(p482Card->c_block), counter, &value, 1); /* int x; 1 or 2 */
    printf("Counter ConstantA1 Register: %X\n", value);
    GetCounterConstantBx(&(p482Card->c_block), counter, &value, 1); /* int x; 1 or 2 */
    printf("Counter ConstantB1 Register: %X\n\n", value);

    GetCounterConstantAx(&(p482Card->c_block), counter, &value, 2); /* int x; 1 or 2 */
    printf("Counter ConstantA2 Register: %X\n", value);
    GetCounterConstantBx(&(p482Card->c_block), counter, &value, 2); /* int x; 1 or 2 */
    printf("Counter ConstantB2 Register: %X\n\n", value);
    
    return 0;
}

int AP482SetClock( int cardNumber, int frequency, int counter) {

    AP482Card *p482Card = &m2tsAP482Card[cardNumber];
    uint32_t value;

    if (!p482Card->c_block.bInitialized) {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return (ERROR);
    }

    p482Card->c_block.counter_num = counter;

    /* Limit check counter (Allow 0 through 8)*/
    if (counter >8 || counter < 0) {

        printf("Error: Counter val >>%d<< out of range", counter);
    }

    if (frequency != 40 && frequency != 0) {
           printf("Error: AP482 Set Clock: freq: %d\n", frequency);
        return (ERROR);
    }

    value = 0xBEBC; /*40 Hz Clock*/
    SetCounterConstantAx(&(p482Card->c_block), counter, value, 1);	/* int x; 1 or 2 */
    SetCounterConstantBx(&(p482Card->c_block), counter, value, 1);	/* int x; 1 or 2 */
    SetCounterConstantAx(&(p482Card->c_block), counter, 0x0, 2);	/* int x; 1 or 2 */
    SetCounterConstantBx(&(p482Card->c_block), counter, 0x0, 2);	/* int x; 1 or 2 */

    printf("Set AP482 Card %d, counter %d to clock freq value of: 0x%x ", cardNumber, counter, value );

    return 0;
}
