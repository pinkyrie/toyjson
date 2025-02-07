#include <string_view>
#include <vector>
#include <iostream>
#include <ranges>

int main() {
    // TODO: 控制台终端中文乱码
    std::string_view sv = "example,string]with}various;delimiters";
    std::string_view delimiters = "}";
    std::vector<std::string_view> tokens;

    auto split_view = sv | std::views::split(delimiters);

    for (auto part : split_view) {
        // 将子范围转换为 string_view
        std::string_view token = sv.substr(std::distance(sv.begin(), part.begin()), std::distance(part.begin(), part.end()));
        tokens.emplace_back(token);
    }

    // 输出所有提取的子字符串
    for (const auto& token : tokens) {
        std::cout << "substr" << token << std::endl;
    }

    return 0;
}
