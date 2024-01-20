/**
 * @file main_send.h
 * @author Samuel Henrique Guimarães Alencar (samuelhenriq12@gmail.com)
 * @brief Application for sending CAN messages using loopback mode and MPU6050
 * @version 1.0
 * @date 2023-11-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>

#include <iostream>

#include "can.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "mcp2515.h"
#include "mpu6050.h"

QueueHandle_t accQueue;
void vTaskAccel(void* pvParameters);
void vTaskCanSend(void* pvParameters);
void vTaskCanReceive(void* pvParameters);

struct MPU6050::ConfigMPU6050 config;
struct MCP2515::ConfigModule configModule;
MCP2515::Device node(configModule);

/**
 * @brief Example application for sending CAN messages
 *
 */
extern "C" void app_main(void) {
  node.setLoopbackMode();
  accQueue = xQueueCreate(5, sizeof(MPU6050::AxisAccel));
  
  xTaskCreate(vTaskAccel, "TaskAccel", 2048, NULL, 1, NULL);
  xTaskCreate(vTaskCanSend, "TaskCanSend", 2048, NULL, 1, NULL);
  xTaskCreate(vTaskCanReceive, "TaskCanReceive", 2048, NULL, 1, NULL);
}

void vTaskAccel(void* pvParameters) {
  MPU6050::Device device(config);
  while (1) {
    MPU6050::AxisAccel const& axis = device.getAcceleration();
    xQueueSend(accQueue, &axis, portMAX_DELAY);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void vTaskCanSend(void* pvParameters) {
  CanMessage canMsg;
  MPU6050::AxisAccel acc;
  while (1) {
    if (xQueueReceive(accQueue, &acc, portMAX_DELAY) == pdTRUE) {
      canMsg.identifier = 0x123;
      canMsg.data_length_code = 6;
      canMsg.data[0] = acc.x >> 8;
      canMsg.data[1] = acc.x;
      canMsg.data[2] = acc.y >> 8;
      canMsg.data[3] = acc.y;
      canMsg.data[4] = acc.z >> 8;
      canMsg.data[5] = acc.z;
      std::cout << "--------SEND START--------" << std::endl;
      std::cout << "ID_SEND = 0x" << std::hex << canMsg.identifier;
      std::cout << " | DLC_SEND = " << std::dec << static_cast<int>(canMsg.data_length_code);
      std::cout << " | X_SEND = " << acc.x;
      std::cout << " | Y_SEND = " << acc.y;
      std::cout << " | Z_SEND = " << acc.z << std::endl;
      node.sendMessage(canMsg);
      std::cout << "--------SEND END--------" << std::endl;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void vTaskCanReceive(void* pvParameters) {
  CanMessage canRes;
  while (1) {
    if (node.readMessage(canRes) == MCP2515::Error::OK) {
      std::cout << "--------RECEIVE START--------" << std::endl;
      std::cout << "ID = 0x" << std::hex << canRes.identifier;
      std::cout << " | DLC = " << std::dec << static_cast<int>(canRes.data_length_code);
      std::cout << " | X = " << static_cast<int16_t>(((canRes.data[0] << 8) | canRes.data[1]));
      std::cout << " | Y = " << static_cast<int16_t>(((canRes.data[2] << 8) | canRes.data[3]));
      std::cout << " | Z = " << static_cast<int16_t>(((canRes.data[4] << 8) | canRes.data[5])) << std::endl;
      std::cout << "--------RECEIVE END--------" << std::endl;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}