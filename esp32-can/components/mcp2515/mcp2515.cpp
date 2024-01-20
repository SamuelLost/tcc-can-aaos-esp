/**
 * @file mcp2515.cpp
 * @author Samuel Henrique (samuelhenriq12@gmail.com)
 * @brief MCP2515 CAN controller driver
 * @version 1.0
 * @date 14-11-2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "mcp2515.h"

#include <iostream>

/**
 * @brief Map of the MCP2515 configuration registers for each
 * combination of clock and speed. The values are taken from the
 * Arduino MCP2515 library.
 * @see [MCP2515 library](https://github.com/autowp/arduino-mcp2515)
 *
 */
const MCP2515::ConfigMap __configMap = {
    // {CanClock, CanSpeed} -> {cfg1, cfg2, cfg3}
    {{MCP2515::CanClock::k16MHZ, CanSpeed::k5KBPS}, {0x3F, 0xFF, 0x87}},
    {{MCP2515::CanClock::k16MHZ, CanSpeed::k10KBPS}, {0x1F, 0xFF, 0x87}},
    {{MCP2515::CanClock::k16MHZ, CanSpeed::k20KBPS}, {0x0F, 0xFF, 0x87}},
    {{MCP2515::CanClock::k16MHZ, CanSpeed::k33KBPS}, {0x4E, 0xF1, 0x85}},
    {{MCP2515::CanClock::k16MHZ, CanSpeed::k40KBPS}, {0x07, 0xFF, 0x87}},
    {{MCP2515::CanClock::k16MHZ, CanSpeed::k50KBPS}, {0x07, 0xFA, 0x87}},
    {{MCP2515::CanClock::k16MHZ, CanSpeed::k80KBPS}, {0x03, 0xFF, 0x87}},
    {{MCP2515::CanClock::k16MHZ, CanSpeed::k83K3BPS}, {0x03, 0xBE, 0x07}},
    {{MCP2515::CanClock::k16MHZ, CanSpeed::k100KBPS}, {0x03, 0xFA, 0x87}},
    {{MCP2515::CanClock::k16MHZ, CanSpeed::k125KBPS}, {0x03, 0xF0, 0x86}},
    {{MCP2515::CanClock::k16MHZ, CanSpeed::k200KBPS}, {0x01, 0xFA, 0x87}},
    {{MCP2515::CanClock::k16MHZ, CanSpeed::k250KBPS}, {0x41, 0xF1, 0x85}},
    {{MCP2515::CanClock::k16MHZ, CanSpeed::k500KBPS}, {0x00, 0xF0, 0x86}},
    {{MCP2515::CanClock::k16MHZ, CanSpeed::k1000KBPS}, {0x00, 0xD0, 0x82}}};

/**
 * @brief Struct containing the MCP2515 registers for each
 * transmit buffer.
 *
 */
struct MCP2515::TXBn_REGS TXB[MCP2515::N_TXBUFFERS] = {
    {MCP2515::Register::TXB0CTRL, MCP2515::Register::TXB0SIDH,
     MCP2515::Register::TXB0DATA},
    {MCP2515::Register::TXB1CTRL, MCP2515::Register::TXB1SIDH,
     MCP2515::Register::TXB1DATA},
    {MCP2515::Register::TXB2CTRL, MCP2515::Register::TXB2SIDH,
     MCP2515::Register::TXB2DATA}};

/**
 * @brief Struct containing the MCP2515 registers for each
 * receive buffer.
 *
 */
struct MCP2515::RXBn_REGS RXB[MCP2515::N_RXBUFFERS] = {
    {MCP2515::Register::RXB0CTRL, MCP2515::Register::RXB0SIDH,
     MCP2515::Register::RXB0DATA, MCP2515::CANINTF::RX0IF},
    {MCP2515::Register::RXB1CTRL, MCP2515::Register::RXB1SIDH,
     MCP2515::Register::RXB1DATA, MCP2515::CANINTF::RX1IF}};

