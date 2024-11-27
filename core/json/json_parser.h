/**
 * @file json_parser.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef CORE_JSON_JSON_PARSER_H_
#define CORE_JSON_JSON_PARSER_H_

#include <fstream>
#include <string>
#include <optional>
#include <vector>
#include <utility>

#include "singleheader/simdjson.h"

namespace simba {
namespace core {
namespace json {
class JsonParser {
    using doc_t = simdjson::dom::object;
    using parser_t = simdjson::dom::parser;

 private:
    parser_t parser_;
    doc_t json_;

 public:
    // Konstruktor z pliku
static std::optional<JsonParser> Parser(const std::string& path) noexcept {
    try {
        return std::optional<JsonParser>(path);
    } catch (const simdjson::simdjson_error& e) {
        return std::nullopt;
    }
}

    JsonParser(JsonParser&& other) noexcept = default;
    // // Konstruktor z danych JSON (jako string)
    explicit JsonParser(const doc_t& data) {
        json_ = data;
    }


    // Konstruktor z obiektu JSON
    explicit JsonParser(const std::string& path) {
        this->json_ = parser_.load(path);
    }

    // Statyczny parser z obiektu JSON
    static std::optional<JsonParser> Parser(const doc_t& json) noexcept {
        return JsonParser(json);
    }

    // Pobranie obiektu JSON o zadanej nazwie
    std::optional<JsonParser> GetObject(const std::string& name) const {
        try {
            if (!json_[name].is_object()) {
                return std::nullopt;
            }
            auto res = json_[name].get_object();
            if (res.error() != simdjson::SUCCESS) {
                return std::nullopt;
            }
            return JsonParser(res.value());
        } catch (const simdjson::simdjson_error& e) {
            return std::nullopt;
        }
    }

    // Pobranie bieżącego obiektu JSON
    doc_t  GetObject() const {
        return json_;
    }

    // Pobranie stringa o zadanej nazwie
    std::optional<std::string> GetString(const std::string& name) const noexcept {
        try {
            std::string_view val;
            auto err = json_[name].get<std::string_view>(val);
            if (err != simdjson::SUCCESS) {
                return std::nullopt;
            }
            return std::string(val);
        } catch (const simdjson::simdjson_error &e) {
            return std::nullopt;
        }
        return std::nullopt;
    }

    std::optional<simdjson::dom::array> GetArray(const std::string& name) {
        try {
            return json_[name].get_array();
        } catch (const simdjson::simdjson_error& e) {
            return std::nullopt;
        }
    }

    // // Pobranie wektora typów z tablicy
    template <typename T>
    std::optional<std::vector<T>> GetArray(const std::string& name) {
        try {
            simdjson::dom::array arr = json_[name].get_array().value();
            std::vector<T> result;
            for (const auto& elem : arr) {
                if constexpr (std::is_floating_point_v<T>) {
                    result.push_back(static_cast<T>(elem.get_double().value()));
                } else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
                    result.push_back(static_cast<T>(elem.get_uint64().value()));
                } else if constexpr (std::is_integral_v<T>) {
                    result.push_back(static_cast<T>(elem.get_int64().value()));
                } else if constexpr (std::is_same_v<T, std::string>) {
                    result.push_back(std::string(elem.get_string().value()));
                }
            }
            return result;
        } catch (const simdjson::simdjson_error& e) {
            return std::nullopt;
        }
    }
    // Pobranie liczby o zadanej nazwie
    template <typename T>
    std::optional<T> GetNumber(const std::string& name) noexcept {
        try {
            if constexpr (std::is_floating_point_v<T>) {
                return static_cast<T>(json_[name].get_double().value());
            } else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
                return static_cast<T>(json_[name].get_uint64().value());
            } else if constexpr (std::is_integral_v<T>) {
                return static_cast<T>(json_[name].get_int64().value());
            }
        } catch (const simdjson::simdjson_error& e) {
            return std::nullopt;
        }
    }

    // Destruktor
    ~JsonParser() = default;
};

}  // namespace json
}  // namespace core
}  // namespace simba

#endif  // CORE_JSON_JSON_PARSER_H_
