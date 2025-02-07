#include <iostream>
#include <string_view>
#include <optional>
#include <cctype>
#include <vector>
#include <charconv>
#include <cmath>
#include <system_error>

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

    // 如果解析失败，返回默认值
    std::cout << "Failed to parse number: \"" << num_str << "\"\n";
    return {std::nullopt, index};
}
int main() {
    // 示例字符串
    std::string_view sv1 = "1234.6,string]with}various;delimiters";
    std::string_view sv2 = "-567, +89.01e2 and 42";
    std::string_view sv3 = "0.5 and -0.25";

    // 解析第一个字符串中的数字
    std::cout << "---- Parsing sv1 ----\n";
    auto [num_double, next_index_double] = parse_number<double>(sv1, 0);
    std::cout << "Parsed number1: " << num_double.value() << "\n";
    std::cout << "Parsed index1: " << next_index_double << "\n";

//    // 解析第二个字符串中的数字（从位置 0）
//    std::cout << "---- Parsing sv2 ----\n";
//    double number2 = parse_number<double>(sv2, 0);
//    std::cout << "Parsed number2: " << number2 << "\n\n";
//
//    // 解析第三个字符串中的数字
//    std::cout << "---- Parsing sv3 ----\n";
//    double number3 = parse_number<double>(sv3, 0);
//    std::cout << "Parsed number3: " << number3 << "\n\n";

    return 0;
}