MCP2515::Device::Device(const ConfigModule& config)
    : Bus(config.miso_pin, config.mosi_pin, config.sclk_pin, config.cs_pin,
          config.spi_interface) {
  /**
   * @brief Base class initialization I2C::Bus
   */
  if (reset() == Error::FAIL) {
    std::cout << "RESET FAIL" << std::endl;
    while (1) {
    }
  }

  if (setBitrate(config.speed, config.clock) == Error::FAIL) {
    std::cout << "SET BITRATE FAIL" << std::endl;
    while (1) {
    }
  }

  if (setNormalMode() == Error::FAIL) {
    std::cout << "SET NORMAL MODE FAIL" << std::endl;
    while (1) {
    }
  }
}

MCP2515::Error MCP2515::Device::reset() {
  if (transferBytes(0x00, 0x00,
                    static_cast<uint8_t>(MCP2515::Instruction::RESET)) !=
      ESP_OK) {
    std::cout << "RESET ERROR" << std::endl;
    return Error::FAIL;
  }
  vTaskDelay(10 / portTICK_PERIOD_MS);

  setRegister(Register::TXB0CTRL, 0x00);
  setRegister(Register::TXB1CTRL, 0x00);
  setRegister(Register::TXB2CTRL, 0x00);
  setRegister(Register::RXB0CTRL, 0x00);
  setRegister(Register::RXB1CTRL, 0x00);

  setRegister(Register::CANINTE, static_cast<int>(CANINTF::RX0IF) |
                                     static_cast<int>(CANINTF::RX1IF) |
                                     static_cast<int>(CANINTF::ERRIF) |
                                     static_cast<int>(CANINTF::MERRF));

  modifyRegister(Register::RXB0CTRL,
                 RXBnCTRL_RXM_MASK | RXB0CTRL_BUKT | RXB0CTRL_FILHIT_MASK,
                 RXBnCTRL_RXM_STDEXT | RXB0CTRL_BUKT | RXB0CTRL_FILHIT);
  modifyRegister(Register::RXB1CTRL, RXBnCTRL_RXM_MASK | RXB1CTRL_FILHIT_MASK,
                 RXBnCTRL_RXM_STDEXT | RXB1CTRL_FILHIT);

  RXF filters[] = {RXF::RXF0, RXF::RXF1, RXF::RXF2,
                   RXF::RXF3, RXF::RXF4, RXF::RXF5};

  for (int i = 0; i < 6; i++) {
    bool ext = (i == 1);
    if (setFilter(filters[i], ext, 0) != Error::OK) {
      return Error::FAIL;
    }
  }

  MCP2515::MASK masks[] = {MASK::MASK0, MASK::MASK1};

  for (int i = 0; i < 2; i++) {
    if (setFilterMask(masks[i], true, 0) != Error::OK) {
      return Error::FAIL;
    }
  }

  return Error::OK;
}

MCP2515::Error MCP2515::Device::setConfigMode() {
  return setMode(CANCTRL_REQOP_MODE::CONFIG);
}

MCP2515::Error MCP2515::Device::setListenOnlyMode() {
  return setMode(CANCTRL_REQOP_MODE::LISTENONLY);
}

MCP2515::Error MCP2515::Device::setLoopbackMode() {
  return setMode(CANCTRL_REQOP_MODE::LOOPBACK);
}

MCP2515::Error MCP2515::Device::setNormalMode() {
  return setMode(CANCTRL_REQOP_MODE::NORMAL);
}

MCP2515::Error MCP2515::Device::setSleepMode() {
  return setMode(CANCTRL_REQOP_MODE::SLEEP);
}

MCP2515::Error MCP2515::Device::setBitrate(const CanSpeed canSpeed) {
  return setBitrate(canSpeed, CanClock::k16MHZ);
}

