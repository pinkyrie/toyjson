#include <iostream>
#include <string>
#include <charconv>

// ����λʮ����������ת��Ϊ��Ӧ�� Unicode �ַ�
char16_t hex_to_char16(std::string_view hex) {
    unsigned int code = 0;
    auto [ptr, ec] = std::from_chars(hex.data(), hex.data() + hex.size(), code, 16);
    if (ec != std::errc()) {
        throw std::runtime_error("Invalid Unicode escape sequence: \\u" + std::string(hex));
    }
    return static_cast<char16_t>(code);
}

// �� UTF-16 �����ת��Ϊ UTF-8 �ַ���
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
    // ʾ�� Unicode ת������
    std::string_view unicode_escape1 = "0041"; // 'A'
    std::string_view unicode_escape2 = "4F60"; // '��'

    // ת��Ϊ Unicode �����
    char16_t code1 = hex_to_char16(unicode_escape1);
    char16_t code2 = hex_to_char16(unicode_escape2);

    // ת��Ϊ UTF-8 �ַ���
    std::string utf8_char1 = utf16_to_utf8(code1);
    std::string utf8_char2 = utf16_to_utf8(code2);

    // ������
    std::cout << "Unicode \\u0041 ת��Ϊ�ַ�: " << utf8_char1 << std::endl;
    std::cout << "Unicode \\u4F60 ת��Ϊ�ַ�: " << utf8_char2 << std::endl;

    return 0;
}