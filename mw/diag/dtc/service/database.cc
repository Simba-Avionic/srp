/**
 * @file database.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-06-25
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "mw/diag/dtc/service/database.h"

#include <ranges>

namespace simba {
namespace mw {
namespace dtc {

namespace {
static constexpr std::uint8_t AllowedMask{0x07};
}  // namespace

void DataBase::SetHash(const std::string& hash) { db_hash = hash; }

uint8_t DataBase::GetAllowedMask() const { return AllowedMask; }
const std::unordered_map<uint32_t, DataBaseElement>& DataBase::GetDataBase()
    const {
  return db;
}

void DataBase::InsertElement(const DataBaseElement& element) {
  if (db.find(element.id_) == db.cend()) {
    db.insert(std::pair(element.id_, element));
  }
}

std::vector<uint32_t> DataBase::GetListOfSupportDtc() const {
  auto res = std::vector<uint32_t>();
  for (const auto& item : db) {
    res.push_back(item.second.id_);
  }
  return res;
}

}  // namespace dtc
}  // namespace mw
}  // namespace simba