#include <iostream>
#include <string>
#include <charconv>

// 将四位十六进制数字转换为对应的 Unicode 字符
char16_t hex_to_char16(std::string_view hex) {
    unsigned int code = 0;
    auto [ptr, ec] = std::from_chars(hex.data(), hex.data() + hex.size(), code, 16);
    if (ec != std::errc()) {
        throw std::runtime_error("Invalid Unicode escape sequence: \\u" + std::string(hex));
    }
    return static_cast<char16_t>(code);
}

// 将 UTF-16 代码点转换为 UTF-8 字符串
std::string utf16_to_utf8(char16_t code) {
    std::string utf8;
    if (code <= 0x7F) {
        utf8 += static_cast<char>(code);
    } else if (code <= 0x7FF) {
        utf8 += static_cast<char>(0xC0 | ((code >> 6) & 0x1F));
        utf8 += static_cast<char>(0x80 | (code & 0x3F));
    } else {
        utf8 += static_cast<char>(0xE0 | ((code >> 12) & 0x0F));
        utf8 += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
        utf8 += static_cast<char>(0x80 | (code & 0x3F));
    }
    return utf8;
}

int main() {
    // 示例 Unicode 转义序列
    std::string_view unicode_escape1 = "0041"; // 'A'
    std::string_view unicode_escape2 = "4F60"; // '你'

    // 转换为 Unicode 代码点
    char16_t code1 = hex_to_char16(unicode_escape1);
    char16_t code2 = hex_to_char16(unicode_escape2);

    // 转换为 UTF-8 字符串
    std::string utf8_char1 = utf16_to_utf8(code1);
    std::string utf8_char2 = utf16_to_utf8(code2);

    // 输出结果
    std::cout << "Unicode \\u0041 转换为字符: " << utf8_char1 << std::endl;
    std::cout << "Unicode \\u4F60 转换为字符: " << utf8_char2 << std::endl;

    return 0;
}