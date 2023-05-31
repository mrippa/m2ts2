#include "CircularBuffer.h"

/*
* This Circular Buffer adds 
*
*/
void initializeBuffer(CircularBuffer *buffer, unsigned int buffer_size, const char *signal_name) {
    buffer->buffer_size = buffer_size;
    strncpy(buffer->signal_name, signal_name, sizeof(buffer->signal_name));
    buffer->payload = (struct Payload *)malloc(buffer_size * sizeof(struct Payload));
    buffer->read_index = 0;
    buffer->write_index = 0;
}


void writeValue(CircularBuffer *buffer, double value) {

    epicsTimeStamp current_timestamp;
    epicsTimeGetCurrent(&current_timestamp);

    buffer->payload[buffer->write_index].value = value;
    
    if (buffer->write_index > 0) {
        double prev_diff_sec = buffer->payload[(buffer->write_index - 1) % buffer->buffer_size].diff_microseconds / 1e6;
        double diff_sec = epicsTimeDiffInSeconds(&current_timestamp, &(buffer->timestamp));
        buffer->payload[buffer->write_index].diff_microseconds = (int)((diff_sec * 1e6) - prev_diff_sec);
    } else {
        buffer->payload[buffer->write_index].diff_microseconds = 0;
    }

    buffer->timestamp = current_timestamp;
    
    buffer->write_index = (buffer->write_index + 1) % buffer->buffer_size;
    if (buffer->write_index == buffer->read_index)
        buffer->read_index = (buffer->read_index + 1) % buffer->buffer_size;
}

double readValue(CircularBuffer *buffer) {
    double value = buffer->payload[buffer->read_index].value;
    buffer->read_index = (buffer->read_index + 1) % buffer->buffer_size;
    return value;
}

void destroyBuffer(CircularBuffer *buffer) {
    free(buffer->payload);
}

void printBuffer(CircularBuffer *buffer) {
    printf("Signal Name: %s\n", buffer->signal_name);
    
    printf("Payload: [");
    unsigned int index = buffer->read_index;
    for (unsigned int i = 0; i < buffer->buffer_size; i++) {
        printf("{%.6f (Diff: %d us)}", buffer->payload[index].value, buffer->payload[index].diff_microseconds);
        
        if (i != buffer->buffer_size - 1)
            printf("\n");
        
        index = (index + 1) % buffer->buffer_size;
    }
    printf("]\n");
}

int main() {
    unsigned int buffer_size = 100; // Example buffer size of 100
    const char *signal_name = "ExampleSignal"; // Example signal name

    CircularBuffer buffer;
    initializeBuffer(&buffer, buffer_size, signal_name);

    // Write values to the circular buffer
    for (int i = 0; i < buffer_size + 10; i++) {
        writeValue(&buffer, i * 0.1);
    }

    // Read values from the circular buffer
    for (int i = 0; i < buffer_size + 10; i++) {
        double value = readValue(&buffer);
        printf("Value: %f\n", value);
    }

    destroyBuffer(&buffer);

    return 0;
}