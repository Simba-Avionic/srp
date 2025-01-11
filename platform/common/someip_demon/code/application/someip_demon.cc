/**
 * @file em_application.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "platform/common/someip_demon/code/application/someip_demon.h"

#include <map>
#include <memory>
#include <memory_resource>
#include <string>

#include "ara/log/log.h"
#include "core/common/condition.h"
#include "core/json/json_parser.h"
#include "platform/common/someip_demon/code/common/network_controller.h"
#include "platform/common/someip_demon/code/sd/sd_controller.h"
// #include "ara/com/someip/HeaderStructure.h"
// #include "ara/com/someip/ServiceEntry.h"
// #include "ara/com/someip/someip_frame.h"
// #include "ara/com/someip/EndpointOption.h"

namespace srp {
namespace someip_demon {

SomeIpApplication::SomeIpApplication(/* args */) : connector_list_{} {}

SomeIpApplication::~SomeIpApplication() {}

int SomeIpApplication::Run(const std::stop_token& token) {
  std::shared_ptr<someip::sd::SdController> sd_controller =
      std::make_shared<someip::sd::SdController>(multicast_controller_, sd_db_);
  std::shared_ptr<common::NetworkController> net_controller =
      std::make_shared<common::NetworkController>();
  net_controller->SetController(
      sd_controller, common::INetworkController::ControllerType::kSomeipSd);
  multicast_controller_->SetController(net_controller);
  multicast_controller_->Start();
  sd_controller->Start();
  for (auto& service : connector_list_) {
    service.second->Init();
  }
  core::condition::wait(token);
  for (auto& service : connector_list_) {
    service.second->DeInit();
  }
  sd_controller->Stop();
  multicast_controller_->Stop();
  return EXIT_SUCCESS;
}
/**
 * @brief This function is called to initialize the application
 *
 * @param parms map with parms
 */
int SomeIpApplication::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  const auto& json = core::json::JsonParser::Parser(
      std::string{"/srp/platform/etc/platform_config.json"});
  if (!json.has_value()) {
    ara::log::LogError() << "Json file with network config not exist";
    return EXIT_FAILURE;
  }
  const auto& platform_obj = json.value().GetObject("platform");
  if (!platform_obj.has_value()) {
    ara::log::LogError() << "Json not have root object";
    return EXIT_FAILURE;
  }

  const auto& ip = platform_obj.value().GetString("ip");
  if (!ip.has_value()) {
    ara::log::LogError() << "IP address not found";
    return EXIT_FAILURE;
  }
  const auto& someip_multicast_ip =
      platform_obj.value().GetString("someip_multicast_ip");
  if (!someip_multicast_ip.has_value()) {
    ara::log::LogError() << "someip_multicast_ip address not found";
    return EXIT_FAILURE;
  }
  const auto& someip_multicast_port =
      platform_obj.value().GetNumber<uint16_t>("someip_multicast_port");
  if (!someip_multicast_port.has_value()) {
    ara::log::LogError() << "someip_multicast_port not found";
    return EXIT_FAILURE;
  }

  const auto ip_ = ip.value();
  const auto someip_multicast_ip_ = someip_multicast_ip.value();
  multicast_controller_ = std::make_unique<common::com::MulticastController>(
      ip_, someip_multicast_ip_, someip_multicast_port.value());

  const auto& endpoint_json = core::json::JsonParser::Parser(
      parms.at("app_path") + "endpoint_config.json");
  if (!endpoint_json.has_value()) {
    ara::log::LogError() << "endpoint_config.json not exist";
    return EXIT_FAILURE;
  }
  const auto& port_list_opt =
      endpoint_json.value().GetArray<std::uint16_t>("udp");
  if (!port_list_opt.has_value()) {
    ara::log::LogError() << "UDP endpoint not exist";
    return EXIT_FAILURE;
  }
  const auto& udp_port_list_ = port_list_opt.value();

  for (const auto& port : udp_port_list_) {
    ara::log::LogInfo() << "New udp endpoint starting: " << port;
    if (!connector_list_
             .insert({port, std::make_shared<ServiceConnector>(ip.value(), port,
                                                               sd_db_)})
             .second) {
      ara::log::LogError() << "Port already used";
    }
  }
  return EXIT_SUCCESS;
}

}  // namespace someip_demon
}  // namespace srp
