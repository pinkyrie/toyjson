#include <iostream>
#include <sstream>
#include <string>
#include <variant>
#include <stdexcept>

// 定义返回类型：可以是 int 或 double
using NumberVariant = std::variant<int, double>;

NumberVariant parse_number(const std::string& str) {
    std::stringstream ss(str);
    double temp_val;

    // 尝试将字符串解析为浮点数
    ss >> temp_val;

    // 检查解析是否成功，以及是否有多余字符
    if (ss.fail() || !ss.eof()) {
        throw std::invalid_argument("Invalid number format: " + str);
    }

    // 如果字符串包含 '.' 或 'e'/'E'，返回 double 类型
    if (str.find('.') != std::string::npos || str.find('e') != std::string::npos || str.find('E') != std::string::npos) {
        return temp_val;  // 返回 double
    } else {
        return static_cast<int>(temp_val);  // 返回 int
    }
}

int main() {
    try {
        // 测试用例
        std::string input = "123.45";  // 可以换成其他输入如 "123" 或 "-456.78"

        // 调用 parse_number
        NumberVariant result = parse_number(input);

        // 检查返回的类型并输出
        if (std::holds_alternative<int>(result)) {
            std::cout << "Parsed as int: " << std::get<int>(result) << std::endl;
        } else if (std::holds_alternative<double>(result)) {
            std::cout << "Parsed as double: " << std::get<double>(result) << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
