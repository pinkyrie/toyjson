#include <string_view>
#include <vector>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
template <typename T>
T parse_number(const std::string_view json, size_t index) {
    std::cout<<json<<std::endl;
    size_t start = index;
    size_t i = index;
    size_t n = json.size();
    T value;
    while (i < n) {
        char prev = json[i];
        if (json[i] == '+' || json[i] == '-') {
            ++i;
        }
        if (json[i] == '.') {
            ++i;
            continue;
        }
        if (json[i] == 'e' || json[i] == 'E') {
            ++i;
            if (i < n && (json[i] == '+' || json[i] == '-')) {
                ++i; // 处理指数部分的符号
            }
            continue;
        }
        if(std::isdigit(static_cast<unsigned char>(json[i]))) {
            ++i;
        }
        break;
    }
    size_t end = i;
    std::string_view num_str = json.substr(start, end - start);
    std::cout << num_str << std::endl;
    return T();
}




int main() {
    std::string_view sv = "1234.6,";
    parse_number<int>(sv,0);

}
/*
auto res = std::from_chars(str.data(), str.data() + str.size(), value);
if (res.ec == std::errc() && res.ptr == str.data() + str.size()) {
    return value;
}
return std::nullopt;


return 0;
*/