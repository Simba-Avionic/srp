#ifndef APPS_EC_ENGINE_SERVICE_ENGINE_APP_HPP_
#define APPS_EC_ENGINE_SERVICE_ENGINE_APP_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <memory>

#include "ara/exec/adaptive_application.h"
#include "mw/i2c_service/controller/ads7828/controller.hpp"
#include "srp/apps/GhAppSkeleton.h"



namespace srp {
namespace apps {

class MyGhApp final : public ara::exec::AdaptiveApplication {
    private:
        std::string myPath;
        std::shared_ptr<i2c::ADS7828> adc;

        apps::GhAppSkeleton service_ipc;
        apps::GhAppSkeleton service_udp;

    protected:
         /**
     * @brief This function is called to launch the application
     *
     * @param token stop token
     */
    int Run(const std::stop_token& token) override;
    /**
     * @brief This function is called to initialiaze the application
     *
     * @param parms map with parms
     */
    int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                        parms) override;

    public:
        MyGhApp();
    };

}
}

#endif