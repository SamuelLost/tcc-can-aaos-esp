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

#include <aidl/android/hardware/automotive/vehicle/VehicleProperty.h>
#include <VehicleUtils.h>
#include <aidl/android/hardware/automotive/vehicle/BnVehicle.h>
#include <AidlVhalClient.h>

#define DEBUG_SOCKET_CAN
#include "logging.h"
#include "socket_can.h"

constexpr static char AIDL_VHAL_SERVICE[] = "android.hardware.automotive.vehicle.IVehicle/default";
constexpr static char TAG[] = "VHAL_SOCKET";

constexpr static int TEMPERATURE_CAN_ID = 0x124;
constexpr static int ACCELEROMETER_CAN_ID = 0x123;

using ::aidl::android::hardware::automotive::vehicle::VehicleProperty;
using ::android::hardware::automotive::vehicle::toInt;

// Transforma um array de bytes em um float
float BytesToFloat(uint8_t *bytes) {
    float f;
    std::memcpy(&f, bytes, sizeof(float));
    return f;
}

using ::android::frameworks::automotive::vhal::IVhalClient;
using ::android::sp;

int main() {
    
    tcc::aaos::can::SocketCan socket_can("can0");
    // Inicialização do socket CAN
    if(!socket_can.Init()) {
        ALOG(LOG_ERROR, TAG, "Failed to initialize socket CAN");
        std::cout << "Failed to initialize socket CAN" << std::endl;
        return 1;
    }

    ALOG(LOG_VERBOSE, TAG, "Socket CAN initialized");
    std::cout << "Socket CAN initialized" << std::endl;

    // Criação do cliente VHAL
    std::shared_ptr<IVhalClient> vhal_client = IVhalClient::tryCreateAidlClient(AIDL_VHAL_SERVICE);
    if (vhal_client == nullptr) {
        ALOG(LOG_ERROR, TAG, "Failed to create VHAL client");
        std::cout << "Failed to create VHAL client" << std::endl;
        return 1;
    }

    ALOG(LOG_VERBOSE, TAG, "VHAL client created");
    std::cout << "VHAL client created" << std::endl;

    struct can_frame frame;

    // Criação dos ponteiros para os valores das propriedades do VHAL
    auto acc_axes = vhal_client->createHalPropValue(toInt(VehicleProperty::INFO_ACCELEROMETER_MPU6050));
    auto acc_fault = vhal_client->createHalPropValue(toInt(VehicleProperty::FAULT_CODE_ACCELEROMETER_MPU6050));
    auto temp = vhal_client->createHalPropValue(toInt(VehicleProperty::INFO_TEMPERATURE_DHT22));
    auto temp_fault = vhal_client->createHalPropValue(toInt(VehicleProperty::FAULT_CODE_TEMPERATURE_DHT22));

    if (acc_axes == nullptr || acc_fault == nullptr || temp == nullptr || temp_fault == nullptr) {
        ALOG(LOG_ERROR, TAG, "Failed to create HAL property value");
        std::cout << "Failed to create HAL property value" << std::endl;
        return 1;
    }

    int16_t axis_x = 0, axis_y = 0, axis_z = 0;
    float temperature = 0;

    while (1) {

        if(!socket_can.ReadCanMessage(frame)) {
            ALOG(LOG_ERROR, TAG, "Failed to read CAN message");
            std::cout << "Failed to read CAN message" << std::endl;
        } else {
            switch (frame.can_id) {
                case ACCELEROMETER_CAN_ID: {
                    axis_x = (frame.data[0] << 8) | frame.data[1];
                    axis_y = (frame.data[2] << 8) | frame.data[3];
                    axis_z = (frame.data[4] << 8) | frame.data[5];

                    std::cout << std::dec << "Axis X: " << axis_x 
                                << ", Axis Y: " << axis_y << ", Axis Z: " << axis_z << std::endl;

                    acc_axes->setInt32Values({axis_x, axis_y, axis_z});
                    vhal_client->setValueSync(*acc_axes);

                    if(axis_x == INT16_MIN && axis_y == INT16_MIN && axis_z == INT16_MIN) {
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
