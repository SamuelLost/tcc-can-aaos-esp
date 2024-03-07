#include <iostream>
#include <string>

#ifdef DEBUG_SOCKET_CAN
    #define LOG_CAN(tag, msg) std::cout << __FILE__ << ":" << __LINE__ << " " << tag << ": " << msg << std::endl
    #define LOG_CAN_ERROR(tag, msg) std::cerr << __FILE__ << ":" << __LINE__ << " " << tag << ": " << msg << std::endl
#else
    #define LOG_CAN(tag, msg)
    #define LOG_CAN_ERROR(tag, msg)
#endif