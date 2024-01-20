/**
 * @file mcp2515.h
 * @author Samuel Henrique (samuelhenriq12@gmail.com)
 * @brief MCP2515 driver
 * @version 1.0
 * @date 14-11-2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _MCP2515_H
#define _MCP2515_H
#include <cstring>
#include <map>
#include <tuple>

#include "can.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "spi.h"

/**
 * @namespace MCP2515
 * @brief Namespace for MCP2515 driver
 */
namespace MCP2515 {

/**
 * @brief Enum class with the possible errors of the MCP2515
 *
 */
enum class Error : int8_t {
  NO_MSG = -5,  //!< No message
  FAIL_TX,      //!< Fail to transmit
  FAIL_INIT,    //!< Fail to initialize
  ALL_TX_BUSY,  //!< All transmit buffers busy
  FAIL,         //!< Fail
  OK            //!< Success
};

/**
 * @brief Enum class with the possible clock speeds of the MCP2515
 *
 */
enum class CanClock : uint8_t {
  k20MHZ /**< 20MHz */,
  k16MHZ /**< 16MHz*/,
  k8MHZ /**< 8MHz*/
};

/**
 * @brief Enum class with the CanClockOut divisor
 *
 */
enum class CanClockOut : int8_t { DISABLE = -1, DIV1, DIV2, DIV4, DIV8 };

/**
 * @brief Enum class with the masks 0 and 1
 *
 */
enum class MASK : uint8_t { MASK0, MASK1 };

/**
 * @brief Enum class with the RX filters of the MCP2515
 *
 */
enum class RXF : uint8_t { RXF0, RXF1, RXF2, RXF3, RXF4, RXF5 };

/**
 * @brief Enum class with the RX buffers of the MCP2515
 *
 */
enum class RXBn : uint8_t { RXB0, RXB1 };

/**
 * @brief Enum class with the TX buffers of the MCP2515
 *
 */
enum class TXBn : uint8_t { TXB0, TXB1, TXB2 };

/**
 * @brief Enum class with the CAN INTERRUPT FLAG REGISTER (CANINTF) of the
 * MCP2515
 *
 */
enum class CANINTF : uint8_t {
  RX0IF = 0x01,  //!<  Receive Buffer 0 Full Interrupt Flag bit
  RX1IF = 0x02,  //!<  Receive Buffer 1 Full Interrupt Flag bit
  TX0IF = 0x04,  //!<  Transmit Buffer 0 Empty Interrupt Flag bit
  TX1IF = 0x08,  //!<  Transmit Buffer 1 Empty Interrupt Flag bit
  TX2IF = 0x10,  //!<  Transmit Buffer 2 Empty Interrupt Flag bit
  ERRIF =
      0x20,  //!<  Error Interrupt Flag bit (multiple sources in EFLG register)
  WAKIF = 0x40,  //!<  Wake-up Interrupt Flag bit
  MERRF = 0x80   //!<  Message Error Interrupt Flag bit
};

/**
 * @brief Enum class with the CAN ERROR FLAG REGISTER (EFLG) of the MCP2515
 *
 */
enum class EFLG : uint8_t {
  EWARN = 0x01,   //!<  Error Warning Flag bit
  RXWAR = 0x02,   //!<  Receive Error Warning Flag bit
  TXWAR = 0x04,   //!<  Transmit Error Warning Flag bit
  RXEP = 0x08,    //!<  Receive Error-Passive Flag bit
  TXEP = 0x10,    //!<  Transmit Error-Passive Flag bit
  TXBO = 0x20,    //!<  Bus-Off Error Flag bit
  RX0OVR = 0x40,  //!<  Receive Buffer 0 Overflow Flag bit
  RX1OVR = 0x80   //!<  Receive Buffer 1 Overflow Flag bit
};

/**
 * @brief Enum class with the modes of operation of the MCP2515
 *
 */
enum class CANCTRL_REQOP_MODE : uint8_t {
  NORMAL = 0x00,      //!<  Normal mode
  SLEEP = 0x20,       //!<  Sleep mode
  LOOPBACK = 0x40,    //!<  Loopback mode
  LISTENONLY = 0x60,  //!<  Listen-only mode
  CONFIG = 0x80,      //!<  Configuration mode
  POWERUP = 0xE0      //!<  Power-up mode
};

/**
 * @brief Enum class with flags receive buffer of the MCP2515
 *
 */
enum class STAT : uint8_t { RX0IF = 0x01, RX1IF = 0x02 };

/**
 * @brief Enum class for register TXBnCTRL of the MCP2515
 *
 */
enum class TXBnCTRL {
  ABTF = 0x40,   //!<  Message Aborted Flag bit
  MLOA = 0x20,   //!<  Message Lost Arbitration Flag bit
  TXERR = 0x10,  //!<  Transmission Error Detected bit
  TXREQ = 0x08,  //!<  Message Transmit Request bit
  TXIE = 0x04,   //!<  Transmit Interrupt Enable bit
  TXP = 0x03     //!<  Transmit Buffer Priority bits
};

/**
 * @brief Enum class with the instructions of the SPI of the MCP2515
 *
 */
enum class Instruction : uint8_t {
  RESET = 0b1100'0000,        //!< Instruction to reset the MCP2515
  READ = 0b0000'0011,         //!< Instruction to read the MCP2515
  WRITE = 0b0000'0010,        //!< Instruction to write the MCP2515
  READ_STATUS = 0b1010'0000,  //!< Instruction to read the status of the MCP2515
  RX_STATUS =
      0b1011'0000,  //!< Instruction to read the receive status of the MCP2515
  BIT_MODIFY = 0b0000'0101  //!< Instruction to modify the bits of the MCP2515
};

/**
 * @brief Enum class with the registers of the MCP2515. \n
 * Name and address of the registers.
 *
 */
enum class Register : uint8_t {
  RXF0SIDH = 0x00,  //!< Receive Filter 0 Standard Identifier High Register
  RXF0SIDL = 0x01,  //!< Receive Filter 0 Standard Identifier Low Register
  RXF0EID8 = 0x02,  //!< Receive Filter 0 Extended Identifier High Register
  RXF0EID0 = 0x03,  //!< Receive Filter 0 Extended Identifier Low Register
  RXF1SIDH = 0x04,  //!< Receive Filter 1 Standard Identifier High Register
  RXF1SIDL = 0x05,  //!< Receive Filter 1 Standard Identifier Low Register
  RXF1EID8 = 0x06,  //!< Receive Filter 1 Extended Identifier High Register
  RXF1EID0 = 0x07,  //!< Receive Filter 1 Extended Identifier Low Register
  RXF2SIDH = 0x08,  //!< Receive Filter 2 Standard Identifier High Register
  RXF2SIDL = 0x09,  //!< Receive Filter 2 Standard Identifier Low Register
  RXF2EID8 = 0x0A,  //!< Receive Filter 2 Extended Identifier High Register
  RXF2EID0 = 0x0B,  //!< Receive Filter 2 Extended Identifier Low Register
  CANSTAT = 0x0E,   //!< CAN Status Register
  CANCTRL = 0x0F,   //!< CAN Control Register
  RXF3SIDH = 0x10,  //!< Receive Filter 3 Standard Identifier High Register
  RXF3SIDL = 0x11,  //!< Receive Filter 3 Standard Identifier Low Register
  RXF3EID8 = 0x12,  //!< Receive Filter 3 Extended Identifier High Register
  RXF3EID0 = 0x13,  //!< Receive Filter 3 Extended Identifier Low Register
  RXF4SIDH = 0x14,  //!< Receive Filter 4 Standard Identifier High Register
  RXF4SIDL = 0x15,  //!< Receive Filter 4 Standard Identifier Low Register
  RXF4EID8 = 0x16,  //!< Receive Filter 4 Extended Identifier High Register
  RXF4EID0 = 0x17,  //!< Receive Filter 4 Extended Identifier Low Register
  RXF5SIDH = 0x18,  //!< Receive Filter 5 Standard Identifier High Register
  RXF5SIDL = 0x19,  //!< Receive Filter 5 Standard Identifier Low Register
  RXF5EID8 = 0x1A,  //!< Receive Filter 5 Extended Identifier High Register
  RXF5EID0 = 0x1B,  //!< Receive Filter 5 Extended Identifier Low Register
  TEC = 0x1C,       //!< Transmit Error Counter Register
  REC = 0x1D,       //!< Receive Error Counter Register
  RXM0SIDH = 0x20,  //!< Receive Buffer 0 Standard Identifier High Register
  RXM0SIDL = 0x21,  //!< Receive Buffer 0 Standard Identifier Low Register
  RXM0EID8 = 0x22,  //!< Receive Buffer 0 Extended Identifier High Register
  RXM0EID0 = 0x23,  //!< Receive Buffer 0 Extended Identifier Low Register
  RXM1SIDH = 0x24,  //!< Receive Buffer 1 Standard Identifier High Register
  RXM1SIDL = 0x25,  //!< Receive Buffer 1 Standard Identifier Low Register
  RXM1EID8 = 0x26,  //!< Receive Buffer 1 Extended Identifier High Register
  RXM1EID0 = 0x27,  //!< Receive Buffer 1 Extended Identifier Low Register
  CNF3 = 0x28,      //!< Configuration Register 3
  CNF2 = 0x29,      //!< Configuration Register 2
  CNF1 = 0x2A,      //!< Configuration Register 1
  CANINTE = 0x2B,   //!< CAN Interrupt Enable Register
  CANINTF = 0x2C,   //!< CAN Interrupt Flag Register
  EFLG = 0x2D,      //!< Error Flag Register
  TXB0CTRL = 0x30,  //!< Transmit Buffer 0 Control Register
  TXB0SIDH = 0x31,  //!< Transmit Buffer 0 Standard Identifier High Register
  TXB0SIDL = 0x32,  //!< Transmit Buffer 0 Standard Identifier Low Register
  TXB0EID8 = 0x33,  //!< Transmit Buffer 0 Extended Identifier High Register
  TXB0EID0 = 0x34,  //!< Transmit Buffer 0 Extended Identifier Low Register
  TXB0DLC = 0x35,   //!< Transmit Buffer 0 Data Length Code Register
  TXB0DATA = 0x36,  //!< Transmit Buffer 0 Data Register
  TXB1CTRL = 0x40,  //!< Transmit Buffer 1 Control Register
  TXB1SIDH = 0x41,  //!< Transmit Buffer 1 Standard Identifier High Register
  TXB1SIDL = 0x42,  //!< Transmit Buffer 1 Standard Identifier Low Register
  TXB1EID8 = 0x43,  //!< Transmit Buffer 1 Extended Identifier High Register
  TXB1EID0 = 0x44,  //!< Transmit Buffer 1 Extended Identifier Low Register
  TXB1DLC = 0x45,   //!< Transmit Buffer 1 Data Length Code Register
  TXB1DATA = 0x46,  //!< Transmit Buffer 1 Data Register
  TXB2CTRL = 0x50,  //!< Transmit Buffer 2 Control Register
  TXB2SIDH = 0x51,  //!< Transmit Buffer 2 Standard Identifier High Register
  TXB2SIDL = 0x52,  //!< Transmit Buffer 2 Standard Identifier Low Register
  TXB2EID8 = 0x53,  //!< Transmit Buffer 2 Extended Identifier High Register
  TXB2EID0 = 0x54,  //!< Transmit Buffer 2 Extended Identifier Low Register
  TXB2DLC = 0x55,   //!< Transmit Buffer 2 Data Length Code Register
  TXB2DATA = 0x56,  //!< Transmit Buffer 2 Data Register
  RXB0CTRL = 0x60,  //!< Receive Buffer 0 Control Register
  RXB0SIDH = 0x61,  //!< Receive Buffer 0 Standard Identifier High Register
  RXB0SIDL = 0x62,  //!< Receive Buffer 0 Standard Identifier Low Register
  RXB0EID8 = 0x63,  //!< Receive Buffer 0 Extended Identifier High Register
  RXB0EID0 = 0x64,  //!< Receive Buffer 0 Extended Identifier Low Register
  RXB0DLC = 0x65,   //!< Receive Buffer 0 Data Length Code Register
  RXB0DATA = 0x66,  //!< Receive Buffer 0 Data Register
  RXB1CTRL = 0x70,  //!< Receive Buffer 1 Control Register
  RXB1SIDH = 0x71,  //!< Receive Buffer 1 Standard Identifier High Register
  RXB1SIDL = 0x72,  //!< Receive Buffer 1 Standard Identifier Low Register
  RXB1EID8 = 0x73,  //!< Receive Buffer 1 Extended Identifier High Register
  RXB1EID0 = 0x74,  //!< Receive Buffer 1 Extended Identifier Low Register
  RXB1DLC = 0x75,   //!< Receive Buffer 1 Data Length Code Register
  RXB1DATA = 0x76   //!< Receive Buffer 1 Data Register
};

constexpr static int N_TXBUFFERS = 3;  //!< Number of transmit buffers
constexpr static int N_RXBUFFERS = 2;  //!< Number of receive buffers

/**
 * @struct TXBn_REGS Registers of the transmit buffers.
 *
 */
struct TXBn_REGS {
  Register CTRL;
  Register SIDH;
  Register DATA;
};

/**
 * @brief RXBn_REGS Registers of the receive buffers.
 *
 */
struct RXBn_REGS {
  Register CTRL;
  Register SIDH;
  Register DATA;
  CANINTF CANINTF_RXnIF;
};

using ConfigEntry =
    std::tuple<int, int,
               int>;  //!< Tuple that stores 3 configuration register values
                      //!< related to the MCP2515 clock and CAN speed.
using ConfigMap = std::map<std::pair<CanClock, CanSpeed>,
                           ConfigEntry>; /**< Map that stores the configuration
                                            register values related to the
                                            MCP2515 clock and CAN speed. */

/**
 * @brief Struct with the configuration of the MCP2515. \n
 * The default values are: \n
 * spi_interface = 0 \n
 * miso_pin = -1 \n
 * mosi_pin = -1 \n
 * sclk_pin = -1 \n
 * cs_pin = -1 \n
 * clock = CanClock::k16MHZ \n
 * speed = CanSpeed::k500KBPS \n
 *
 *
 */
struct ConfigModule {
  int spi_interface = 0;                //!< 0 for VSPI, 1 for HSPI
  int miso_pin = -1;                    //!< MISO pin
  int mosi_pin = -1;                    //!< MOSI pin
  int sclk_pin = -1;                    //!< SCLK pin
  int cs_pin = -1;                      //!< CS pin
  CanClock clock = CanClock::k16MHZ;    //!< MCP2515 clock speed default 16MHz
  CanSpeed speed = CanSpeed::k500KBPS;  //!< CAN bus speed default 500KBPS
};

/**
 * @class Device inherits from SPI::Bus
 *
 */
class Device : protected SPI::Bus {
 private:
  constexpr static int CANCTRL_REQOP = 0xE0;  //!<  Request Operation mode bits
  constexpr static int CANCTRL_ABAT =
      0x10;  //!<  Abort All Pending Transmissions bit
  constexpr static int CANCTRL_OSM = 0x08;     //!<  One-Shot Mode bit
  constexpr static int CANCTRL_CLKEN = 0x04;   //!<  CLKOUT Pin Enable bit
  constexpr static int CANCTRL_CLKPRE = 0x03;  //!<  CLKOUT Pin Prescaler bits
  constexpr static int CANSTAT_OPMOD = 0xE0;   //!<  Operation Mode bits
  constexpr static int CANSTAT_ICOD = 0x0E;    //!<  Interrupt Flag Code bits

