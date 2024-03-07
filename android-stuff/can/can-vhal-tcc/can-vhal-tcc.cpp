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

#include <aidl/android/hardware/automotive/vehicle/BnVehicle.h>
#include <AidlVhalClient.h>

#include "SocketCan.h"
#define DEBUG_SOCKET_CAN
#include "Logging.h"

constexpr static char AIDL_VHAL_SERVICE[] = "android.hardware.automotive.vehicle.IVehicle/default";
constexpr static char TAG[] = "VHAL_SOCKET";

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

    while (1) {
        if(!socket_can.ReadCanMessage(frame)) {
            ALOG(LOG_ERROR, TAG, "Failed to read CAN message");
            LOG_CAN_ERROR(TAG, "Failed to read CAN message");
        } else {
            std::cout << std::setfill('0') << std::setw(3) << std::uppercase << std::hex << frame.can_id;
            std::cout << "\t[" << static_cast<int>(frame.can_dlc) << "]\t";
            for (int i = 0; i < frame.can_dlc; i++) {
                std::cout << std::setfill('0') << std::setw(2) << static_cast<int>(frame.data[i]);
                (i < frame.can_dlc - 1) ? std::cout << " " : std::cout << "";
            }
            std::cout << std::endl;
        }

    }

    // ALOG(LOG_INFO, TAG, "Sending CAN message");
    // LOG(INFO) << "Sending CAN message";
    // std::cout << "Sending CAN message" << std::endl;
}