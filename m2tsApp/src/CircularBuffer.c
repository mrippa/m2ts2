#include "CircularBuffer.h"

void initializeBuffer(CircularBuffer *buffer, unsigned int sample_rate, unsigned int buffer_size, const char *signal_name) {
    buffer->sample_rate = sample_rate;
    buffer->buffer_size = buffer_size;
    strncpy(buffer->signal_name, signal_name, sizeof(buffer->signal_name));
    buffer->payload = (float *)malloc(buffer_size * sizeof(float));
    buffer->read_index = 0;
    buffer->write_index = 0;
}

void writeValue(CircularBuffer *buffer, float value) {
    buffer->payload[buffer->write_index] = value;
    gettimeofday(&(buffer->timestamp), NULL);
    
    buffer->write_index = (buffer->write_index + 1) % buffer->buffer_size;
    if (buffer->write_index == buffer->read_index)
        buffer->read_index = (buffer->read_index + 1) % buffer->buffer_size;
}

float readValue(CircularBuffer *buffer) {
    float value = buffer->payload[buffer->read_index];
    buffer->read_index = (buffer->read_index + 1) % buffer->buffer_size;
    return value;
}

void destroyBuffer(CircularBuffer *buffer) {
    free(buffer->payload);
}

int main() {
    unsigned int sample_rate = 1000; // Example sample rate of 1000 Hz
    unsigned int buffer_size = 100; // Example buffer size of 100
    const char *signal_name = "ExampleSignal"; // Example signal name

    CircularBuffer buffer;
    initializeBuffer(&buffer, sample_rate, buffer_size, signal_name);

    // Write values to the circular buffer
    for (int i = 0; i < buffer_size + 10; i++) {
        writeValue(&buffer, i * 0.1);
    }

    // Read values from the circular buffer
    for (int i = 0; i < buffer_size + 10; i++) {
        float value = readValue(&buffer);
        printf("Value: %f\n", value);
    }

    destroyBuffer(&buffer);

    return 0;
}