  constexpr static int CNF3_SOF = 0x80;  //!<  Start-of-Frame (SOF) signal bit

  constexpr static int TXB_EXIDE_MASK = 0x08;  //!<  Extended Identifier bit
  constexpr static int DLC_MASK = 0x0F;        //!<  Data Length Code bits
  constexpr static int RTR_MASK = 0x40;  //!<  Remote Transmission Request bit

  constexpr static int RXBnCTRL_RXM_STD =
      0x20;  //!<  Receive Buffer Operating mode bits standard identifier
  constexpr static int RXBnCTRL_RXM_EXT =
      0x40;  //!<  Receive Buffer Operating mode bits extended identifier
  constexpr static int RXBnCTRL_RXM_STDEXT =
      0x00;  //!<  Receive Buffer Operating mode bits standard or extended
             //!<  identifier
  constexpr static int RXBnCTRL_RXM_MASK =
      0x60;  //!<  Receive Buffer Operating mode bits mask
  constexpr static int RXBnCTRL_RTR =
      0x08;  //!<  Remote Transmission Request bit
  constexpr static int RXB0CTRL_BUKT = 0x04;         //!<  Rollover Enable bit
  constexpr static int RXB0CTRL_FILHIT_MASK = 0x03;  //!<  Filter Hit bits mask
  constexpr static int RXB1CTRL_FILHIT_MASK = 0x07;  //!<  Filter Hit bits mask
  constexpr static int RXB0CTRL_FILHIT = 0x00;       //!<  Filter 0 hit
  constexpr static int RXB1CTRL_FILHIT = 0x01;       //!<  Filter 1 hit

