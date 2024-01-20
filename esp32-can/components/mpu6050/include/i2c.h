/**
 * @file i2c.h
 * @author Samuel Henrique Guimarães Alencar (samuel.alencar@eldorado.org.br)
 * @brief Class to handle I2C communication
 * @version 1.0
 * @date 2023-10-02
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _I2C_H_
#define _I2C_H_

#include <cstdint>

#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/i2c_reg.h"
#include "soc/i2c_struct.h"

/**
 * @brief Class to handle I2C communication
 *
 */
namespace I2C {
class Bus {
 private:
  i2c_config_t config_; //!< I2C configuration 
  i2c_port_t port_;    //!< I2C port 
  gpio_num_t sda_;    //!< SDA pin 
  gpio_num_t scl_;   //!< SCL pin 
  uint32_t clk_speed_; //!< Clock speed 

 public:
  /**
   * @brief Construct a new Bus object
   * 
   * @param port port number of the I2C controller
   * @param sda pin number of the SDA pin
   * @param scl pin number of the SCL pin
   * @param clk_speed clock speed of the I2C bus
   */
  Bus(i2c_port_t port, gpio_num_t sda, gpio_num_t scl, uint32_t clk_speed);
  //Bus(i2c_port_t port, gpio_num_t sda, gpio_num_t scl);
  /**
   * @brief Destroy the Bus object. It will delete the I2C controller
   * 
   */
  virtual ~Bus();

  /**
   * @brief A initialization function to configure the I2C controller
   * 
   * @return esp_err_t \n 
   *        - ESP_OK: success \n
   *       - ESP_FAIL: fail \n
   */
  esp_err_t init();

  /**
   * @brief Method to write a byte to a register. It will write a byte to a
   * register of a device connected to the I2C bus.
   * 
   * @param dev_addr address of the device.
   * @param reg_addr address of the register target.
   * @param tx_data data to be written.
   * @return esp_err_t \n - ESP_OK: success \n - ESP_FAIL: fail \n
   *
   */
  esp_err_t writeRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t tx_data);

  /**
   * @brief Method to write multiple bytes to a register. It will write multiple
   * bytes to a register of a device connected to the I2C bus.
   * TODO: Implement this method
   * @param dev_addr address of the device.
   * @param reg_addr address of the register target.
   * @param tx_data data to be written.
   * @param length number of bytes to be written.
   * @return esp_err_t \n - ESP_OK: success \n - ESP_FAIL: fail \n
   */
  esp_err_t writeRegisterMultipleBytes(uint8_t dev_addr, uint8_t reg_addr,
                                       uint8_t *tx_data, uint32_t length);

  /**
   * @brief Method to write a bit to a register. It will write a bit to a
   * register of a device connected to the I2C bus.
   * 
   * @param dev_addr address of the device.
   * @param reg_addr address of the register target.
   * @param bit_num number of the bit to be written.
   * @param data data to be written.
   * @return esp_err_t \n - ESP_OK: success \n - ESP_FAIL: fail \n
   *
   */
  esp_err_t writeBitRegister(uint8_t dev_addr, uint8_t reg_addr,
                             uint8_t bit_num, bool data);

  /**
   * @brief Method to write a bit field to a register. It will write a bit field
   * to a register of a device connected to the I2C bus.
   * 
   * @param dev_addr address of the device.
   * @param reg_addr address of the register target.
   * @param start_bit number of the start bit to be written.
   * @param length number of bits to be written.
   * @param data data to be written.
   * @return esp_err_t \n - ESP_OK: success \n - ESP_FAIL: fail \n
   */
  esp_err_t writeBitField(uint8_t dev_addr, uint8_t reg_addr, uint8_t start_bit,
                          uint8_t length, uint8_t data);

  /**
   * @brief method to read a byte from a register. It will read a byte from a
   * register of a device connected to the I2C bus.
   * 
   * @param dev_addr address of the device.
   * @param reg_addr address of the register target.
   * @return uint8_t data read from the register.
   */
  uint8_t readRegister(uint8_t dev_addr, uint8_t reg_addr);

  /**
   * @brief Method to read multiple bytes from a register. It will read multiple
   * bytes from a register of a device connected to the I2C bus.
   * 
   * @param dev_addr address of the device.
   * @param reg_addr address of the register target.
   * @param rx_data pointer to the array where the data will be stored.
   * @param length number of bytes to be read.
   * @return esp_err_t \n - ESP_OK: success \n - ESP_FAIL: fail \n
   */
  esp_err_t readRegisterMultipleBytes(uint8_t dev_addr, uint8_t reg_addr,
                                      uint8_t *rx_data, uint32_t length); 

  /**
   * @brief Method to read a bit field from a register. It will read a bit field
   * from a register of a device connected to the I2C bus.
   * 
   * @param dev_addr address of the device.
   * @param reg_addr address of the register target.
   * @param start_bit number of the start bit to be read.
   * @param length number of bits to be read.
   * @return uint8_t data read from the register.
   */
  uint8_t readBitFieldFromRegister(uint8_t dev_addr, uint8_t reg_addr,
                                   uint8_t start_bit, uint8_t length);

  /**
   * @brief Method to read a word from a register. It will read a word from a
   * register of a device connected to the I2C bus.
   * 
   * @param dev_addr address of the device.
   * @param reg_addr address of the register target.
   * @param rx_data pointer to the variable where the data will be stored.
   * @return esp_err_t \n - ESP_OK: success \n - ESP_FAIL: fail \n
   */
  esp_err_t readWord(uint8_t dev_addr, uint8_t reg_addr, uint16_t *rx_data);

  /**
   * @brief Get the sda pin number
   * 
   * @return gpio_num_t sda pin number
   *
   */
  gpio_num_t getSda() const { return sda_; };

  /**
   * @brief Get the scl pin number
   * 
   * @return gpio_num_t scl pin number
   *
   */
  gpio_num_t getScl() const { return scl_; };

  /**
   * @brief Get the Clk Speed value
   * 
   * @return uint32_t Clk Speed value
   */
  uint32_t getClkSpeed() const { return clk_speed_; };
};
}  // namespace I2C

#endif  // _I2C_H_