MCP2515::Error MCP2515::Device::setBitrate(const CanSpeed canSpeed,
                                           const CanClock canClock) {
  Error error = setConfigMode();
  if (error != Error::OK) {
    std::cout << "ERROR SETTING CONFIG MODE" << std::endl;
    return error;
  }

  const auto& config = __configMap.find({canClock, canSpeed});
  if (config == __configMap.end()) {
    return Error::FAIL;
  }
  setRegister(Register::CNF1,
              static_cast<uint8_t>(std::get<0>(config->second)));
  setRegister(Register::CNF2,
              static_cast<uint8_t>(std::get<1>(config->second)));
  setRegister(Register::CNF3,
              static_cast<uint8_t>(std::get<2>(config->second)));
  return Error::OK;
}

MCP2515::Error MCP2515::Device::setClkOut(MCP2515::CanClockOut divisor) {
  if (divisor == CanClockOut::DISABLE) {
    modifyRegister(Register::CANCTRL, CANCTRL_CLKEN, 0x00);  // Turn off CLKEN
    modifyRegister(Register::CNF3, CNF3_SOF,
                   CNF3_SOF);  // Turn on CLKOUT for SOF
    return Error::OK;
  }

  modifyRegister(Register::CANCTRL, CANCTRL_CLKPRE,
                 static_cast<int>(divisor));  // Set the prescaler (CLKPRE)
  modifyRegister(Register::CANCTRL, CANCTRL_CLKEN,
                 CANCTRL_CLKEN);                   // Turn on CLKEN
  modifyRegister(Register::CNF3, CNF3_SOF, 0x00);  // Turn off CLKOUT for SOF

  return Error::OK;
}

bool MCP2515::Device::checkReceive() { return (getStatus() & STAT_RXIF_MASK); }

bool MCP2515::Device::checkError() {
  return (getErrorFlags() & EFLG_ERRORMASK);
}

uint8_t MCP2515::Device::getErrorFlags() {
  return readRegister(Register::EFLG);
}

void MCP2515::Device::setRegister(Register reg, uint8_t data) {
  if (transferBytes(static_cast<uint8_t>(reg), data,
                    static_cast<uint8_t>(MCP2515::Instruction::WRITE)) !=
      ESP_OK) {
    std::cout << "SET REGISTER ERROR" << std::endl;
    while (1) {
    }
  }
}

uint8_t MCP2515::Device::readRegister(Register reg) {
  /**
   * @brief This function uses the I2C::Bus::transferBytes() function to
   * read a single byte from the MCP2515 register.
   *
   */
  if (transferBytes(static_cast<uint8_t>(reg), 0x00,
                    static_cast<uint8_t>(MCP2515::Instruction::READ)) !=
      ESP_OK) {
    std::cout << "READ ERROR" << std::endl;
    while (1) {
    }
  }

  return transaction_.rx_data[0];
}

void MCP2515::Device::prepareId(uint8_t* buffer, const bool ext,
                                const uint32_t id) {
  uint16_t can_id = static_cast<uint16_t>(id & 0x0FFFF);

  if (ext) {
    buffer[EID0] = static_cast<uint8_t>(can_id & 0xFF);
    buffer[EID8] = static_cast<uint8_t>(can_id >> 8);
    can_id = static_cast<uint16_t>(id >> 16);
    buffer[SIDL] = static_cast<uint8_t>(can_id & 0x03);
    buffer[SIDL] += static_cast<uint8_t>((can_id & 0x1C) << 3);
    buffer[SIDL] |= TXB_EXIDE_MASK;
    buffer[SIDH] = static_cast<uint8_t>(can_id >> 5);
  } else {
    buffer[SIDH] = static_cast<uint8_t>(can_id >> 3);
    buffer[SIDL] = static_cast<uint8_t>((can_id & 0x07) << 5);
    buffer[EID0] = 0;
    buffer[EID8] = 0;
  }
}

