/**
 * @file spi.cpp
 * @author Samuel Henrique (samuelhenriq12@gmail.com)
 * @brief SPI class implementation.
 * @version 1.0
 * @date 14-11-2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "spi.h"

#include <cstdint>
#include <cstring>
#include <iostream>

SPI::Bus::Bus(int miso, int mosi, int sclk, int cs, int host) {
  begin(miso, mosi, sclk, cs, host);
}

/**
 * @brief Destroy the SPI::Bus::Bus object
 * Release the spi bus and the spi device using the functions
 * <b>spi_bus_remove_device</b> and <b>spi_bus_free</b> from the
 * <b>driver/spi_master.h</b> from ESP32.
 */
SPI::Bus::~Bus() {
  spi_bus_remove_device(handle_);
  spi_bus_free(host_);
}

esp_err_t SPI::Bus::begin(int miso, int mosi, int sclk, int cs, int host) {
  /**
   * @brief This function checks the pins for the spi bus and the spi device. \n
   * If <b>host</b> is 1 the function will use the HSPI bus, if <b>host</b> is 2
   * the function will use the VSPI bus. \n If the pins are not defined (-1) the
   * function will use the default pins for the spi bus.
   *
   */
  host_ = host == 1 ? HSPI_HOST : VSPI_HOST;

  if (mosi == -1 && miso == -1 && sclk == -1 && cs == -1) {
    mosi_ = host == 1 ? HSPI_IOMUX_PIN_NUM_MOSI : VSPI_IOMUX_PIN_NUM_MOSI;
    miso_ = host == 1 ? HSPI_IOMUX_PIN_NUM_MISO : VSPI_IOMUX_PIN_NUM_MISO;
    sclk_ = host == 1 ? HSPI_IOMUX_PIN_NUM_CLK : VSPI_IOMUX_PIN_NUM_CLK;
    cs_ = host == 1 ? HSPI_IOMUX_PIN_NUM_CS : VSPI_IOMUX_PIN_NUM_CS;
  } else {
    mosi_ = mosi;
    miso_ = miso;
    sclk_ = sclk;
    cs_ = cs;
  }

  memset(&bus_cfg_, 0, sizeof(spi_bus_config_t));
  memset(&dev_cfg_, 0, sizeof(spi_device_interface_config_t));

  // bus_cfg_.mosi_io_num = mosi_;
  // bus_cfg_.miso_io_num = miso_;
  // bus_cfg_.sclk_io_num = sclk_;
  // bus_cfg_.quadwp_io_num = -1;
  // bus_cfg_.quadhd_io_num = -1;
  bus_cfg_ = {.mosi_io_num = mosi_,
              .miso_io_num = miso_,
              .sclk_io_num = sclk_,
              .quadwp_io_num = -1,
              .quadhd_io_num = -1,
            };

  dev_cfg_ = {.command_bits = 8,
              .address_bits = 8,
              .dummy_bits = 0,
              .mode = 0,
              .duty_cycle_pos = 128,
              .cs_ena_posttrans = 3,
              .clock_speed_hz = SPI_CLOCK,
              .spics_io_num = cs_,
              .queue_size = 3,
              };

  if (spi_bus_initialize(host_, &bus_cfg_, SPI_DMA_CH_AUTO) != ESP_OK) {
    std::cout << "BUS INIT ERROR" << std::endl;
    while (1) {
    }
  }

  if (spi_bus_add_device(host_, &dev_cfg_, &handle_) != ESP_OK) {
    std::cout << "ADD DEVICE ERROR" << std::endl;
    while (1) {
    }
  }

  return ESP_OK;
}

esp_err_t SPI::Bus::transferBytes(uint8_t registerAddress, const uint8_t data,
                                  uint8_t command) {
  /**
   * @brief First, clear the transaction structure to start a new transaction.
   * Then the configuration is done. \n The flags allow the use of the
   * rx_data[4] and tx_data[4] array to read and send data. \n The cmd field is
   * the instruction to be executed. \n The default size is 8 bits, this depends
   * on the device. \n The addr field is the address of the target register. \n
   * The tx_data[0] field receives the byte to be written to the register.
   *
   */
  memset(&transaction_, 0, sizeof(transaction_));
  transaction_.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
  transaction_.cmd = command;
  transaction_.length = 8;
  transaction_.addr = registerAddress;
  transaction_.tx_data[0] = data;

  return spi_device_transmit(handle_, &transaction_);
}

esp_err_t SPI::Bus::transferMultiplesBytes(uint8_t registerAddress,
                                           const uint8_t* txBuffer,
                                           const uint8_t* rxBuffer,
                                           size_t dataLength, uint8_t command) {
  /**
   * @brief Same as transferBytes but for multiples bytes.
   * @see transferBytes.
   *
   */
  dataLength < 1 ? (dataLength = 1) : (dataLength = dataLength);

  memset(&transaction_multi_, 0, sizeof(transaction_multi_));

  transaction_multi_.length = 8 * dataLength;
  transaction_multi_.rxlength = 0;
  transaction_multi_.cmd = command;
  transaction_multi_.addr = registerAddress;
  transaction_multi_.tx_buffer = const_cast<uint8_t*>(txBuffer);
  transaction_multi_.rx_buffer = const_cast<uint8_t*>(rxBuffer);

  return spi_device_transmit(handle_, &transaction_multi_);
}