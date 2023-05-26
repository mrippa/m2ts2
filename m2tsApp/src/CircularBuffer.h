#include <epicsTime.h>

typedef struct {
    epicsTimeStamp timestamp;
    unsigned int sample_rate;
    unsigned int buffer_size;
    char signal_name[256];
    float *payload;
    unsigned int read_index;
    unsigned int write_index;
} CircularBuffer;