MCP2515::Error MCP2515::Device::setFilterMask(const MASK mask, const bool ext,
                                              const uint32_t ulData) {
  setConfigMode();
  uint8_t tbufdata[4];
  prepareId(tbufdata, ext, ulData);
  Register reg;
  switch (mask) {
    case MASK::MASK0:
      reg = Register::RXM0SIDH;
      break;
    case MASK::MASK1:
      reg = Register::RXM1SIDH;
      break;
    default:
      return Error::FAIL;
      break;
  }

  setRegisters(reg, tbufdata, 4);
  return Error::OK;
}

MCP2515::Error MCP2515::Device::setFilter(const RXF num, const bool ext,
                                          const uint32_t ulData) {
  setConfigMode();
  Register reg;
  switch (num) {
    case RXF::RXF0:
      reg = Register::RXF0SIDH;
      break;
    case RXF::RXF1:
      reg = Register::RXF1SIDH;
      break;
    case RXF::RXF2:
      reg = Register::RXF2SIDH;
      break;
    case RXF::RXF3:
      reg = Register::RXF3SIDH;
      break;
    case RXF::RXF4:
      reg = Register::RXF4SIDH;
      break;
    case RXF::RXF5:
      reg = Register::RXF5SIDH;
      break;
    default:
      return Error::FAIL;
      break;
  }

  uint8_t tbufdata[4];
  prepareId(tbufdata, ext, ulData);
  setRegisters(reg, tbufdata, 4);
  return Error::OK;
}

void MCP2515::Device::setRegisters(Register reg, const uint8_t* data, int n) {
  if (transferMultiplesBytes(
          static_cast<uint8_t>(reg), data, nullptr, n,
          static_cast<uint8_t>(MCP2515::Instruction::WRITE)) != ESP_OK) {
    std::cout << "SET REGISTERS ERROR" << std::endl;
    while (1) {
    }
  }
}

void MCP2515::Device::readRegisters(Register reg, uint8_t* data, int n) {
  if (transferMultiplesBytes(
          static_cast<uint8_t>(reg), nullptr, data, n,
          static_cast<uint8_t>(MCP2515::Instruction::READ)) != ESP_OK) {
    std::cout << "READ REGISTERS ERROR" << std::endl;
    while (1) {
    }
  }
}

void MCP2515::Device::modifyRegister(Register reg, uint8_t mask, uint8_t data) {
  uint8_t txBuffer[] = {mask, data};

  if (transferMultiplesBytes(
          static_cast<uint8_t>(reg), txBuffer, nullptr, sizeof(txBuffer),
          static_cast<uint8_t>(MCP2515::Instruction::BIT_MODIFY)) != ESP_OK) {
    std::cout << "MODIFY REGISTER ERROR" << std::endl;
    while (1) {
    }
  }
}

