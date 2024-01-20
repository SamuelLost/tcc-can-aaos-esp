#include "i2c.h"

I2C::Bus::Bus(i2c_port_t port, gpio_num_t sda, gpio_num_t scl,
              uint32_t clk_speed)
    : port_(port), sda_(sda), scl_(scl), clk_speed_(clk_speed) {
  if (init() != ESP_OK) {
    ESP_LOGE("I2C", "Failed to initialize I2C bus");
  } else {
    ESP_LOGI("I2C", "Initialized I2C bus");
  }
}

I2C::Bus::~Bus() { i2c_driver_delete(port_); }

esp_err_t I2C::Bus::init() {
  config_.mode = I2C_MODE_MASTER;
  config_.sda_io_num = sda_;
  config_.scl_io_num = scl_;
  config_.sda_pullup_en = GPIO_PULLUP_ENABLE;
  config_.scl_pullup_en = GPIO_PULLUP_ENABLE;
  config_.master.clk_speed = clk_speed_;
  config_.clk_flags = 0;

  esp_err_t err = ESP_OK;

  err |= i2c_param_config(port_, &config_);

  err |= i2c_driver_install(port_, config_.mode, 0, 0, 0);

  return err;
}

uint8_t I2C::Bus::readRegister(uint8_t dev_addr, uint8_t reg_addr) {
  uint8_t rx_data;

  i2c_master_write_read_device(port_, dev_addr, &reg_addr, 1, &rx_data, 1,
                               1000 / portTICK_PERIOD_MS);

  return rx_data;
}

esp_err_t I2C::Bus::writeRegister(uint8_t dev_addr, uint8_t reg_addr,
                                  uint8_t tx_data) {
  uint8_t data[2] = {reg_addr, tx_data};

  return i2c_master_write_to_device(port_, dev_addr, data, 2,
                                    1000 / portTICK_PERIOD_MS);
}

esp_err_t I2C::Bus::readRegisterMultipleBytes(uint8_t dev_addr,
                                              uint8_t reg_addr,
                                              uint8_t* rx_data,
                                              uint32_t length) {
  return i2c_master_write_read_device(port_, dev_addr, &reg_addr, 1, rx_data,
                                      length, 1000 / portTICK_PERIOD_MS);
}

esp_err_t I2C::Bus::readWord(uint8_t dev_addr, uint8_t reg_addr,
                             uint16_t* rx_data) {
  uint8_t data[2];

  esp_err_t err = i2c_master_write_read_device(
      port_, dev_addr, &reg_addr, 1, data, 2, 1000 / portTICK_PERIOD_MS);

  *rx_data = (data[0] << 8) | data[1];

  return err;
}

uint8_t I2C::Bus::readBitFieldFromRegister(uint8_t dev_addr, uint8_t reg_addr,
                                           uint8_t start_bit, uint8_t length) {
  uint8_t data = readRegister(dev_addr, reg_addr);

  return (data >> start_bit) & ((1U << length) - 1);
}

esp_err_t I2C::Bus::writeBitRegister(uint8_t dev_addr, uint8_t reg_addr,
                                     uint8_t bit_num, bool data) {
  uint8_t register_value = readRegister(dev_addr, reg_addr);

  register_value = (data) ? (register_value | (1 << bit_num))
                          : (register_value & ~(1 << bit_num));

  return writeRegister(dev_addr, reg_addr, register_value);
}

esp_err_t I2C::Bus::writeBitField(uint8_t dev_addr, uint8_t reg_addr,
                                  uint8_t start_bit, uint8_t length,
                                  uint8_t data) {
  int8_t register_value = readRegister(dev_addr, reg_addr);
  uint8_t bit_mask = ((1U << length) - 1);
  register_value &= ~(bit_mask << start_bit);
  register_value |= (data << start_bit);

  return writeRegister(dev_addr, reg_addr, register_value);
}