  constexpr static int SIDH = 0;  //!<  Standard Identifier High bits
  constexpr static int SIDL = 1;  //!<  Standard Identifier Low bits
  constexpr static int EID8 = 2;  //!<  Extended Identifier High bits
  constexpr static int EID0 = 3;  //!<  Extended Identifier Low bits
  constexpr static int DLC = 4;   //!<  Data Length Code bits
  constexpr static int DATA = 5;  //!<  Data bits

  constexpr static int STAT_RXIF_MASK =
      static_cast<int>(STAT::RX0IF) |
      static_cast<int>(STAT::RX1IF);  //!<  Receive Interrupt Flag bit mask

  constexpr static int EFLG_ERRORMASK =
      static_cast<int>(EFLG::RX1OVR) | static_cast<int>(EFLG::RX0OVR) |
      static_cast<int>(EFLG::TXBO) | static_cast<int>(EFLG::TXEP) |
      static_cast<int>(EFLG::RXEP);  //!<  Error Flag bit mask

 private:
  /**
   * @brief Set the Mode of operation of the MCP2515
   *
   * @param mode mode of operation
   * @return Error::OK if the mode was set successfully
   * @return Error::FAIL if the mode was not set successfully
   */
  Error setMode(CANCTRL_REQOP_MODE mode);

