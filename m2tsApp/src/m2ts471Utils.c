#include "m2ts471Utils.h"

void m2ts471PrintBits(uint16_t word) {
    for (int i = 15; i >= 0; i--) {
        uint16_t mask = 1 << i;
        uint16_t bit = (word & mask) >> i;
        printf("%u", bit);
        
        if ( i % 4 )
            printf("%hu"); 
    }
    printf("\n");
}
