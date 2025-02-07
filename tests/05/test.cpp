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
T parse_number(const std::string_view json, size_t index) {
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
            return T();
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
            return T();
        }
    }

    size_t end = i;
    std::string_view num_str = json.substr(start, end - start);
    std::cout << "Extracted number string: \"" << num_str << "\"\n";

    // 尝试解析为指定类型
    if constexpr (std::is_same_v<T, int>) {
        auto num_int = try_parse_num<int>(num_str);
        if (num_int) {
            return *num_int;
        }
    } else if constexpr (std::is_same_v<T, double>) {
        auto num_double = try_parse_num<double>(num_str);
        if (num_double) {
            return *num_double;
        }
    }

    // 如果解析失败，返回默认值
    std::cout << "Failed to parse number: \"" << num_str << "\"\n";
    return T();
}
int main() {
    // 示例字符串
    std::string_view sv1 = "1234.6,string]with}various;delimiters";
    std::string_view sv2 = "-567, +89.01e2 and 42";
    std::string_view sv3 = "0.5 and -0.25";

    // 解析第一个字符串中的数字
    std::cout << "---- Parsing sv1 ----\n";
    double number1 = parse_number<double>(sv1, 0);
    std::cout << "Parsed number1: " << number1 << "\n\n";

    // 解析第二个字符串中的数字（从位置 0）
    std::cout << "---- Parsing sv2 ----\n";
    double number2 = parse_number<double>(sv2, 0);
    std::cout << "Parsed number2: " << number2 << "\n\n";

    // 解析第三个字符串中的数字
    std::cout << "---- Parsing sv3 ----\n";
    double number3 = parse_number<double>(sv3, 0);
    std::cout << "Parsed number3: " << number3 << "\n\n";

    return 0;
}
