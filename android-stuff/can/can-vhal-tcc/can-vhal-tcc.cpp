#include <android-base/logging.h>
#include <log/log.h>

#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>

#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <android/binder_ibinder.h>

#include <memory>
#include <iostream>
#include <iomanip>
#include <endian.h>

#include <aidl/android/hardware/automotive/vehicle/BnVehicle.h>
#include <AidlVhalClient.h>

#include "SocketCan.h"
#define DEBUG_SOCKET_CAN
#include "Logging.h"
#include <byteswap.h>

constexpr static char AIDL_VHAL_SERVICE[] = "android.hardware.automotive.vehicle.IVehicle/default";
constexpr static char TAG[] = "VHAL_SOCKET";

constexpr static int TEMPERATURE_CAN_ID = 0x124;
constexpr static int PROP_INFO_TEMPERATURE = 291508224;
constexpr static int PROP_FAULT_CODE_TEMPERATURE = 286265345;

constexpr static int ACCELEROMETER_CAN_ID = 0x123;
constexpr static int PROP_INFO_ACCELEROMETER = 289476610;
constexpr static int PROP_FAULT_CODE_ACCELEROMETER = 286265347;

// 10.0 ºC = 0x00002041 -> Works
// -273.15 ºC = 0x339388c30000 -> Works
float BytesToFloat(uint8_t *bytes) {
    //Funcionou
    // float f;
    // uint32_t value = __builtin_bswap32(*(uint32_t *)bytes); // RPI4B é little endian. Essa função converte para big endian
    // value = htobe32(value); // Converte host byte order para big endian
    // std::memcpy(&f, &value, sizeof(float));
    // return f;

    //Funcionou também
    float f;
    std::memcpy(&f, bytes, sizeof(float));
    return f;
}

using ::android::frameworks::automotive::vhal::IVhalClient;
using ::android::sp;

int main() {
    
    tcc::aaos::can::SocketCan socket_can;
    if(!socket_can.Init()) {
        ALOG(LOG_ERROR, TAG, "Failed to initialize CAN interface");
        std::cout << "Failed to initialize CAN interface" << std::endl;
        return 1;
    }

    ALOG(LOG_VERBOSE, TAG, "CAN interface up");
    std::cout << "CAN interface up" << std::endl;

    LOG(VERBOSE) << "Starting CAN HAL";
    ALOG(LOG_VERBOSE, TAG, "Starting CAN HAL");

    // Aqui já tinha feito antes no estágio
    std::shared_ptr<IVhalClient> vhal_client = IVhalClient::tryCreateAidlClient(AIDL_VHAL_SERVICE);
    if (vhal_client == nullptr) {
        LOG(ERROR) << "Failed to create VHAL client";
        ALOG(LOG_ERROR, TAG, "Failed to create VHAL client");
        std::cout << "Failed to create VHAL client" << std::endl;
        return 1;
    }

    ALOG(LOG_VERBOSE, TAG, "VHAL client created");
    std::cout << "VHAL client created" << std::endl;

    if (!vhal_client->isAidlVhal()) {
        ALOG(LOG_VERBOSE, TAG, "VHAL client is NOT AIDL");
        std::cout << "VHAL client is NOT AIDL" << std::endl;
        return 1;
    }

    ALOG(LOG_VERBOSE, TAG, "VHAL client is AIDL");
    std::cout << "VHAL client is AIDL" << std::endl;

    struct can_frame frame;

    auto acc_axes = vhal_client->createHalPropValue(PROP_INFO_ACCELEROMETER);
    auto acc_fault = vhal_client->createHalPropValue(PROP_FAULT_CODE_ACCELEROMETER);
    auto temp = vhal_client->createHalPropValue(PROP_INFO_TEMPERATURE);
    auto temp_fault = vhal_client->createHalPropValue(PROP_FAULT_CODE_TEMPERATURE);

    if (acc_axes == nullptr || acc_fault == nullptr || temp == nullptr || temp_fault == nullptr) {
        ALOG(LOG_ERROR, TAG, "Failed to create HAL property value");
        std::cout << "Failed to create HAL property value" << std::endl;
        return 1;
    }

    int16_t axis_x = 0;
    int16_t axis_y = 0;
    int16_t axis_z = 0;
    float temperature = 0;

    while (1) {

        if(!socket_can.ReadCanMessage(frame)) {
            ALOG(LOG_ERROR, TAG, "Failed to read CAN message");
            LOG_CAN_ERROR(TAG, "Failed to read CAN message");
        } else {
            // std::cout << "0x" << std::setfill('0') << std::setw(3) << std::uppercase << std::hex << frame.can_id;
            // std::cout << "\t[" << static_cast<int>(frame.can_dlc) << "]\t";
            // for (int i = 0; i < frame.can_dlc; i++) {
            //     std::cout << std::setfill('0') << std::setw(2) << static_cast<int>(frame.data[i]);
            //     (i < frame.can_dlc - 1) ? std::cout << " " : std::cout << "";
            // }
            // std::cout << std::endl;

            switch (frame.can_id) {
                case ACCELEROMETER_CAN_ID: {
                    axis_x = (frame.data[0] << 8) | frame.data[1];
                    axis_y = (frame.data[2] << 8) | frame.data[3];
                    axis_z = (frame.data[4] << 8) | frame.data[5];
                    ALOG(LOG_INFO, TAG, "Axis X: %d, Axis Y: %d", axis_x, axis_y);
                    std::cout << std::dec << "Axis X: " << axis_x << ", Axis Y: " << axis_y << ", Axis Z: " << axis_z << std::endl;
                    acc_axes->setInt32Values({axis_x, axis_y, axis_z});
                    vhal_client->setValueSync(*acc_axes);

                    if(axis_x == 0 && axis_y == 0 && axis_z == 0) {
                        acc_fault->setStringValue("ACC-E1");
                        vhal_client->setValueSync(*acc_fault);
                    } else {
                        acc_fault->setStringValue("ACC-0");
                        vhal_client->setValueSync(*acc_fault);
                    }
                    break;
                }
                case TEMPERATURE_CAN_ID: {
                    temperature = BytesToFloat(frame.data);
                    ALOG(LOG_INFO, TAG, "Temperature: %f", temperature);
                    std::cout << std::dec << "Temperature: " << temperature << std::endl;
                    temp->setFloatValues({temperature});
                    vhal_client->setValueSync(*temp);
                    if (temperature <= -273) {
                        temp_fault->setStringValue("TMP-E1");
                        vhal_client->setValueSync(*temp_fault);
                    } else {
                        temp_fault->setStringValue("TMP-0");
                        vhal_client->setValueSync(*temp_fault);
                    }
                    break;
                }
                default:
                    break;
            }
        }

    }
}