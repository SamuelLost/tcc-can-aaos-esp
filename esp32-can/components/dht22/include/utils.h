#include <cstring>

void floatToBytes(float f, uint8_t* bytes) {
    std::memcpy(bytes, &f, sizeof(float));
}

float bytesToFloat(uint8_t* bytes) {
    float f;
    std::memcpy(&f, bytes, sizeof(float));
    return f;
}