  /**
   * @brief Read the register of the MCP2515
   *
   * @param reg Register to be read
   * @return value of the register
   */
  uint8_t readRegister(Register reg);

  /**
   * @brief Read multiples registers of the MCP2515
   *
   * @param reg First register to be read
   * @param data Buffer to store the data
   * @param n  Number of registers to be read
   */
  void readRegisters(Register reg, uint8_t* data, int n);

  /**
   * @brief Write in the register of the MCP2515
   *
   * @param reg Register to be written
   * @param data Value to be written in the register
   */
  void setRegister(Register reg, uint8_t data);

  /**
   * @brief Write in multiples registers of the MCP2515
   *
   * @param reg First register to be written
   * @param data Buffer with the data to be written
   * @param n Number of registers to be written
   */
  void setRegisters(Register reg, const uint8_t* data, int n);

  /**
   * @brief Modify the bits of the register of the MCP2515
   *
   * @param reg Register to be modified
   * @param mask Mask to be applied in the register
   * @param data Value to be written in the register
   */
  void modifyRegister(Register reg, uint8_t mask, uint8_t data);

  /**
   * @brief Set the Filter of the MCP2515
   *
   * @param num Number of the filter
   * @param ext Extended identifier
   * @param ulData Data to be set
   * @return Error::OK if the filter was set successfully
   * @return Error::FAIL if the filter was not set successfully
   */
  Error setFilter(RXF num, bool ext, uint32_t ulData);

