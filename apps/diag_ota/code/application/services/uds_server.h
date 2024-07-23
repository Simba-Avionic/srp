/**
 * @file uds_server.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-04-28
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef APPS_DIAG_OTA_CODE_APPLICATION_SERVICES_UDS_SERVER_H_
#define APPS_DIAG_OTA_CODE_APPLICATION_SERVICES_UDS_SERVER_H_

#include <memory>
#include <string>
#include <vector>
#include "DoIPServer.h"  // NOLINT
#include "apps/diag_ota/code/application/services/i_validator.h"
#include "communication-core/sockets/Isocket_stream.h"
namespace simba {
namespace diag_ota {
namespace uds {
class UdsServer {
 protected:
  DoIPServer server;
  std::unique_ptr<DoIPConnection> connection;
  std::vector<std::thread> doipReceiver;
  bool serverActive = false;
  const std::string vin_;
  const uint16_t logical_address_;
  std::unique_ptr<uds::IValidator> validator;
  const std::unique_ptr<com::soc::ISocketStream> soc_;
  /**
   * Is called when the doip library receives a diagnostic message.
   * @param address   logical address of the ecu
   * @param data      message which was received
   * @param length    length of the message
   */
  void ReceiveFromLibrary(uint16_t address, unsigned char* data, int length);

  /**
   * Will be called when the doip library receives a diagnostic message.
   * The library notifies the application about the message.
   * Checks if there is a ecu with the logical address
   * @param targetAddress     logical address to the ecu
   * @return                  If a positive or negative ACK should be send
   * to the client
   */
  bool DiagnosticMessageReceived(uint16_t targetAddress);

  /**
   * Closes the connection of the server by ending the listener threads
   */
  void CloseConnection();

  /*
   * Check permantly if udp message was received
   */
  void listenUdp();

  /*
   * Check permantly if tcp message was received
   */
  void listenTcp();
  void ConfigureDoipServer();

 public:
  void Start();
  void AddValidator(const uint8_t service_id,
                    std::unique_ptr<IValidator> validator);
  void AddValidator(std::unique_ptr<IValidator> validator);
  UdsServer(const uint16_t& logical_address, const std::string& vin);
  ~UdsServer();
};
}  // namespace uds
}  // namespace diag_ota
}  // namespace simba

#endif  // APPS_DIAG_OTA_CODE_APPLICATION_SERVICES_UDS_SERVER_H_
