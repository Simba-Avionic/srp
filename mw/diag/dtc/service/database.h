#ifndef MW_DIAG_DTC_SERVICE_DATABASE_H_
#define MW_DIAG_DTC_SERVICE_DATABASE_H_

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace simba {
namespace mw {
namespace dtc {
struct SnapshotService {
  SnapshotService(const std::string& address, const std::uint16_t id)
      : address_{address}, id_{id} {}
  const std::string address_;
  const std::uint16_t id_;
};

struct DataBaseElement {
  DataBaseElement(const uint32_t id, const std::string& name,
                  const std::vector<SnapshotService>& snapshot_list)
      : id_{id}, name_{name}, status_{0x00}, snapshot_list_{snapshot_list} {}
  const std::vector<SnapshotService>& GetSnapshotService() const {
    return snapshot_list_;
  }
  const uint32_t id_;
  const std::string name_;
  const std::uint8_t status_;

 private:
  std::vector<SnapshotService> snapshot_list_{};
};
class DataBase {
 private:
  std::unordered_map<uint32_t, DataBaseElement> db{};
  std::string db_hash{};
 public:
  const std::unordered_map<uint32_t, DataBaseElement>& GetDataBase() const;
  void InsertElement(const DataBaseElement& element);
  void SetHash(const std::string& hash);
  std::vector<uint32_t> GetListOfSupportDtc() const;
  uint8_t GetAllowedMask() const;
  DataBase(/* args */) = default;
  ~DataBase() = default;
};
}  // namespace dtc
}  // namespace mw
}  // namespace simba

#endif  // MW_DIAG_DTC_SERVICE_DATABASE_H_
