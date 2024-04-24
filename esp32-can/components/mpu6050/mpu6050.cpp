#include "mpu6050.h"

constexpr uint8_t kDeviceAddress = 0x68; //!< MPU6050 device address
constexpr uint8_t kDeviceId = 0x34;     //!< MPU6050 device id
constexpr uint8_t kAccelerometerRegisterAccelConfig_FullScaleStartBit = 0x03; //!< Start bit of the full scale field of the accelerometer configuration register
constexpr uint8_t kAccelerometerRegisterAccelConfig_FullScaleLength = 0x02; //!< Length of the full scale field of the accelerometer configuration register
constexpr uint8_t kAccelorometerRegisterGyroConfig_FullScaleStartBit = 0x03; //!< Start bit of the full scale field of the gyroscope configuration register
constexpr uint8_t kAccelorometerRegisterGyroConfig_FullScaleLength = 0x02; //!< Length of the full scale field of the gyroscope configuration register
constexpr uint8_t kAccelerometerRegisterPwrMgmt1_DeviceResetBit = 0x07; //!< Device reset bit of the power management 1 register
constexpr uint8_t kAccelerometerRegisterPwrMgmt1_SleepBit = 0x06; //!< Sleep bit of the power management 1 register
constexpr uint8_t kAccelerometerRegisterWhoAmI_StartBit = 0x01; //!< Start bit of the who am i register
constexpr uint8_t kAccelerometerRegisterWhoAmI_Length = 0x06; //!< Length of the who am i register

/**
 * @brief Construct a new MPU6050::Device::Device object
 * 
 * @param config configuration of the MPU6050
 */
MPU6050::Device::Device(ConfigMPU6050& config)
    : I2C::Bus(config.port, config.sda, config.scl, config.clk_speed),
      config_(config) {
  if (init()) {
    ESP_LOGI("MPU6050", "Initialized MPU6050");
  } else {
    ESP_LOGE("MPU6050", "Failed to initialize MPU6050");
  }
}

bool MPU6050::Device::init() {
  if (!isConnected()) {
    return false;
  }

  acc_scale_ = 32768 / (2 << static_cast<uint32_t>(config_.accel_full_scale));
  gyro_scale_ = 262 / (2 << static_cast<uint32_t>(config_.gyro_full_scale));

  wakeUp();

  // Set accelerometer range
  writeBitField(MPU6050::Register::ACCELCONFIG,
                kAccelerometerRegisterAccelConfig_FullScaleStartBit,
                kAccelerometerRegisterAccelConfig_FullScaleLength,
                static_cast<uint8_t>(config_.accel_full_scale));

  // Set gyroscope range
  writeBitField(MPU6050::Register::GYROCONFIG,
                kAccelorometerRegisterGyroConfig_FullScaleStartBit,
                kAccelorometerRegisterGyroConfig_FullScaleLength,
                static_cast<uint8_t>(config_.gyro_full_scale));
  return true;
}

bool MPU6050::Device::isConnected() {
  return readBitField(MPU6050::Register::WHOAMI,
                      kAccelerometerRegisterWhoAmI_StartBit,
                      kAccelerometerRegisterWhoAmI_Length) == kDeviceId;
}

uint8_t MPU6050::Device::readRegister(Register reg) {
  return I2C::Bus::readRegister(kDeviceAddress, static_cast<uint8_t>(reg));
}

uint8_t MPU6050::Device::readBitField(Register reg, uint8_t start_bit,
                                      uint8_t length) {
  return I2C::Bus::readBitFieldFromRegister(
      kDeviceAddress, static_cast<uint8_t>(reg), start_bit, length);
}

esp_err_t MPU6050::Device::readBlock(Register reg, uint8_t* data,
                                     uint32_t length) {
  return I2C::Bus::readRegisterMultipleBytes(
      kDeviceAddress, static_cast<uint8_t>(reg), data, length);
}

esp_err_t MPU6050::Device::writeRegister(Register reg, uint8_t data) {
  return I2C::Bus::writeRegister(kDeviceAddress, static_cast<uint8_t>(reg),
                                 data);
}

esp_err_t MPU6050::Device::writeBitField(Register reg, uint8_t start_bit,
                                         uint8_t length, uint8_t data) {
  return I2C::Bus::writeBitField(kDeviceAddress, static_cast<uint8_t>(reg),
                                 start_bit, length, data);
}

esp_err_t MPU6050::Device::writeBit(Register reg, uint8_t bit_num, bool data) {
  return I2C::Bus::writeBitRegister(kDeviceAddress, static_cast<uint8_t>(reg),
                                    bit_num, data);
}

void MPU6050::Device::writeSleep(bool sleep) {
  writeBit(MPU6050::Register::PWRMGMT1, kAccelerometerRegisterPwrMgmt1_SleepBit,
           sleep);
}

void MPU6050::Device::reset() {
  writeBit(MPU6050::Register::PWRMGMT1,
           kAccelerometerRegisterPwrMgmt1_DeviceResetBit, true);
}

void MPU6050::Device::readAcceleration() {
  
  if (!isConnected()) {
    ESP_LOGE("MPU6050", "Device not connected");
    is_initialized_ = false;
    acceleration_ = {-1, -1, -1};
    return;
  }

  if (!is_initialized_) {
    if (!init()) {
      ESP_LOGE("MPU6050", "Failed to initialize device");
      is_initialized_ = false;
      acceleration_ = {-1, -1, -1};
      return;
    }
    ESP_LOGI("MPU6050", "Device initialized");
    is_initialized_ = true;
  }

  uint8_t data[6];
  esp_err_t status = readBlock(MPU6050::Register::XOUTH, data, 6);
  if (status == ESP_OK) {
    acceleration_.x = (data[0] << 8) | data[1];
    acceleration_.y = (data[2] << 8) | data[3];
    acceleration_.z = (data[4] << 8) | data[5];
  } else {
    ESP_LOGE("MPU6050", "Failed to read acceleration");
  }
}

