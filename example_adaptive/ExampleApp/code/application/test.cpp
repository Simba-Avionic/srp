#include "core/json/json_parser.h"
#include <iostream>
#include <vector>

struct GPIO {
    int id;
    int num;
    std::string direction;
    std::string description;
};

std::optional<std::vector<GPIO>> ParseGPIO(const std::string& filepath) {
    auto parser_opt = simba::core::json::JsonParser::Parser(filepath);
    if (!parser_opt) {
        std::cout<< "123";
        return std::nullopt;
    }

    auto parser = std::move(parser_opt.value());
    auto gpio_array_opt = parser.GetArray("gpio");
    std::cout<< "123";
    if (!gpio_array_opt) {
        return std::nullopt;
    }
std::cout<< "4";
    std::vector<GPIO> gpio_list;
    std::cout<< "5";
    for (const auto& gpio_elem : gpio_array_opt.value()) {
        simba::core::json::JsonParser gpio_parser(gpio_elem);
        std::cout<< "123";
        GPIO gpio{
            gpio_parser.GetNumber<int>("id").value_or(-1),
            gpio_parser.GetNumber<int>("num").value_or(-1),
            gpio_parser.GetString("direction").value_or("unknown"),
            gpio_parser.GetString("desc").value_or("unknown")
        };
        gpio_list.push_back(gpio);
    }
    return gpio_list;
}

int main() {
    auto gpio_list_opt = ParseGPIO("bazel-bin/example_adaptive/ExampleApp/code/application/test.json");
    if (!gpio_list_opt) {
        std::cerr << "Failed to parse GPIO configuration\n";
        return 1;
    }

    for (const auto& gpio : gpio_list_opt.value()) {
        std::cout << "ID: " << gpio.id
                  << ", Num: " << gpio.num
                  << ", Direction: " << gpio.direction
                  << ", Description: " << gpio.description << "\n";
    }

    return 0;
}
