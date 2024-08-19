#ifndef ARA_COM_SHM_SHM_OBJECT_H_
#define ARA_COM_SHM_SHM_OBJECT_H_

#include "ara/com/com_error_domain.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"

namespace ara {
namespace com {
namespace shm {
template <typename shm_type_t>
class ShmProxy {
 private:
  const ara::core::InstanceSpecifier instance_specifier_;

 public:
  ShmProxy(const ara::core::InstanceSpecifier& instance_specifier)
      : instance_specifier_{instance_specifier} {}

  ShmProxy(ShmProxy&) = delete;
  ShmProxy(ShmProxy&&) = delete;
  ShmProxy& operator=(ShmProxy&) = delete;
  ShmProxy& operator=(ShmProxy&&) = delete;

  ara::core::Result<void> StartFindService() noexcept;
  void StopFindService() noexcept;

  ara::core::Result<shm_type_t*> GetNewSamples() {}

  ~ShmProxy();
};
}  // namespace shm
}  // namespace com
}  // namespace ara
#endif  // ARA_COM_SHM_SHM_OBJECT_H_