  /**
   * @brief Set the Filter Mask of the MCP2515
   *
   * @param mask Mask to be set
   * @param ext Extended identifier
   * @param ulData Data to be set
   * @return Error::OK if the filter mask was set successfully
   * @return Error::FAIL if the filter mask was not set successfully
   */
  Error setFilterMask(MASK mask, bool ext, uint32_t ulData);

  /**
   * @brief Method to prepare the identifier of the message
   *
   * @param buffer Buffer to store the identifier
   * @param ext Flag for extended identifier
   * @param id Identifier of the message
   */
  void prepareId(uint8_t* buffer, bool ext, uint32_t id);

 public:
  /**
   * @brief Construct a new Device object
   *
   * @param config Struct with the configuration of the MCP2515
   */
  Device(const ConfigModule& config);

  /**
   * @brief Destroys the Device object. This destructor is virtual because the
   * class Device is inherited from SPI::Bus. This destructor will call the
   * destructor of the SPI::Bus class.
   */
  virtual ~Device() = default;

  /**
   * @brief Method to reset the MCP2515 via SPI and software.
   *
   * @return Error::OK if the reset was successful
   * @return Error::FAIL if the reset was not successful
   */
  Error reset();

  /**
   * @brief Set the Config Mode of the MCP2515
   *
   * @return Error::OK if the mode was set successfully
   * @return Error::FAIL if the mode was not set successfully
   */
  Error setConfigMode();

