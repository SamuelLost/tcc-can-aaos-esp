#include <stdio.h>

#include "mcp2515.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <iostream>

void send_task(void* pvParameters);
void receive_task(void* pvParameters);

extern "C" void app_main(void) {
  // queue_can = xQueueCreate(5, sizeof(CanMessage));
  xTaskCreatePinnedToCore(receive_task, "Receive Task", 2048, NULL, 1, NULL,
                          PRO_CPU_NUM);
  // xTaskCreatePinnedToCore(send_task, "Send Task", 2048 + 1024, NULL, 1, NULL,
  //                         APP_CPU_NUM);
}

// void send_task(void* pvParameters) {
//   CanMessage canData;
//   int16_t x, y, z;
//   while (1) {
//     if (xQueueReceive(queue_can, &canData, portMAX_DELAY) == pdPASS) {
//       x = canData.data[0] << 8;
//       x |= canData.data[1];

//       y = canData.data[2] << 8;
//       y |= canData.data[3];

//       z = canData.data[4] << 8;
//       z |= canData.data[5];
      
//       vTaskDelay(100 / portTICK_PERIOD_MS);
//     }
//   }
// }

void receive_task(void* pvParameters) {
  MCP2515::ConfigModule configModule;
  MCP2515::Device node(configModule);

  while (1) {
    CanMessage canRes;
    node.readMessage(canRes);

    std::cout << "\t--------RECEIVE DATA--------" << std::endl;
    std::cout << "\tID: 0x" << std::hex <<canRes.identifier << " DLC: " << static_cast<int>(canRes.data_length_code) << std::endl;
    std::cout << "\tDATA: ";
    for (int i = 0; i < canRes.data_length_code; i++) {
      std::cout << static_cast<int>(canRes.data[i]) << " ";
    }
    std::cout << std::endl;
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}
