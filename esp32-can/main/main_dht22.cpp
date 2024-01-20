#include "dht22.h"
#include "mcp2515.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>
#include <cstring>


void floatToBytes(float f, uint8_t* bytes) {
    std::memcpy(bytes, &f, sizeof(float));
}

float bytesToFloat(uint8_t* bytes) {
    float f;
    std::memcpy(&f, bytes, sizeof(float));
    return f;
}

extern "C" void app_main() {
    MCP2515::ConfigModule configModule;
    DHT dht;
    // configModule.clock = MCP2515::CanClock::k8MHZ;
    MCP2515::Device node(configModule);
    dht.setDHTgpio(GPIO_NUM_4);


    while(1) {

        dht.errorHandler(dht.readDHT());
        printf("Humidity: %.1f %% Temperature: %.1f *C\n", dht.getHumidity(), dht.getTemperature());
        float temp = dht.getTemperature();
        uint8_t data[8];
        floatToBytes(temp, data);
        // printf("Data: %d %d %d %d %d %d %d %d\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);

        // float temp_res = bytesToFloat(data);
        // printf("Result: %.1f\n", temp_res);
        CanMessage canMsg;
        canMsg.identifier = 0x123;
        canMsg.data_length_code = 4;
        canMsg.data[0] = data[0];
        canMsg.data[1] = data[1];
        canMsg.data[2] = data[2];
        canMsg.data[3] = data[3];
        node.sendMessage(canMsg);
        
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}