/**
 * @file uds_server.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-04-28
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "apps/diag_ota/code/application/services/uds_server.h"

#include <functional>
#include <iomanip>
#include <map>
#include <memory>
#include <strstream>

#include "apps/diag_ota/code/application/services/uds_nrc_codes.h"
#include "apps/diag_ota/code/application/services/uds_request.h"
#include "apps/diag_ota/code/application/services/validator_controller.h"
#include "communication-core/sockets/stream_ipc_socket.h"
#include "core/logger/Logger.h"

namespace simba {
namespace diag_ota {
namespace uds {
namespace {
std::string ParseToServiceAdress(const uint8_t service_id,
                                 const uint8_t sub_service_id_msb,
                                 const uint8_t sub_service_id_lsb) {
  uint64_t res{0U};
  res |= static_cast<uint64_t>(service_id) << 16;
  res |= static_cast<uint64_t>(sub_service_id_msb) << 8;
  res |= static_cast<uint64_t>(sub_service_id_lsb);
  return std::to_string(res);
}

const std::map<std::uint8_t, std::function<std::string(const UdsRequest&)>>
    service_adress_generator{
        {0x22,
         [](const UdsRequest& req) {
           return ParseToServiceAdress(0x22, req.GetPayload()[0],
                                       req.GetPayload()[1]);
         }},
        {0x2E, [](const UdsRequest& req) {
           return ParseToServiceAdress(0x22, req.GetPayload()[0],
                                       req.GetPayload()[1]);
         }}};
}  // namespace

UdsServer::UdsServer(const uint16_t& logical_address, const std::string& vin)
    : logical_address_{logical_address},
      vin_{vin},
      validator{std::make_unique<ValidatorController>()},
      soc_{std::make_unique<com::soc::StreamIpcSocket>()} {}

UdsServer::~UdsServer() {}

/**
 * Is called when the doip library receives a diagnostic message.
 * @param address   logical address of the ecu
 * @param data      message which was received
 * @param length    length of the message
 */

void UdsServer::ReceiveFromLibrary(uint16_t address, unsigned char* data,
                                   int length) {
  // std::strstream dyn_s;
  // dyn_s << "[DoIP server]: DoIP Message received from 0x" << std::hex <<
  // address
  //       << ": ";
  // for (int i = 0; i < length; i++) {
  //   dyn_s << std::hex << (int)data[i] << " ";  // NOLINT
  // }
  const auto req = uds::UdsRequestFactory::ParseRequest(data, length);
  if (!req.has_value()) {
    AppLogger::Error("[DoIP server]: Request dropped due to parsing error");
    unsigned char responseData[] = {0x7F, 0x00,
                                    uds::UdsNrcCodes::kGeneralReject};
    connection->sendDiagnosticPayload(logical_address_, responseData,
                                      sizeof(responseData));
    return;
  }
  /// TODO: Remove this log and add in doip lib
  {
    std::stringstream stream;
    stream << "[DoIP server] rx: 0x" << std::hex
           << static_cast<int>(req.value().GetServiceId()) << "_";
    for (const auto& bits : req.value().GetPayload()) {
      stream << std::hex << static_cast<int>(bits) << "_";
    }
    auto ll = stream.str();
    ll.pop_back();
    AppLogger::Info(ll);
  }
  if (validator) {
    const auto valid_status = validator->Validate(req.value());
    if (valid_status == uds::UdsNrcCodes::kOk) {
      unsigned char responseData[] = {
          0x7F, req.value().GetServiceId(),
          uds::UdsNrcCodes::kRequestReceivedResponsePending};
      connection->sendDiagnosticPayload(logical_address_, responseData,
                                        sizeof(responseData));
      const auto parser =
          service_adress_generator.find(req.value().GetServiceId());
      if (parser != service_adress_generator.cend()) {
        const auto adress = parser->second(req.value());
        const auto payload =
            uds::UdsRequestFactory::ParseRequestToLocalMSG(req.value());
        AppLogger::Debug("[DoIP server]: Sending to: "+adress);
        const auto res = soc_->Transmit("SIMBA.DIAG."+adress, 0x00, payload);

        if (res.has_value()) {
          /// TODO: Remove this log and add in doip lib
            {
              std::stringstream stream;
              stream << "[DoIP server] tx: 0x";
              for (const auto& bits : res.value()) {
                stream << std::hex << static_cast<int>(bits) << "_";
              }
              auto ll = stream.str();
              ll.pop_back();
              AppLogger::Info(ll);
            }
          auto temp = res.value();
          std::vector<unsigned char>& res_vec = reinterpret_cast<std::vector<unsigned char>&>(temp);
          connection->sendDiagnosticPayload(logical_address_, res_vec.data(),
                                            res_vec.size());
        } else {
          AppLogger::Error("[DoIP server]: Internal error");
          unsigned char responseData[] = {0x7F, req.value().GetServiceId(),
                                          uds::UdsNrcCodes::kGeneralReject};
          connection->sendDiagnosticPayload(logical_address_, responseData,
                                            sizeof(responseData));
        }

      } else {
        AppLogger::Error("[DoIP server]: Service not support");
        unsigned char responseData[] = {0x7F, req.value().GetServiceId(),
                                        valid_status};
        connection->sendDiagnosticPayload(logical_address_, responseData,
                                          sizeof(responseData));
        return;
      }
    } else {
      AppLogger::Error("[DoIP server]: Validation Failed(" +
                       std::to_string(valid_status) + ")");
      unsigned char responseData[] = {0x7F, req.value().GetServiceId(),
                                      valid_status};
      connection->sendDiagnosticPayload(logical_address_, responseData,
                                        sizeof(responseData));
      return;
    }
  } else {
    AppLogger::Error("[DoIP server]: Request dropped by missing validator");
    unsigned char responseData[] = {0x7F, data[0],
                                    uds::UdsNrcCodes::kGeneralReject};
    connection->sendDiagnosticPayload(logical_address_, responseData,
                                      sizeof(responseData));
  }
}