MCP2515::Error MCP2515::Device::setMode(CANCTRL_REQOP_MODE mode) {
  modifyRegister(Register::CANCTRL, CANCTRL_REQOP, static_cast<int>(mode));

  for (int i = 0; i < 10; i++) {
    uint8_t new_mode = readRegister(Register::CANSTAT);
    new_mode &= CANSTAT_OPMOD;
    if (new_mode == static_cast<uint8_t>(mode)) {
      return Error::OK;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  return Error::FAIL;
}

uint8_t MCP2515::Device::getStatus() {
  if (transferBytes(0x00, 0x00,
                    static_cast<uint8_t>(MCP2515::Instruction::READ_STATUS)) !=
      ESP_OK) {
    std::cout << "GET STATUS ERROR" << std::endl;
  }

  return transaction_.rx_data[0];
}

uint8_t MCP2515::Device::getRxStatus() {
  if (transferBytes(0x00, 0x00,
                    static_cast<uint8_t>(MCP2515::Instruction::RX_STATUS)) !=
      ESP_OK) {
    std::cout << "GET RX STATUS ERROR" << std::endl;
  }

  return transaction_.rx_data[0];
}

MCP2515::Error MCP2515::Device::readMessage(CanMessage& message) {
  uint8_t stat = getStatus();

  if (stat & static_cast<uint8_t>(STAT::RX0IF)) {
    return readMessage(RXBn::RXB0, message);
  } else if (stat & static_cast<uint8_t>(STAT::RX1IF)) {
    return readMessage(RXBn::RXB1, message);
  }
  return Error::NO_MSG;
}

MCP2515::Error MCP2515::Device::readMessage(RXBn rxbn, CanMessage& message) {
  const RXBn_REGS* rxb = &RXB[static_cast<int>(rxbn)];

  uint8_t tbufdata[5];

  readRegisters(rxb->SIDH, tbufdata, 5);

  uint32_t id = (tbufdata[SIDH] << 3) + (tbufdata[SIDL] >> 5);

  if ((tbufdata[SIDL] & TXB_EXIDE_MASK) == TXB_EXIDE_MASK) {
    id = (id << 2) + (tbufdata[SIDL] & 0x03);
    id = (id << 8) + tbufdata[EID8];
    id = (id << 8) + tbufdata[EID0];
    id |= CAN_EFF_FLAG;
  }

  uint8_t dlc = (tbufdata[DLC] & DLC_MASK);
  if (dlc > CAN_MAX_DATA_LENGTH) {
    return Error::FAIL;
  }

  uint8_t ctrl = readRegister(rxb->CTRL);
  if (ctrl & RXBnCTRL_RTR) {
    id |= CAN_RTR_FLAG;
  }

  message.identifier = id;
  message.data_length_code = dlc;

  readRegisters(rxb->DATA, message.data, dlc);

  modifyRegister(Register::CANINTF, static_cast<int>(rxb->CANINTF_RXnIF), 0);

  return Error::OK;
}

MCP2515::Error MCP2515::Device::sendMessage(CanMessage& message) {
  if (message.data_length_code > CAN_MAX_DATA_LENGTH) {
    return Error::FAIL_TX;
  }

  TXBn txBuffers[N_TXBUFFERS] = {TXBn::TXB0, TXBn::TXB1, TXBn::TXB2};

  for (int i = 0; i < N_TXBUFFERS; i++) {
    const TXBn_REGS* txbuf = &TXB[static_cast<int>(txBuffers[i])];
    uint8_t ctrlval = readRegister(txbuf->CTRL);
    if ((ctrlval & static_cast<int>(TXBnCTRL::TXREQ)) == 0) {
      return sendMessage(txBuffers[i], message);
    }
  }

  return Error::ALL_TX_BUSY;
}

MCP2515::Error MCP2515::Device::sendMessage(TXBn txbn, CanMessage& message) {
  if (message.data_length_code > CAN_MAX_DATA_LENGTH) {
    return Error::FAIL_TX;
  }

  const TXBn_REGS* txbuf = &TXB[static_cast<int>(txbn)];

  uint8_t data[13];

  bool ext = (message.identifier & CAN_EFF_FLAG);
  bool rtr = (message.identifier & CAN_RTR_FLAG);
  uint32_t id = (message.identifier & (ext ? CAN_EFF_MASK : CAN_SFF_MASK));

  prepareId(data, ext, id);

  data[DLC] =
      rtr ? (message.data_length_code | RTR_MASK) : message.data_length_code;

  memcpy(&data[DATA], message.data, message.data_length_code);

  setRegisters(txbuf->SIDH, data, 5 + message.data_length_code);

  modifyRegister(txbuf->CTRL, static_cast<int>(TXBnCTRL::TXREQ),
                 static_cast<int>(TXBnCTRL::TXREQ));

  uint8_t ctrl = readRegister(txbuf->CTRL);
  if ((ctrl &
       (static_cast<int>(TXBnCTRL::ABTF) | static_cast<int>(TXBnCTRL::MLOA) |
        static_cast<int>(TXBnCTRL::TXERR))) != 0) {
    return Error::FAIL_TX;
  }
  return Error::OK;
}

// Path: components/mcp2515/include/mcp2515.h
