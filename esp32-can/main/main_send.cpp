/**
 * @file main_send.h
 * @author Samuel Henrique Guimarães Alencar (samuelhenriq12@gmail.com)
 * @brief Application for sending CAN messages
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
#include "dht22.h"
#include "utils.h"

QueueHandle_t acc_queue;
QueueHandle_t temperature_queue;
void vTaskAccel(void* pvParameters);
void vTaskCanSend(void* pvParameters);
void vTaskCanReceive(void* pvParameters);
void vTaskTemperature(void* pvParameters);

struct MPU6050::ConfigMPU6050 config;
struct MCP2515::ConfigModule configModule;

MCP2515::Device node(configModule);

/**
 * @brief Example application for sending CAN messages
 *
 */
extern "C" void app_main(void) {
  // node.setBitrate(CanSpeed::k125KBPS);
  // node.setLoopbackMode();
  acc_queue = xQueueCreate(1, sizeof(MPU6050::AxisAccel));
  temperature_queue = xQueueCreate(1, sizeof(float));
  
  xTaskCreate(vTaskAccel, "TaskAccel", 2048, nullptr, 1, nullptr);
  xTaskCreate(vTaskCanSend, "TaskCanSend", 2048, nullptr, 1, nullptr);
  // xTaskCreate(vTaskCanReceive, "TaskCanReceive", 2048, nullptr, 1, nullptr);
  xTaskCreate(vTaskTemperature, "TaskTemperature", 2048, nullptr, 1, nullptr);
}

void vTaskAccel(void* pvParameters) {
  MPU6050::Device device(config);
  while (1) {
    MPU6050::AxisAccel const& axis = device.getAcceleration();
    xQueueSend(acc_queue, &axis, 0);
    // std::cout << "X = " << axis.x << " | Y = " << axis.y << " | Z = " << axis.z << std::endl;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void vTaskCanSend(void* pvParameters) {
  CanMessage can_msg;
  MPU6050::AxisAccel acc;
  float temp;
  while (1) {
    if (xQueueReceive(acc_queue, &acc, 0) == pdTRUE) {
      can_msg.identifier = 0x123;
      can_msg.data_length_code = 6;
      can_msg.data[0] = acc.x >> 8;
      can_msg.data[1] = acc.x;
      can_msg.data[2] = acc.y >> 8;
      can_msg.data[3] = acc.y;
      can_msg.data[4] = acc.z >> 8;
      can_msg.data[5] = acc.z;
      // std::cout << "--------SEND ACCELERATION DATA--------" << std::endl;
      // std::cout << "ID_SEND = 0x" << std::hex << can_msg.identifier;
      // std::cout << " | DLC_SEND = " << std::dec << static_cast<int>(can_msg.data_length_code);
      // std::cout << " | X_SEND = " << acc.x;
      // std::cout << " | Y_SEND = " << acc.y;
      // std::cout << " | Z_SEND = " << acc.z << std::endl;
      node.sendMessage(can_msg);
      // if (err == MCP2515::Error::OK) {
      //   std::cout << "--------SEND OK--------" << std::endl;

      // } else {
      //   std::cout << "--------SEND ERROR--------" << std::endl;
      //   std::cout << "ERROR = " << static_cast<int>(err) << std::endl;
      // }
      // std::cout << "--------SEND END--------" << std::endl;
    }

    if (xQueueReceive(temperature_queue, &temp, 0) == pdTRUE) {
      can_msg.identifier = 0x124;
      can_msg.data_length_code = 4;
      floatToBytes(temp, can_msg.data);
      // for (int i = 0; i < 4; i++) {
      //   std::cout << std::hex << "[" << (int) can_msg.data[i] << "]";
      // }
      // std::cout << std::endl;
      // std::cout << "--------SEND TEMPERATURE DATA--------" << std::endl;
      // std::cout << "ID_SEND = 0x" << std::hex << can_msg.identifier;
      // std::cout << " | DLC_SEND = " << std::dec << static_cast<int>(can_msg.data_length_code);
      // std::cout << " | TEMP_SEND = " << temp << " ºC" << std::endl;
      node.sendMessage(can_msg);
      // std::cout << "--------SEND END--------" << std::endl;

    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void vTaskCanReceive(void* pvParameters) {
  CanMessage canRes;
  while (1) {
    if (node.readMessage(canRes) == MCP2515::Error::OK) {
      // std::cout << "--------RECEIVE START--------" << std::endl;
      switch (canRes.identifier) {
        case 0x123:
          std::cout << "--------RECEIVE ACCELERATION--------" << std::endl;
          std::cout << "ID = 0x" << std::hex << canRes.identifier;
          std::cout << " | DLC = " << std::dec << static_cast<int>(canRes.data_length_code);
          std::cout << " | X = " << static_cast<int16_t>(((canRes.data[0] << 8) | canRes.data[1]));
          std::cout << " | Y = " << static_cast<int16_t>(((canRes.data[2] << 8) | canRes.data[3]));
          std::cout << " | Z = " << static_cast<int16_t>(((canRes.data[4] << 8) | canRes.data[5])) << std::endl;
          // std::cout << "--------RECEIVE END--------" << std::endl;
          break;
        
        case 0x124:
          std::cout << "--------RECEIVE TEMPERATURE--------" << std::endl;
          std::cout << "ID = 0x" << std::hex << canRes.identifier;
          std::cout << " | DLC = " << std::dec << static_cast<int>(canRes.data_length_code);
          std::cout << " | TEMP = " << bytesToFloat(canRes.data) << " ºC" << std::endl;
          break;
        default:
          break;
      }
    }
    // std::cout << "--------RECEIVE END--------" << std::endl;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void vTaskTemperature(void *pvParameters) {
  DHT dht;
  // dht.setDHTgpio(GPIO_NUM_4);
  while (1) {
    dht.errorHandler(dht.readDHT());
    float temp = dht.getTemperature();
    xQueueSend(temperature_queue, &temp, 0);
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}