#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <epicsTime.h>


typedef struct {
    epicsTimeStamp timestamp;
    unsigned int buffer_size;
    char signal_name[256];
    struct Payload{
      int diff_microseconds;
      double value;  
    } *payload;
    unsigned int read_index;
    unsigned int write_index;
} CircularBuffer;

void initializeBuffer(CircularBuffer *buffer, unsigned int buffer_size, const char *signal_name);
void writeValue(CircularBuffer *buffer, double value);
void destroyBuffer(CircularBuffer *buffer);
void printBuffer(CircularBuffer *buffer);