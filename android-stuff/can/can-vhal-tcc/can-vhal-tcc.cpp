#include <android-base/logging.h>
#include <log/log.h>

#include <libnetdevice/can.h>
#include <libnetdevice/libnetdevice.h>

#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>

#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <android/binder_ibinder.h>

#include <memory>
#include <iostream>

#include <android/hardware/automotive/can/1.0/ICanBus.h> //CanMessage
#include <android/hardware/automotive/can/1.0/ICanController.h>
#include <android/hardware/automotive/can/1.0/ICanMessageListener.h>
#include <android/hardware/automotive/can/1.0/ICloseHandle.h>
#include <android/hardware/automotive/can/1.0/types.h>

#include <aidl/android/hardware/automotive/vehicle/BnVehicle.h>
#include <AidlVhalClient.h>

constexpr static char AIDL_VHAL_SERVICE[] = "android.hardware.automotive.vehicle.IVehicle/default";
constexpr static char TAG[] = "VHAL_SOCKET";

using ::android::frameworks::automotive::vhal::IVhalClient;
using ::android::sp;
using ::android::hardware::automotive::can::V1_0::ICanBus;
using ::android::hardware::automotive::can::V1_0::ICanController;
using ::android::hardware::automotive::can::V1_0::ICanMessageListener;
using ::android::hardware::automotive::can::V1_0::ICloseHandle;
using ::android::hardware::automotive::can::V1_0::CanMessage;
using ::android::hardware::automotive::can::V1_0::CanMessageFilter;

// using namespace android::hardware::automotive::can::V1_0::implementation;
class ListenCallback : public ICanMessageListener {
public:
    // Implemente a função de retorno da interface
    android::hardware::Return<void> onReceive(const CanMessage& message) override {
        // Implemente o código aqui para tratar as mensagens recebidas.
        ALOG(LOG_INFO, TAG, "Message received");
        LOG(INFO) << "Message received";
        std::cout << "Message received" << std::endl;
        std::cout << android::hardware::automotive::can::V1_0::toString(message) << std::endl;
        return android::hardware::Void();
    }
};

int main() {
    // Aqui eu consegui fazer sozinho com base no da eldorado
    CanMessage message;
    sp<ICanBus> can_bus = ICanBus::getService();
    sp<ICanController> can_controller = ICanController::getService();
    if (can_bus == nullptr || can_controller == nullptr) {
        ALOG(LOG_ERROR, TAG, "Failed to get CAN service");
        std::cout << "Failed to get CAN service" << std::endl;
        return 1;
    }
    //---------------

    // Aqui comecei a penar kkkk, recorri ao chat
    ICanController::BusConfig::InterfaceId::Socketcan socketcan_interface;
    socketcan_interface.ifname("can0");
    ICanController::BusConfig config = {
        .name = "can0",
        .interfaceId = ICanController::BusConfig::InterfaceId{},
        .bitrate = 500000,
    };
    config.interfaceId.socketcan(socketcan_interface);
    //---------------

    // Essa função upInterface eu consegui usar sozinho, só não sabia configurar o parametro config
    if (can_controller->upInterface(config) != ICanController::Result::OK) {
        ALOG(LOG_ERROR, TAG, "Failed to bring up CAN interface");
        std::cout << "Failed to bring up CAN interface" << std::endl;
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
    
    // ::android::hardware::hidl_vec<CanMessageFilter> filters;
    // sp<ICanMessageListener> listener = ICanMessageListener::tryGetService();
    // Agora a parte de callback eu não consegui fazer sozinho, recorri ao chat e o chat penooooou, passei umas 2h preso nisso
    sp<ICanMessageListener> listener = new ListenCallback();
    auto callback = [](android::hardware::automotive::can::V1_0::Result result, const android::sp<android::hardware::automotive::can::V1_0::ICloseHandle>& closeHandle) {
        // Implemente sua lógica aqui
        std::cout << "Callback" << std::endl;
        result = android::hardware::automotive::can::V1_0::Result::OK; // Só para compilar
        closeHandle.get(); // Só para compilar
    };

    can_bus->listen({}, listener, callback);
    //---------------
    while (1) {
        // bus->listen({}, listener, nullptr);
    }

    // ALOG(LOG_INFO, TAG, "Sending CAN message");
    // LOG(INFO) << "Sending CAN message";
    // std::cout << "Sending CAN message" << std::endl;



}