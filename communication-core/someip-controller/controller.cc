/**
 * @file controller.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "communication-core/someip-controller/controller.h"

#include "communication-core/sockets/ipc_socket.h"
#include "communication-core/sockets/udp_socket.h"
#include "communication-core/someip/factory/someip_header_factory.h"
#include "communication-core/someip/factory/someip_message_factory.h"
#include "core/common/error_code.h"
#include "core/logger/Logger.h"

namespace srp {
namespace com {
namespace someip {
void Controller::DropTransferCallback(const uint16_t transfer_id) noexcept {
  std::unique_lock lk{transfer_mutex};
  std::remove_if(
      this->transfer_list.begin(), this->transfer_list.end(),
      [&transfer_id](const std::pair<uint16_t, ResultCallback>& obj) {
        return obj.first == transfer_id;
      });
}

void Controller::FindServiceCallback(
    const objects::Endpoint& endpoint,
    FindServiceCallbackResult res_callback) noexcept {
  AppLogger::Info("[ SOMEIP CONTROLLER ]: Find Service start (" +
                  std::to_string(endpoint.GetServiceId()) + ")");
  auto obj = db->FindService(endpoint.GetServiceId());
  if (obj.has_value()) {
    res_callback(obj.value());
  }
}

uint16_t Controller::SendFrameCallback(const std::vector<uint8_t> payload,
                                       const objects::Endpoint endpoint,
                                       const objects::Interface interface,
                                       data::MessageType type,
                                       ResultCallback result) noexcept {
  if (type == data::MessageType::kRequest) {
    const uint16_t trans_id = this->GetTransferID();
    auto header = header_factory->CreateRequest(endpoint.GetServiceId(),
                                                endpoint.GetEndpointId());
    auto msg =
        msg_factory->GetBuffor(header, this->service_id, trans_id, payload);
    if (Transfer(std::move(interface), std::move(msg))) {
      std::unique_lock lk{transfer_mutex};
      this->transfer_list.push_back({trans_id, std::move(result)});
      return trans_id;
    }
  } else if (type == data::MessageType::kNotification) {
    this->event_queue.push({std::move(endpoint), std::move(payload)});
  }
  return 0;
}
void Controller::SendEvent(const objects::Endpoint endpoint,
                           const uint16_t client, std::vector<uint8_t> data) {
  auto interface = db->FindService(client);
  if (interface.has_value()) {
    auto header =
        header_factory->CreatEvent(this->service_id, endpoint.GetEndpointId());
    auto msg = msg_factory->GetBuffor(std::move(header), client, 0, data);
    this->Transfer(interface.value(), std::move(msg));
  } else {
    AppLogger::Error("[ SOMEIP CONTROLLER ]: Can't find service " +
                     std::to_string(client));
  }
}
void Controller::SendEventLoop(const objects::Endpoint endpoint,
                               std::vector<uint8_t> data) {
  const auto r_list = db->FindEventClient(endpoint.GetEndpointId());
  if (r_list.has_value()) {
    const auto list = r_list.value();
    for (const auto& item : list) {
      this->SendEvent(endpoint, item, data);
    }
  }
}
void Controller::SubscribeCallback(const objects::Endpoint endpoint,
                                   const objects::Interface interface,
                                   ResultCallback result) noexcept {
  const uint32_t id = (static_cast<uint32_t>(endpoint.GetServiceId()) << 16) +
                      static_cast<uint32_t>(endpoint.GetEndpointId());
  this->event_callback.insert({id, std::move(result)});
  AppLogger::Info("Subscribed to " + std::to_string(endpoint.GetServiceId()) +
                  ":" + std::to_string(endpoint.GetEndpointId()));
}

void Controller::RXCallback(const std::string& ip, const std::uint16_t& port,
                            std::vector<std::uint8_t> data) noexcept {
  const auto header = msg_factory->GetHeader(data);
  if (header->GetServiceID() == this->service_id) {
    if (header->GetMessageType() == data::MessageType::kRequest) {
      auto payload = msg_factory->GetPayload(data);
      this->onRequest(std::move(header), std::move(payload));
      return;
    }

  } else if (header->GetMessageType() == data::MessageType::kResponse &&
             header->GetClientID() == this->service_id) {
    const auto payload = msg_factory->GetPayload(data);
    this->onResult(std::move(header), std::move(payload));
    return;
  } else if (header->GetMessageType() == data::MessageType::kNotification &&
             header->GetClientID() == this->service_id) {
    const auto payload = msg_factory->GetPayload(data);
    this->onEvent(std::move(header), std::move(payload));
    return;
  } else if (header->GetMessageType() == data::MessageType::kError &&
             header->GetClientID() == this->service_id) {
    this->onError(std::move(header));
    return;
  } else if (header->GetMessageType() == data::MessageType::kRequest) {
    AppLogger::Error("[ SOMEIP CONTROLLER ]: Wrong service ID");
    SendError(header, data::MessageCode::kEUnknownService);
  }
}

void Controller::onError(std::shared_ptr<SomeIpHeader> header) {
  const auto trans_id = header->GetSessionID();
  ResultCallback callback;
  bool exist{false};
  {
    std::unique_lock lk{transfer_mutex};
    auto cal = std::find_if(
        this->transfer_list.begin(), this->transfer_list.end(),
        [&trans_id](const std::pair<uint16_t, ResultCallback>& obj) {
          return obj.first == trans_id;
        });
    if (cal != this->transfer_list.end()) {
      callback = (cal->second);
      exist = true;
    }
  }
  if (exist) {
    callback({}, static_cast<data::MessageCode>(header->GetReturnCode()),
             trans_id);
    std::unique_lock lk{transfer_mutex};
    std::remove_if(this->transfer_list.begin(), this->transfer_list.end(),
                   [&trans_id](const std::pair<uint16_t, ResultCallback>& obj) {
                     return obj.first == trans_id;
                   });
  }
}
void Controller::onRequest(std::shared_ptr<SomeIpHeader> header,
                           const std::vector<uint8_t> payload) {
  const uint16_t s_id =
      (header->GetServiceID() == service_id) ? 0 : header->GetServiceID();
  const uint32_t id = (static_cast<uint32_t>(s_id) << 16) +
                      static_cast<uint32_t>(header->GetMethodID());
  const auto func = this->callback.find(id);
  if (func == this->callback.end()) {
    AppLogger::Error("[ SOMEIP CONTROLLER ]: Method not found!");
    SendError(header, data::MessageCode::kEUnknownMethod);
    return;
  } else {
    AppLogger::Info("[ SOMEIP CONTROLLER ]: Callback found!");
    auto data = func->second(std::move(payload),
                             objects::Endpoint{header->GetClientID(), 0});
    if (data.first == data::MessageCode::kEOk) {
      if (data.second.has_value()) {
        auto res_header = this->header_factory->CreateResponse(
            header->GetServiceID(), header->GetMethodID(),
            data::MessageCode::kEOk);
        auto msg =
            msg_factory->GetBuffor(res_header, header->GetClientID(),
                                   header->GetSessionID(), data.second.value());
        auto interf = db->FindService(header->GetClientID());
        if (interf.has_value()) {
          Transfer(interf.value(), msg);
        } else {
          AppLogger::Error("[ SOMEIP CONTROLLER ]: can't find service: " +
                           std::to_string(header->GetClientID()));
        }
      }
    } else {
      SendError(header, data.first);
    }
  }
}
void Controller::SendError(std::shared_ptr<SomeIpHeader> rx_header,
                           data::MessageCode error_code) {
  auto res_header = this->header_factory->CreateErrorResponse(
      rx_header->GetServiceID(), rx_header->GetMethodID(), error_code);
  auto msg = msg_factory->GetBuffor(res_header, rx_header->GetClientID(),
                                    rx_header->GetSessionID(), {});
  auto interf = db->FindService(rx_header->GetClientID());
  if (interf.has_value()) {
    Transfer(interf.value(), msg);
  } else {
    AppLogger::Error("[ SOMEIP CONTROLLER ]: can't find service: " +
                     std::to_string(rx_header->GetClientID()));
  }
}
void Controller::onResult(std::shared_ptr<SomeIpHeader> header,
                          const std::vector<uint8_t> payload) {
  const auto trans_id = header->GetSessionID();
  ResultCallback callback;
  bool exist{false};
  {
    std::unique_lock lk{transfer_mutex};
    auto cal = std::find_if(
        this->transfer_list.begin(), this->transfer_list.end(),
        [&trans_id](const std::pair<uint16_t, ResultCallback>& obj) {
          return obj.first == trans_id;
        });
    if (cal != this->transfer_list.end()) {
      callback = (cal->second);
      exist = true;
    }
  }
  if (exist) {
    callback(std::move(payload),
             static_cast<data::MessageCode>(header->GetReturnCode()), trans_id);
    std::unique_lock lk{transfer_mutex};
    std::remove_if(this->transfer_list.begin(), this->transfer_list.end(),
                   [&trans_id](const std::pair<uint16_t, ResultCallback>& obj) {
                     return obj.first == trans_id;
                   });
  }
}

void Controller::onEvent(std::shared_ptr<SomeIpHeader> header,
                         const std::vector<uint8_t> payload) {
  const uint32_t id = (static_cast<uint32_t>(header->GetServiceID()) << 16) +
                      static_cast<uint32_t>(header->GetMethodID());
  const auto func = this->event_callback.find(id);
  if (func == this->event_callback.end()) {
    AppLogger::Error("[ SOMEIP CONTROLLER ]: Event not subscribed!");
    return;
  }

  func->second(std::move(payload),
               static_cast<data::MessageCode>(header->GetReturnCode()), 0);
}

Controller::Controller(std::shared_ptr<objects::DataBase> db_)
    : db{std::move(db_)}, service_id{db->GetServiceId()} {
  const auto interfaces = db->GetInterfaces();
  for (const auto& inter : interfaces) {
    AppLogger::Info("[ SOMEIP CONTROLLER ]: Interface added - " +
                    inter.GetIp() + ":" + std::to_string(inter.GetPort()));
    if (inter.GetPort() == 0) {
      ipc_soc_ = std::make_unique<com::soc::IpcSocket>();
      ipc_soc_->Init(soc::SocketConfig{inter.GetIp(), inter.GetPort(), 0});
      ipc_soc_->SetRXCallback(
          std::bind(&Controller::RXCallback, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3));
      ipc_soc_->StartRXThread();
      AppLogger::Info("[ SOMEIP CONTROLLER ]: IPC soc started");
    } else {
      udp_soc_ = std::make_unique<com::soc::UdpSocket>();
      udp_soc_->Init(soc::SocketConfig{inter.GetIp(), inter.GetPort(), 0});
      udp_soc_->SetRXCallback(
          std::bind(&Controller::RXCallback, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3));
      udp_soc_->StartRXThread();
      AppLogger::Info("[ SOMEIP CONTROLLER ]: UDP soc started on port: " +
                      std::to_string(inter.GetPort()));
    }

    header_factory = std::make_unique<someip::factory::SomeIpHeaderFactory>();
    msg_factory = std::make_unique<someip::factory::SomeIpMessageFactory>();
  }
}

bool Controller::Transfer(const objects::Interface interface,
                          const std::vector<uint8_t>& payload) {
  if (interface.GetPort() == 0) {
    return this->SendIpcMsg(std::move(interface), std::move(payload));
  } else {
    return this->SendUdpMsg(std::move(interface), std::move(payload));
  }
}

bool Controller::SendIpcMsg(const objects::Interface interface,
                            const std::vector<uint8_t>& payload) {
  if (this->ipc_soc_) {
    std::unique_lock lk{ipc_soc_mutex};
    return ipc_soc_->Transmit(interface.GetIp(), 0, std::move(payload)) ==
           srp::core::ErrorCode::kOk;
  }
  return false;
}
bool Controller::SendUdpMsg(const objects::Interface interface,
                            const std::vector<uint8_t>& payload) {
  if (this->udp_soc_) {
    std::unique_lock lk{udp_soc_mutex};
    return udp_soc_->Transmit(interface.GetIp(), interface.GetPort(),
                              std::move(payload)) ==
           srp::core::ErrorCode::kOk;
  }
  return false;
}
void Controller::SendAck(const uint16_t client_id, const uint16_t endpoint_id,
                         const uint16_t trans_id) {
  auto header =
      this->header_factory->CreateRequestACK(this->service_id, endpoint_id);

  auto msg = this->msg_factory->GetBuffor(header, client_id, trans_id, {});
  auto interf = db->FindService(client_id);
  if (interf.has_value()) {
    Transfer(interf.value(), msg);
  } else {
    AppLogger::Error("[ SOMEIP CONTROLLER ]: can't find service: " +
                     std::to_string(client_id));
  }
}
uint16_t Controller::GetTransferID() {
  ++this->transfer_id;
  if (transfer_id == 0) {
    ++transfer_id;
  }
  return transfer_id;
}

void Controller::Add(std::shared_ptr<ISkeleton> skeleton) {
  auto endpoint = skeleton->GetEndPoint();
  if (endpoint.has_value()) {
    if (endpoint.value().GetEndpointId() < 0x8000) {
      uint32_t id =
          (static_cast<uint32_t>(endpoint.value().GetServiceId()) << 16) +
          endpoint.value().GetEndpointId();
      this->callback.insert(
          {id, std::bind(&ISkeleton::Call, skeleton.get(),
                         std::placeholders::_1, std::placeholders::_2)});
      AppLogger::Info("[ SOMEIP CONTROLLER ]: Method skeleton added (" +
                      skeleton->GetName() + ")");
    } else {
      if (!this->event_thread) {
        this->event_thread =
            std::make_unique<std::thread>([this]() { this->ThreadLoop(); });
      }
      skeleton->SetCallback(
          std::bind(&Controller::SendFrameCallback, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3,
                    std::placeholders::_4, std::placeholders::_5));
      AppLogger::Info("[ SOMEIP CONTROLLER ]: Event skeleton added (" +
                      skeleton->GetName() + ")");
    }
  }
}
void Controller::ThreadLoop() {
  while (true) {
    auto event = this->event_queue.Get();
    this->SendEventLoop(std::move(event.first), std::move(event.second));
  }
}
void Controller::Add(std::shared_ptr<IProxy> proxy) {
  AppLogger::Info("[ SOMEIP CONTROLLER ]: Proxy added (" + proxy->GetName() +
                  ")");
  proxy->SetSendCallback(
      std::bind(&Controller::SendFrameCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5),
      std::bind(&Controller::FindServiceCallback, this, std::placeholders::_1,
                std::placeholders::_2),
      std::bind(&Controller::DropTransferCallback, this, std::placeholders::_1),
      std::bind(&Controller::SubscribeCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
}

}  // namespace someip
}  // namespace com
}  // namespace srp
