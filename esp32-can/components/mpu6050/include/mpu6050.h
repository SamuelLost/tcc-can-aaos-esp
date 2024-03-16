/**
 * @file mpu6050.h
 * @author Samuel Henrique Guimarães Alencar (samuel.alencar@eldorado.org.br)
 * @brief Class to handle MPU6050 communication
 * @version 1.0
 * @date 2023-10-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MPU6050_H
#define MPU6050_H

#include <iostream>

#include "i2c.h"

/**
 * @brief A namespace for MPU6050
 * 
 */
namespace MPU6050 {

/**
 * @brief A struct to store the acceleration data. Each axis is a int16_t
 * 
 */
struct AxisAccel {
  int16_t x; //!< X axis
  int16_t y; //!< Y axis
  int16_t z; //!< Z axis

  /**
   * @brief Default constructor. It will initialize all axis with 0
   * 
   */
  AxisAccel() noexcept : AxisAccel(0, 0, 0) {}
  /**
   * @brief Construct a new Axis Accel object. It will initialize the axis with
   * the given values.
   * @param x X axis
   * @param y Y axis
   * @param z Z axis
   */
  AxisAccel(int x, int y, int z) noexcept : x(x), y(y), z(z) {}
};

/**
 * @brief Enum to store the full scale of the accelerometer. G means gravity.
 * 
 */
enum class AccelerationFullScale : uint8_t { 
  A2G, //!< 2G
  A4G, //!< 4G
  A8G, //!< 8G 
  A16G //!< 16G 
};

/**
 * @brief Enum to store the full scale of the gyroscope. DPS means degrees per
 * second.
 * 
 */
enum class GyroscopeFullScale : uint8_t {
  G250DPS, //!< 250 DPS
  G500DPS, //!< 500 DPS
  G1000DPS, //!< 1000 DPS
  G2000DPS //!< 2000 DPS
}; 

/**
 * @brief A struct to store the configuration of the MPU6050. It will be used to
 * initialize the MPU6050.
 * 
 */
struct ConfigMPU6050 {
  i2c_port_t port = I2C_NUM_0; //!< I2C port
  gpio_num_t sda = GPIO_NUM_21; //!< SDA pin
  gpio_num_t scl = GPIO_NUM_22; //!< SCL pin
  uint32_t clk_speed = 400000; //!< Clock speed
  GyroscopeFullScale gyro_full_scale = GyroscopeFullScale::G250DPS; //!< Gyroscope full scale
  AccelerationFullScale accel_full_scale = AccelerationFullScale::A2G; //!< Acceleration full scale
};


/**
 * @brief Enum to store the register address of the MPU6050
 * 
 * @see Section 3 of the datasheet:
 * https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf
 */
enum class Register : uint8_t {
  GYROCONFIG = 0x1B,
  ACCELCONFIG = 0x1C,
  XOUTH = 0x3B,
  XOUTL = 0x3C,
  YOUTH = 0x3D,
  YOUTL = 0x3E,
  ZOUTH = 0x3F,
  ZOUTL = 0x40,
  PWRMGMT1 = 0x6B,
  PWRMGMT2 = 0x6C,
  WHOAMI = 0x75
};

/**
 * @class Device
 * @brief Class to handle MPU6050 communication with I2C::Bus inheritance
 * 
 */
class Device : protected I2C::Bus {
 public:

  /**
   * @brief Construct a new Device object
   * 
   * @param config Configuration of the MPU6050
   */
  Device(ConfigMPU6050& config);

  /**
   * @brief Destroy the Device object
   * 
   */
  virtual ~Device() = default;

  /**
   * @brief Reset all registers to default values
   * 
   * @note This method writes bit 7 of the PWRMGMT1 register to 1
   * 
   * @see Section 4.28 of the datasheet: 
   * https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf
   * 
   * 
   */
  void reset();

  /**
   * @brief Check if the MPU6050 is connected
   * 
   * @return true if connected
   * @return false if not connected
   */
  bool isConnected();

  /**
   * @brief Wake up the MPU6050
   * 
   */
  void wakeUp() { writeSleep(false); };

  /**
   * @brief Put the MPU6050 to sleep
   * 
   */
  void sleep() { writeSleep(true); };

  /**
   * @brief Get the Acceleration object
   * 
   * @return AxisAccel const& 
   */
  AxisAccel const& getAcceleration() {
    readAcceleration();
    return acceleration_;
  };

 private:
  /**
   * @brief Initialize the MPU6050
   * 
   * @return true 
   * @return false 
   */
  bool init();

  /**
   * @brief Read a register
   * 
   * @param reg Register address
   * @return uint8_t: a value of the register
   */
  uint8_t readRegister(Register reg);

  /**
   * @brief Read a bit field of a register
   * 
   * @param reg a register address
   * @param start_bit start bit of the field
   * @param length length of the field
   * @return uint8_t: a value of the field
   */
  uint8_t readBitField(Register reg, uint8_t start_bit, uint8_t length);

  /**
   * @brief Read a block of data
   * 
   * @param reg register address
   * @param data pointer to store the data
   * @param length length of the block to read
   * @return esp_err_t \n - ESP_OK: success \n - ESP_FAIL: fail \n
   */
  esp_err_t readBlock(Register reg, uint8_t* data, uint32_t length);

  /**
   * @brief Write a register
   * 
   * @param reg register address
   * @param data data to write
   * @return esp_err_t \n - ESP_OK: success \n - ESP_FAIL: fail \n
   */
  esp_err_t writeRegister(Register reg, uint8_t data);

  /**
   * @brief Write a bit field of a register
   * 
   * @param reg register address
   * @param start_bit start bit of the field
   * @param length length of the field
   * @param data data to write
   * @return esp_err_t \n - ESP_OK: success \n - ESP_FAIL: fail \n
   */
  esp_err_t writeBitField(Register reg, uint8_t start_bit, uint8_t length,
                          uint8_t data);

  /**
   * @brief Write a bit to a register
   * 
   * @param reg register address
   * @param bit_num number of the bit to write
   * @param data data to write
   * @return esp_err_t \n - ESP_OK: success \n - ESP_FAIL: fail \n
   */
  esp_err_t writeBit(Register reg, uint8_t bit_num, bool data);

  /**
   * @brief Put the MPU6050 to sleep or wake up
   * 
   * @param sleep 0 = awake, 1 = sleep
   */
  void writeSleep(bool sleep);  //<! Put device to sleep. 0 = awake, 1 = sleep

  /**
   * @brief Read the acceleration data
   * 
   */
  void readAcceleration();

 private:

  ConfigMPU6050 config_; //!< Configuration of the MPU6050
  AxisAccel acceleration_; //!< Acceleration data
  uint32_t acc_scale_ = 0; //!< Acceleration scale
  double gyro_scale_ = 0; //!< Gyroscope scale
  bool is_initialized_ = false; //!< Flag to check if the MPU6050 is initialized
};
}  // namespace MPU6050

#endif  // MPU6050_H