  /**
   * @brief Set the Listen Only Mode of the MCP2515
   *
   * @return Error::OK if the mode was set successfully
   * @return Error::FAIL if the mode was not set successfully
   *
   */
  Error setListenOnlyMode();

  /**
   * @brief Set the Loopback Mode of the MCP2515
   *
   * @return Error::OK if the mode was set successfully
   * @return Error::FAIL if the mode was not set successfully
   */
  Error setLoopbackMode();

  /**
   * @brief Set the Normal Mode of the MCP2515
   *
   * @return Error::OK if the mode was set successfully
   * @return Error::FAIL if the mode was not set successfully
   */
  Error setNormalMode();

  /**
   * @brief Set the Sleep Mode of the MCP2515
   *
   * @return Error::OK if the mode was set successfully
   * @return Error::FAIL if the mode was not set successfully
   */
  Error setSleepMode();

  /**
   * @brief Set the Bitrate of the MCP2515. \n Speed of the CAN bus.
   *
   * @param canSpeed Speed of the CAN bus
   * @return Error::OK if the bitrate was set successfully
   * @return Error::FAIL if the bitrate was not set successfully
   */
  Error setBitrate(CanSpeed canSpeed);

  /**
   * @brief Overload of the method setBitrate. \n Speed of the CAN bus and clock
   * of the MCP2515.
   *
   * @param canSpeed Speed of the CAN bus
   * @param canClock Clock of the MCP2515. Default 16MHz
   * @return Error::OK if the bitrate was set successfully
   * @return Error::FAIL if the bitrate was not set successfully
   */
  Error setBitrate(CanSpeed canSpeed, CanClock canClock);

