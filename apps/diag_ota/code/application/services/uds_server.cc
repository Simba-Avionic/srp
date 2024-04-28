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

#include <iomanip>
#include <strstream>

#include "apps/diag_ota/code/application/services/uds_nrc_codes.h"
#include "apps/diag_ota/code/application/services/uds_request.h"
#include "core/logger/Logger.h"
namespace simba {
namespace diag_ota {
namespace uds {

UdsServer::UdsServer(const uint16_t& logical_address, const std::string& vin)
    : logical_address_{logical_address}, vin_{vin} {}

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
  if (validator) {
    const auto valid_status = validator->Validate(req.value());
    if (valid_status == uds::UdsNrcCodes::kOk) {
      unsigned char responseData[] = {
          0x7F, req.value().GetServiceId(),
          uds::UdsNrcCodes::kRequestReceivedResponsePending};
      connection->sendDiagnosticPayload(logical_address_, responseData,
                                        sizeof(responseData));
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

  // AppLogger::Info(dyn_s.str());
  // if (length > 2 && data[0] == 0x22) {
  //   AppLogger::Info("-> Send diagnostic message positive response");
  //   unsigned char responseData[] = {0x62, data[1], data[2], 0x01,
  //                                   0x02, 0x03,    0x04};
  //   connection->sendDiagnosticPayload(logical_address_, responseData,
  //                                     sizeof(responseData));
  // } else {
  //   AppLogger::Warning("-> Send diagnostic message negative response");
  //   unsigned char responseData[] = {0x7F, data[0], 0x11};
  //   connection->sendDiagnosticPayload(logical_address_, responseData,
  //                                     sizeof(responseData));
  // }
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
  while (serverActive) {
    server.receiveUdpMessage();
  }
}

/*
 * Check permantly if tcp message was received
 */
void UdsServer::listenTcp() {
  server.setupTcpSocket();

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
