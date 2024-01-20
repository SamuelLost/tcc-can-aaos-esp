/**
 * @file spi.h
 * @author Samuel Henrique (samuelhenriq12@gmail.com)
 * @brief SPI class header.
 * @version 1.0
 * @date 14-11-2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _SPI_H_
#define _SPI_H_

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "soc/spi_pins.h"

/**
 * @namespace SPI
 * @brief Namespace for SPI driver
 */
namespace SPI {

/**
 * @class Bus
 * @brief Class for bus SPI, that provides the communication between the spi bus
 * of the ESP32.
 *
 */
class Bus {
 public:
  /**
   * @brief Construct a new Bus object
   *
   * @param miso pin number for miso.
   * @param mosi pin number for mosi.
   * @param sclk pin number for sclk.
   * @param cs  pin number for cs.
   * @param host host number for spi bus.
   */
  Bus(int miso, int mosi, int sclk, int cs, int host);

  //! Destructor for Bus object
  virtual ~Bus();

  /**
   * @brief A function to initialize the spi bus
   * This function initializes the spi bus and the spi device using the
   * <b>driver/spi_master.h</b> from ESP32.
   *
   * @param miso pin number for miso.
   * @param mosi pin number for mosi.
   * @param sclk pin number for sclk.
   * @param cs pin number for cs.
   * @param host host number for spi bus.
   * @return esp_err_t: Structure with the error code. \n
   *                    ESP_OK if the SPI bus was initialized correctly \n
   *                    ESP_FAIL if the SPI bus was not initialized correctly
   */
  esp_err_t begin(int miso, int mosi, int sclk, int cs, int host);

  /**
   * @brief Function to transfer data through the spi bus
   * This function transfers data through the spi bus using the
   * <b>driver/spi_master.h</b> from ESP32. This function is used to write or
   * read data.
   *
   * @param registerAddress address of the register to be written or read.
   * @param data value to be written in the register.
   * @param command command to be sent to the device. Instructions are defined
   * in datasheet.
   * @return esp_err_t: Structure with the error code. \n
   *                  ESP_OK if the data was transferred correctly \n
   *                  ESP_FAIL if the data was not transferred correctly
   */
  esp_err_t transferBytes(uint8_t registerAddress, uint8_t data,
                          uint8_t command);

  /**
   * @brief Function to transfer multiples bytes through the spi bus
   * This function transfers multiples bytes through the spi bus using the
   * <b>driver/spi_master.h</b> from ESP32. This function is used to write or
   * read multiples bytes.
   *
   * @param registerAddress address of the register to be written or read.
   * @param txBuffer buffer with the data to be written in the register.
   * @param rxBuffer buffer with the read data.
   * @param dataLength length of the data to be written or read.
   * @param command command to be sent to the device. Instructions are defined
   * in datasheet.
   * @return esp_err_t: Structure with the error code. \n
   *                   ESP_OK if the data was transferred correctly \n
   *                  ESP_FAIL if the data was not transferred correctly
   */
  esp_err_t transferMultiplesBytes(uint8_t registerAddress,
                                   const uint8_t* txBuffer,
                                   const uint8_t* rxBuffer, size_t dataLength,
                                   uint8_t command);

 public:
  /**
   * @brief Structure of the ESP-IDF for transaction spi.
   *
   * It is possible to define the register, command, transaction size and some
   * flags. Contains the buffer to send data for writing and the buffer for
   * reading.
   */
  spi_transaction_t transaction_;

  spi_transaction_t
      transaction_multi_;  //!< Same as transaction_ but for multiples bytes

 private:
  constexpr static uint32_t SPI_CLOCK =
      10'000'000;  //!< spi clock speed (10MHz)

  spi_device_handle_t handle_;  //!< Handle for spi device
  spi_bus_config_t bus_cfg_;    //!< Structure for spi bus configuration
  spi_device_interface_config_t
      dev_cfg_;             //!< Structure for spi device configuration
  spi_host_device_t host_;  //!< Host number for spi bus

  int miso_;  //!< MISO pin
  int mosi_;  //!< MOSI pin
  int sclk_;  //!< SCLK pin
  int cs_;    //!< CS pin
};
}  // namespace SPI

#endif  // _SPI_H_
