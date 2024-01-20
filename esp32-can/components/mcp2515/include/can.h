/**
 * @file can.h
 * @author Samuel Henrique (samuelhenriq12@gmail.com)
 * @brief 
 * @version 1.0
 * @date 14-11-2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CAN_H
#define CAN_H

#include <cstdint>
#include <cstring>

/**
 * @brief Flags for CAN message IDs
 *
 */
constexpr int CAN_EFF_FLAG =
    0x80000000UL;  //!< Flag for EFF/SFF IDs. Is set in the MSB
constexpr int CAN_RTR_FLAG =
    0x40000000UL;  //!< Flag for remote transmission request (RTR)
constexpr int CAN_ERR_FLAG = 0x20000000UL;  //!< Flag for error message frame

/**
 * @brief Valid bits in CAN ID for frame formats
 *
 */
constexpr int CAN_SFF_MASK =
    0x000007FFUL;  //!< Mask for standard frame format (SFF)
constexpr int CAN_EFF_MASK =
    0x1FFFFFFFUL;  //!< Mask for extended frame format (EFF)
constexpr int CAN_ERR_MASK = 0x1FFFFFFFUL;  //!< Maks omit EFF, RTR, ERR flags

constexpr short CAN_MAX_DATA_LENGTH = 8;  //!< Maximum length of a CAN message

/**
 * @brief Enum class with the possible speeds of the CAN
 *
 */
enum class CanSpeed : uint8_t {
  k5KBPS,     //!< CAN speed of 5KBPS
  k10KBPS,    //!< CAN speed of 10KBPS
  k20KBPS,    //!< CAN speed of 20KBPS
  k31K25BPS,  //!< CAN speed of 31.25KBPS
  k33KBPS,    //!< CAN speed of 33.3KBPS
  k40KBPS,    //!< CAN speed of 40KBPS
  k50KBPS,    //!< CAN speed of 50KBPS
  k80KBPS,    //!< CAN speed of 80KBPS
  k83K3BPS,   //!< CAN speed of 83.3KBPS
  k95KBPS,    //!< CAN speed of 95KBPS
  k100KBPS,   //!< CAN speed of 100KBPS
  k125KBPS,   //!< CAN speed of 125KBPS
  k200KBPS,   //!< CAN speed of 200KBPS
  k250KBPS,   //!< CAN speed of 250KBPS
  k500KBPS,   //!< CAN speed of 500KBPS
  k666KBPS,   //!< CAN speed of 666KBPS
  k1000KBPS   //!< CAN speed of 1000KBPS (1MBPS)
};

/**
 * @brief Struct with the CAN message
 *
 */
struct CanMessage {
  bool identifier_extended = 0;  //!< Flag for extended identifier
  bool rtr = 0;                  //!< Flag for remote transmission request
  uint32_t identifier = 0;           //!< Identifier of the message (or payload)
  uint8_t data_length_code = 0;      //!< Length of the data
  uint8_t data[CAN_MAX_DATA_LENGTH];  //!< Data of the message
  CanMessage() {
    memset(data, 0, CAN_MAX_DATA_LENGTH);
  };
};

#endif /* CAN_H */