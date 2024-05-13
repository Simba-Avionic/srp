/**
 * @file main.cc
 * @author Jacek Kukawski (jacekka6@gmail.com)
 * @brief runs jaku_service
 * @version 1.0
 * @date 2024-05-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "apps/jaku_service/jaku_service.h"
#include "core/application/application_factory.h"

int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::jakuService::JakuService>::Start(argc, argv);
  return 0;
}