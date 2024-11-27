/**
 * @file sim_gpio_driver.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <fstream>

#include "gpio_driver.hpp"
#include "ara/log/log.h"
#include "core/curl/curl_driver.hpp"
#include "core/json/json_parser.h"

namespace simba {
namespace core {
namespace gpio {

GpioDriver::GpioDriver(): gpio_logger_{
    ara::log::LoggingMenager::GetInstance()->CreateLogger("GPIO", "", ara::log::LogLevel::kDebug)} {}

GpioDriver::~GpioDriver() {}

core::ErrorCode  GpioDriver::unregisterPin(const uint16_t& pinNumber) {
    ara::log::LogInfo() << "Unregister PIN with ID:" << pinNumber;
        // Przygotowanie danych w formacie JSON
    std::string data = "{\"action\":\"unregister\", \"pinNumber\":" + std::to_string(pinNumber) + "}";

    // Wywołanie funkcji CURL do wysłania danych JSON
    simba::curl::Curl::send_json_data("hardware/gpio", data);
    return core::ErrorCode::kOk;
}

core::ErrorCode GpioDriver::initializePin(const uint16_t& pinNumber, const direction_t& direction) {
    ara::log::LogInfo() << "Initialize PIN with ID:" << pinNumber
                        << " direction:" << ((direction == direction_t::IN) ? "IN" : "OUT");
    
    // Przygotowanie danych w formacie JSON
    std::string data = "{\"action\":\"initialize\", \"pinNumber\":" + std::to_string(pinNumber) + 
                   ", \"direction\":\"" + ((direction == direction_t::IN) ? "in" : "out") + "\"}";

    // Wywołanie funkcji CURL do wysłania danych JSON
    simba::curl::Curl::send_json_data("hardware/gpio", data);
    
    return core::ErrorCode::kOk;
}


std::string GpioDriver::getEndpointPath(const uint16_t& pinNumber, const std::string& endpoint) {
    return this->path+"/gpio"+std::to_string(pinNumber)+"/"+endpoint;
}

core::ErrorCode GpioDriver::setValue(const uint16_t& pinNumber, const uint8_t& value) {
    ara::log::LogInfo() << "SET VALUE PIN with ID:" << pinNumber
                        << " value:" << std::to_string(static_cast<uint16_t>(value));
    
    // Przygotowanie danych w formacie JSON
    std::string data = "{\"action\":\"setValue\", \"pinNumber\":" + std::to_string(pinNumber) + 
                       ", \"value\":" + std::to_string(value) + "}";

    // Wywołanie funkcji CURL do wysłania danych JSON
    simba::curl::Curl::send_json_data("hardware/gpio", data);
    
    return core::ErrorCode::kOk;
}

core::ErrorCode GpioDriver::setDirection(const uint16_t& pinNumber , const direction_t& direction) {
    ara::log::LogInfo() << "SET DIRECTION PIN with ID:" << pinNumber
    << "direction:" << ((direction == direction_t::IN) ? "IN" : "OUT");
    // Przygotowanie danych w formacie JSON
    std::string data = "{\"action\":\"setDirection\", \"pinNumber\":" + std::to_string(pinNumber) + ", \"direction\":" + ((direction == direction_t::IN) ? "in" : "out") + "}";

    // Wywołanie funkcji CURL do wysłania danych JSON
    simba::curl::Curl::send_json_data("hardware/gpio", data);
    return ErrorCode::kOk;
}

uint8_t GpioDriver::getValue(const uint16_t& pinNumber) {
    auto res_opt = simba::curl::Curl::get_json_data("hardware/gpio/value/"+std::to_string(pinNumber));
    if (!res_opt.has_value()) {
        return 2;
    }
    auto parser = json::JsonParser::Parser(res_opt.value());
    auto val_opt = parser->GetNumber<uint8_t>("value");
    if (!val_opt.has_value()) {
        return 2;
    }
    ara::log::LogInfo() << "GET VALUE PIN with ID:" << pinNumber
    << "VALUE:" << val_opt.value();
    return val_opt.value();
}

direction_t GpioDriver::getDirection(const uint16_t& pinNumber) {
        auto res_opt = simba::curl::Curl::get_json_data("hardware/gpio/direction/"+std::to_string(pinNumber));
    if (!res_opt.has_value()) {
        return direction_t::ERROR;
    }
    auto parser = json::JsonParser::Parser(res_opt.value());
    auto val_opt = parser->GetString("value");
    if (!val_opt.has_value()) {
        return direction_t::ERROR;
    }
    auto val = val_opt.value() == "in" ? direction_t::IN : direction_t::OUT;
    ara::log::LogInfo() << "GET VALUE PIN with ID:" << pinNumber
    << "VALUE:" << val_opt.value();
    return val;
}

}  // namespace gpio
}  // namespace core
}  // namespace simba
