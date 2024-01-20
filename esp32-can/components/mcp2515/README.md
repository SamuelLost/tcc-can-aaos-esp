# MCP2515 CAN Bus Controller Driver Documentation

#### Overview

This driver provides an interface to the MCP2515 CAN Bus Controller. It is based on the [MCP2515 Stand-Alone CAN Controller with SPI Interface](https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP2515-Family-Data-Sheet-DS20001801K.pdf) datasheet.

#### Prerequisites

- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#installation-step-by-step) installed and configured
- [ESP32](https://www.espressif.com/en/products/socs/esp32) development board
- [MCP2515](https://www.microchip.com/en-us/product/mcp2515) CAN Bus Controller
- [MCP2515 Stand-Alone CAN Controller with SPI Interface](https://www.eletrogate.com/modulo-can-bus-mcp2515-tja1050?utm_source=Site&utm_medium=GoogleMerchant&utm_campaign=GoogleMerchant&utm_source=google&utm_medium=cpc&utm_campaign=[MC4]_[PMax]_PrincipaisCategorias&utm_content=&utm_term=&gad=1&gclid=CjwKCAjw38SoBhB6EiwA8EQVLujYcS72rm_ggWMpVJV2xqvEcRHi4ZGZUIlc5_sqGiHogT-2tzfApRoCddMQAvD_BwE) module
- [This driver](components/mcp2515) in your project's `components` folder
- Wires to connect the MCP2515 module to the ESP32

#### Hardware Setup

Follow the diagram below to connect the MCP2515 module to the ESP32.

| **MCP2515** | **ESP32** | **Wire** |
|:-----------:|:---------:|:--------:|
|     VCC     |     5V    |    Red   |
|     GND     |    GND    |   Black  |
|      CS     |   GPIO5   |  Magenta |
|     MISO    |   GPIO19  |   Green  |
|     MOSI    |   GPIO23  |   White  |
|     SCK     |   GPIO18  |  Orange  |
|     INT     |    N.C    |  No wire |

#### Software Setup

##### Configuring the driver

To configure the driver, you must create a `MCP2515::ConfigModule` struct and pass it to the constructor of the `MCP2515` class. The struct has the following fields:

```cpp
struct ConfigModule {
  int spi_interface = 0;                //!< 0 for VSPI, 1 for HSPI
  int miso_pin = -1;                    //!< MISO pin
  int mosi_pin = -1;                    //!< MOSI pin
  int sclk_pin = -1;                    //!< SCLK pin
  int cs_pin = -1;                      //!< CS pin
  CanClock clock = CanClock::k16MHZ;    //!< MCP2515 clock speed default 16MHz
  CanSpeed speed = CanSpeed::k500KBPS;  //!< CAN bus speed default 500KBPS
};
```

##### Examples

###### Sending a CAN message

```cpp
CanMessage canMsg;

canMsg.identifier = 0x123;
canMsg.data_length_code = 8;
canMsg.data[0] = 0x01;
canMsg.data[1] = 0x02;
canMsg.data[2] = 0x03;
canMsg.data[3] = 0x04;
canMsg.data[4] = 0x05;
canMsg.data[5] = 0x06;
canMsg.data[6] = 0x0F;
canMsg.data[7] = 0x08;

sendMessage(canMsg);
```

###### Receiving a CAN message

```cpp
MCP2515::ConfigModule configModule;
MCP2515::Device node(configModule);
CanMessage canRes;
node.readMessage(canRes);
// node.readMessage(MCP2515::RXBn::RXB0, canRes); // To read from RXB0 or RXB1
printf("Can ID: %lx | Can DLC:%d (bytes)\n", canRes.identifier, canRes.data_length_code);

printf("Can DATA: [0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x]\n", canRes.data[0], canRes.data[1], canRes.data[2], canRes.data[3], canRes.data[4], canRes.data[5], 
canRes.data[6], canRes.data[7]);
```

This example will only work with two nodes connected to the same CAN bus. If you only have an ESP32 and an MCP2515, enable loopback mode with the method `setLoopbackMode()`.

#### Contributors

Samuel Henrique Guimarães Alencar <samuelhenriq12@gmail.com>