  /**
   * @brief Set the Clk Out of the MCP2515
   *
   * @param divisor Divisor of the clock out
   * @return Error::OK if the clock out was set successfully
   * @return Error::FAIL if the clock out was not set successfully
   */
  Error setClkOut(CanClockOut divisor);

  /**
   * @brief Get the Status of the MCP2515. Use the instruction READ_STATUS of
   * the SPI.
   *
   * @return uint8_t: Status of the MCP2515 \n
   * Bit 7 - CANINTF.TX2IF: Transmit Buffer 2 Empty Interrupt Flag bit \n
   * Bit 6 - TXB2CNTRL.TXREQ: Transmit Buffer 2 Request-to-Send bit \n
   * Bit 5 - CANINTF.TX1IF: Transmit Buffer 1 Empty Interrupt Flag bit \n
   * Bit 4 - TXB1CNTRL.TXREQ: Transmit Buffer 1 Request-to-Send bit \n
   * Bit 3 - CANINTF.TX0IF: Transmit Buffer 0 Empty Interrupt Flag bit \n
   * Bit 2 - TXB0CNTRL.TXREQ: Transmit Buffer 0 Request-to-Send bit \n
   * Bit 1 - CANINTF.RX1IF: Receive Buffer 1 Full Interrupt Flag bit \n
   * Bit 0 - CANINTF.RX0IF: Receive Buffer 0 Full Interrupt Flag bit \n
   */
  uint8_t getStatus();

  /**
   * @brief This method checks if RX0IF or RX1IF is set.
   *
   * @return true if RX0IF or RX1IF is set
   * @return false if RX0IF and RX1IF is not set
   */
  bool checkReceive();

  /**
   * @brief This method checks if receive message
   *
   * @return true if receive message
   * @return false if not receive message
   */
  bool checkError();

  /**
   * @brief Get the Error Flags of the MCP2515. The register EFLG is read.
   *
   * @return value of the register EFLG
   */
  uint8_t getErrorFlags();

  /**
   * @brief Get the RX Status of the MCP2515. Use the instruction RX_STATUS of
   * the SPI.
   *
   * @return 8 bits of the RX Status \n
   * Bits 7-6: Received Message (RXB0 and RXB1) \n
   * Bits 5-4: Message Type Received \n
   * Bits 3-0: Filter Match (RXF0 - RXF5)
   *
   * @see <b>Figure 12-9: RX STATUS INSTRUCTION</b> of the MCP2515 datasheet
   */
  uint8_t getRxStatus();

  /**
   * @brief This method sends a message to the CAN bus.
   *
   * @param txbn Transmit buffer to be used
   * @param message Message to be sent
   * @return Error::OK if the message was sent successfully
   * @return Error::FAIL_TX if data length code is too long
   *
   */
  Error sendMessage(TXBn txbn, CanMessage& message);

  /**
   * @brief This method sends a message to the CAN bus.
   *
   * @param message Message to be sent
   * @return Error::OK if the message was sent successfully
   * @return Error::FAIL_TX if data length code is too long
   * @return Error::ALL_TX_BUSY if all transmit buffers are busy
   */
  Error sendMessage(CanMessage& message);

  /**
   * @brief This method reads a message from the CAN bus.
   *
   * @param rxbn Receive buffer to be used
   * @param message Message to be read
   * @return Error::OK if the message was read successfully
   * @return Error::FAIL if data length code is too long
   */
  Error readMessage(RXBn rxbn, CanMessage& message);

  /**
   * @brief This method reads a message from the CAN bus.
   *
   * @param message Message to be read
   * @return Error::OK if the message was read successfully
   * @return Error::FAIL if data length code is too long
   * @return Error::NO_MSG if there is no message to be read
   */
  Error readMessage(CanMessage& message);

};  // class Device

}  // namespace MCP2515

#endif  // MCP2515_H