/**
 * Will be called when the doip library receives a diagnostic message.
 * The library notifies the application about the message.
 * Checks if there is a ecu with the logical address
 * @param targetAddress     logical address to the ecu
 * @return                  If a positive or negative ACK should be send to
 * the client
 */
bool UdsServer::DiagnosticMessageReceived(uint16_t targetAddress) {
  (void)targetAddress;
  unsigned char ackCode;

  AppLogger::Info("Received Diagnostic message");

  // send positiv ack
  ackCode = 0x00;
  AppLogger::Info("-> Send positive diagnostic message ack");
  connection->sendDiagnosticAck(logical_address_, true, ackCode);

  return true;
}

/**
 * Closes the connection of the server by ending the listener threads
 */
void UdsServer::CloseConnection() {
  AppLogger::Info("Connection closed");
  // serverActive = false;
}

/*
 * Check permantly if udp message was received
 */
void UdsServer::listenUdp() {
  AppLogger::Info("UDP started");
  while (serverActive) {
    server.receiveUdpMessage();
  }
}

/*
 * Check permantly if tcp message was received
 */
void UdsServer::listenTcp() {
  server.setupTcpSocket();
  AppLogger::Info("TCP started");
  while (true) {
    this->connection = server.waitForTcpConnection();
    this->connection->setCallback(
        std::bind(&UdsServer::ReceiveFromLibrary, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3),
        std::bind(&UdsServer::DiagnosticMessageReceived, this,
                  std::placeholders::_1),
        std::bind(&UdsServer::CloseConnection, this));
    this->connection->setGeneralInactivityTime(50000);

    while (this->connection->isSocketActive()) {
      this->connection->receiveTcpMessage();
    }
  }
}
void UdsServer::ConfigureDoipServer() {
  // VIN needs to have a fixed length of 17 bytes.
  // Shorter VINs will be padded with '0'
  server.setVIN(vin_);
  server.setLogicalGatewayAddress(logical_address_);
  server.setGID(0);
  server.setFAR(0);
  server.setEID(1);

  // doipserver->setA_DoIP_Announce_Num(tempNum);
  // doipserver->setA_DoIP_Announce_Interval(tempInterval);
}
void UdsServer::Start() {
  ConfigureDoipServer();

  server.setupUdpSocket();

  serverActive = true;
  doipReceiver.push_back(std::thread(&UdsServer::listenUdp, this));
  doipReceiver.push_back(std::thread(&UdsServer::listenTcp, this));
  server.setA_DoIP_Announce_Num(4);
  server.sendVehicleAnnouncement();
}

}  // namespace uds
}  // namespace diag_ota
}  // namespace simba
