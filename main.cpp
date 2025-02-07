#include <iostream>
#include <unordered_map>
#include <variant>
#include <vector>
#include <string_view>
#include <optional>
#include <cctype>
#include <charconv>


struct JSONObject;

using JSONDict = std::unordered_map<std::string, JSONObject>;
using JSONList = std::vector<JSONObject>;

struct JSONObject {
    std::variant
    < std::nullptr_t  // null
    , bool            // true
    , int             // 42
    , double          // 3.14
    , std::string     // "hello"
    , JSONList        // [42, "hello"]
    , JSONDict        // {"hello": 985, "world": 211}
    > inner;
    friend std::ostream& operator<<(std::ostream& os, const JSONObject& obj);
};

template <typename T>
std::optional<T> try_parse_num(std::string_view str) {
    T value;
    auto res = std::from_chars(str.data(), str.data() + str.size(), value);
    if (res.ec == std::errc() && res.ptr == str.data() + str.size()) {
        return value;
    }
    return std::nullopt;
}

template <typename T>
std::pair<std::optional<T>, size_t> parse_number(const std::string_view json, size_t index) {
    std::cout << "Full JSON string: " << json << std::endl;
    size_t start = index;
    size_t i = index;
    size_t n = json.size();

    // 处理可选的负号或正号
    if (i < n && (json[i] == '-' || json[i] == '+')) {
        ++i;
    }

    // 处理整数部分
    if (i < n && json[i] == '0') {
        ++i;
    } else {
        while (i < n && std::isdigit(static_cast<unsigned char>(json[i]))) {
            ++i;
        }
    }

    // 处理可选的小数部分
    if (i < n && json[i] == '.') {
        ++i;
        // 小数点后至少有一个数字
        if (i < n && std::isdigit(static_cast<unsigned char>(json[i]))) {
            while (i < n && std::isdigit(static_cast<unsigned char>(json[i]))) {
                ++i;
            }
        } else {
            // 无效的小数部分，返回默认值
            std::cout << "Invalid fractional part in number.\n";
            return {std::nullopt, index};
        }
    }

    // 处理可选的指数部分
    if (i < n && (json[i] == 'e' || json[i] == 'E')) {
        ++i;
        // 指数部分可以有可选的正负号
        if (i < n && (json[i] == '+' || json[i] == '-')) {
            ++i;
        }
        // 指数部分至少有一个数字
        if (i < n && std::isdigit(static_cast<unsigned char>(json[i]))) {
            while (i < n && std::isdigit(static_cast<unsigned char>(json[i]))) {
                ++i;
            }
        } else {
            // 无效的指数部分，返回默认值
            std::cout << "Invalid exponent part in number.\n";
            return {std::nullopt, index};
        }
    }

    size_t end = i;
    std::string_view num_str = json.substr(start, end - start);
    std::cout << "Extracted number string: \"" << num_str << "\"\n";

    std::optional<T> parsed_number;
    if constexpr (std::is_same_v<T, int>) {
        parsed_number = try_parse_num<int>(num_str);
    }
    else if constexpr (std::is_same_v<T, double>) {
        parsed_number = try_parse_num<double>(num_str);
    }

    if (parsed_number) {
        std::cout << "Parsed number: " << *parsed_number << "\n";
    }
    else {
        std::cout << "Failed to parse number: \"" << num_str << "\"\n";
    }

    return {parsed_number, end};
}

std::pair<JSONObject, size_t> parse_string(const std::string_view json, size_t index) {
    std::cout << "Full JSON string: " << json << std::endl;
    size_t start = index;
    size_t i = index;
    size_t n = json.size();
}

std::pair<JSONObject, size_t> parse(std::string_view json) {

}

int main()
{
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
