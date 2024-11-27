/**
 * @file curl_driver.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef  CORE_CURL_CURL_DRIVER_HPP_
#define  CORE_CURL_CURL_DRIVER_HPP_

#include <curl/curl.h>
#include <string>
#include <optional>

namespace simba {
namespace curl {

namespace {
    constexpr auto K_URL = "http://192.168.10.99:5000/";
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

class Curl {
 public:
static void send_json_data(const std::string& endpoint, const std::string& json_data) {
    CURL *curl;
    CURLcode res;

    // Inicjalizacja libcurl (to można zrobić globalnie w aplikacji)
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if (!curl) {
        fprintf(stderr, "curl_easy_init() failed\n");
        curl_global_cleanup();
        return;
    }

    const std::string URL = K_URL + endpoint;
    
    // Ustawienie URL
    curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());

    // Ustawienie metody POST
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    // Ustawienie danych do wysłania
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());

    // Ustawienie nagłówka Content-Type na application/json
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    if (headers) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    // Wykonanie żądania
    res = curl_easy_perform(curl);

    // Sprawdzanie wyniku
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
        printf("Dane zostały pomyślnie wysłane!\n");
    }

    // Czyszczenie zasobów
    if (headers) {
        curl_slist_free_all(headers);
    }
    curl_easy_cleanup(curl);

    // Cleanup libcurl (to można zrobić globalnie w aplikacji)
    curl_global_cleanup();
    }

    static std::optional<std::string> get_json_data(const std::string& endpoint) {
        CURL *curl;
        CURLcode res;
        std::string readBuffer;

        // Inicjalizacja libcurl
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        
        if (!curl) {
            fprintf(stderr, "curl_easy_init() failed\n");
            curl_global_cleanup();
            return {};
        }

        const std::string URL = K_URL + endpoint;
        
        // Ustawienie URL
        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());

        // Ustawienie metody GET
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

        // Ustawienie callbacku do przechwytywania odpowiedzi
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Wykonanie żądania
        res = curl_easy_perform(curl);

        // Sprawdzanie wyniku
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return {};
        }
        // Czyszczenie zasobów
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return readBuffer;
    }
};

}  // namespace curl
}  // namespace simba

#endif  //  CORE_CURL_CURL_DRIVER_HPP_
