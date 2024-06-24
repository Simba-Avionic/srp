#ifndef MW_DIAG_DTC_SERVICE_LIB_CONVERTER_H_
#define MW_DIAG_DTC_SERVICE_LIB_CONVERTER_H_

#include <cstdint>
#include <vector>

namespace simba {
namespace mw {
namespace dtc {
namespace converter {
class VectorConverter {
 public:
  template <class T>
  static std::vector<uint8_t> Convert(const T& base, const bool invert);
  template <class T>
  static std::vector<uint8_t> Convert(const T& base);
};
}  // namespace converter
}  // namespace dtc
}  // namespace mw
}  // namespace simba

#endif  // MW_DIAG_DTC_SERVICE_LIB_CONVERTER_H_
