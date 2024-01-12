/**
 * @file main2.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-12
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "communication-core/database/database.h"
#include "communication-core/json-parser/database_json_parser.h"
int main(int argc, char const *argv[]) {
  simba::database::Database db{};

  simba::database::json::DatabaseJsonParser::LoadJson(
      db, "apps/example/config/out.json");
  return 0;
}
