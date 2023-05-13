#ifndef M2TS_H
#define M2TS_H


#include <stdio.h>
#include <string.h>

int InitAP323(void );
int InitAP236(void );
int InitAP471(void );
int InitAP48x(void );

int write_AP236out (double myvolts);

#endif