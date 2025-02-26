#include <iostream>
#include <string_view>
#include <optional>
#include <variant>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <charconv>
#include <system_error>
#include <limits>

struct JSONObject;

using JSONDict = std::unordered_map<std::string, JSONObject>;
using JSONList = std::vector<JSONObject>;
struct JSONObject {
    std::variant
    < std::monostate  // null
    , bool            // true
    , int             // 42
    , double          // 3.14
    , std::string     // "hello"
    , JSONList        // [42, "hello"]
    , JSONDict        // {"hello": 985, "world": 211}
    > inner;
    friend std::ostream& operator<<(std::ostream& os, const JSONObject& obj);
};
std::string escape_string(const std::string& s) {
    std::string result = "\"";
    for (char c : s) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) { // 控制字符
                    char buffer[7];
                    snprintf(buffer, sizeof(buffer), "\\u%04x", c);
                    result += buffer;
                } else {
                    result += c;
                }
        }
    }
    result += "\"";
    return result;
}

std::ostream& operator<<(std::ostream& os, const JSONObject& obj) {
    std::visit([&os](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            os << "null";
        }
        else if constexpr (std::is_same_v<T, bool>) {
            os << (arg ? "true" : "false");
        }
        else if constexpr (std::is_same_v<T, int>) {
            os << arg;
        }
        else if constexpr (std::is_same_v<T, double>) {
            os << arg;
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            os << escape_string(arg);
        }
        else if constexpr (std::is_same_v<T, JSONList>) { // 为什么能看到struct内部声明的variant的类型
            os << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                os << arg[i];
                if (i != arg.size() - 1)
                    os << ", ";
            }
            os << "]";
        }
        else if constexpr (std::is_same_v<T, JSONDict>) {
            os << "{";
            size_t count = 0;
            for (const auto& [key, value] : arg) {
                os << escape_string(key) << ": " << value;
                if (count != arg.size() -1)
                    os << ", ";
                ++count;
            }
            os << "}";
        }
    }, obj.inner);
    return os;
}

namespace num_machine {
    enum status {
        Blank    = 0,
        Sign     = 1,
        Dot      = 2,
        Exponentiation  = 3,
        Number   = 4,
        Alphabet = 5,
        End      = 6,

        T       = 98,
        F       = 99
    };
    enum num_type {
        INT,
        DOUBLE,
        NAN
    };
}

num_machine::status get_status(char c) {
    if (c >= '0' && c <= '9')
    {
        return num_machine::Number;
    }
    else if (c == ' ')
    {
        return num_machine::Blank;
    }
    else if (c == '+' || c == '-')
    {
        return num_machine::Sign;
    }
    else if (c == '.')
    {
        return num_machine::Dot;
    }
    else if (c == 'e')
    {
        return num_machine::Exponentiation;
    }
    else if (c == '\0')
    {
        return num_machine::End;
    }
    else
    {
        return num_machine::Alphabet;
    }
}

num_machine::num_type get_num_type(std::string_view str) {
    using namespace num_machine;
    int statusTable[10][7] = {
        //   +  .  e  0  a  end
        {0, 1, 2, F, 8, F, F},// 0: Begin
        {F, F, 2, F, 8, F, F},// 1: Sign
        {F, F, F, F, 9, F, F},// 2: Dot [before number]
        {7, F, 4, 5, 3, F, T},// 3: number [before e]
        {7, F, F, 5, 9, F, T},// 4: Dot [after number]
        {F, 1, F, F, 6, F, F},// 5: e
        {7, F, F, F, 6, F, T},// 6: number[after e]
        {7, F, F, F, F, F, T},// 7: Blank
        {7, F, 4, 5, 8, F, T},// 8: number[before dot]
        {7, F, F, 5, 9, F, T},// 9: number[after dot]
    };
    int idx = 0;
    int status = statusTable[0][get_status(str.at(idx))];
    bool hasDot = false;  // Track if we encountered a dot
    bool hasE = false;    // Track if we encountered 'e' or 'E'

    while (idx < str.size() && status != (int)T && status != (int)F)
    {

        if (str.at(idx) == '.') {
            hasDot = true;
        } else if (str.at(idx) == 'e' || str.at(idx) == 'E') {
            hasE = true;
        }

        idx += 1;
        if (idx < str.size()) {
            status = statusTable[status][get_status(str.at(idx))];
        }
    }
    if (idx == str.size()) {
        status = statusTable[status][6];
    }
    if (status == T) {
        // If we encountered a dot or 'e', it's likely a floating point number (double)
        if (hasDot || hasE) {
            std::cout << "The number is a double.\n";
            return DOUBLE;
        } else {
            std::cout << "The number is an int.\n";
            return INT;
        }
    }
    std:: cout << "The number is invalid.\n";
    return NAN;
}

template <typename T>
std::optional<T> try_parse_num(std::string_view str) {
    T value;
    auto res = std::from_chars(str.data(), str.data() + str.size(), value);
    if(res.ec == std::errc() && res.ptr == str.data() + str.size()) {
        return value;
    }
    return std::nullopt;
}



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

JSONObject try_parse_string(const std::string_view json, size_t index) {
    size_t i = index;
    std::cout << "Full JSON string: " << json << std::endl;
    std::string result;
    while (i < json.size()) {
        char c = json[i];
        if (c == '"') {
            i++; // 跳过结束的双引号
            continue;
        }
        if (c == '\\') {
            i++; // 跳过反斜杠
            if (i >= json.size()) {
                throw std::runtime_error("Invalid escape sequence at end of string");
            }
            switch (char esc = json[i]) {
                    case '"': result += '\"'; i++; break;
                    case '\\': result += '\\'; i++; break;
                    case '/': result += '/'; i++; break;
                    case 'b': result += '\b'; i++; break;
                    case 'f': result += '\f'; i++; break;
                    case 'n': result += '\n'; i++; break;
                    case 'r': result += '\r'; i++; break;
                    case 't': result += '\t'; i++; break;
                    case 'u': {
                    if (i + 4 >= json.size()) {
                        throw std::runtime_error("Incomplete Unicode escape sequence");
                    }
                        auto hex = json.substr(i + 1, 4);
                        char16_t code = hex_to_char16(hex);
                        std::string utf8 = utf16_to_utf8(code);
                        result += utf8;
                        i += 5; // 跳过 'u' 和四个十六进制数字
                        break;
                    }
                    default:
                        throw std::runtime_error(std::string("Unsupported escape character: \\") + esc);
                }
            } else {
                // 处理普通字符
                // 检查控制字符是否被正确转义
                if (static_cast<unsigned char>(c) < 0x20) {
                    throw std::runtime_error("Unescaped control character in string");
                }
                result += c;
                i++;
            }
        }
    JSONObject obj;
    obj.inner = result;
    return obj;
}


std::pair<JSONObject, size_t> parse(std::string_view json) {
    using namespace num_machine;
    for(int i = 0; i < json.size(); ++i) {
        if((json[0] <= '9' && json[0] >= '0') || json[0] == '+' || json[0] == '-') {
            auto type = get_num_type(json);
            if(type == INT) {
                if(auto res = try_parse_num<int>(json); res) {
                    return {JSONObject(res.value()), json.size()};
                }
            }
            else if(type == DOUBLE) {
                if(auto res = try_parse_num<double>(json); res.has_value()) {
                    return {JSONObject(res.value()), json.size()};
                }
            }
        }
        else if(json[0] == '"') {
            return {try_parse_string(json, i + 1),json.size()};
        }
        else if(json[0] == '[') {
            int colon_conunter = 0; //[213,123]
            std::vector<JSONObject> res;
            size_t prev_seperate_pos = 0;
            size_t idx = 1;
            size_t start_pos = idx;
            for(; idx < json.size();) {
                if(json[idx] == ']') {
                    idx += 1;
                    break;
                }
                size_t separate_pos = json.find(',', idx);
                if(separate_pos == std::string::npos) {
                    separate_pos = json.find(']', idx);
                }
                if (json[idx] == '[') {
                    separate_pos = json.find(']', idx);
                    auto [obj, eaten] = parse(json.substr(idx, separate_pos - idx)); // 递归解析
                    if (eaten == 0) {
                        idx = 0;
                        break;
                    }
                    res.push_back(std::move(obj));
                    idx += eaten + 1;
                }
                start_pos = idx;
                auto [obj, eaten] = parse(json.substr(start_pos, separate_pos - start_pos));
                if (eaten == 0) {
                    idx = 0;
                    break;
                }
                res.push_back(std::move(obj));
                idx = idx + eaten + 1;
                start_pos = idx;
            }
            return {JSONObject{std::move(res)}, idx};
            }
        else if (json[0] == '{') {

        }
        }
    std:: cout << " null " <<std::endl;
    return {JSONObject(std::monostate()), 0};
}

int main() {
    /*JSONDict dict;
    dict["hello"] = JSONObject{985};
    dict["world"] = JSONObject{211};
    JSONObject obj1;
    obj1.inner = dict;

    std::string test_json = "{\"test\": \"test\"}";
    auto json = try_parse_string(test_json, 0);
    std::cout << json << std::endl;
    std::cout << obj1 << std::endl;*/
    /*
     * test num
     */
    // using namespace num_machine;
    // std::string test = "0";
    // auto res = get_num_type(test);
    // std::cout << res << std::endl;
    /*
     * test vector
     */
    std::string test = "[[123,123],[456,456]]";
    // std::string test = "[[123],[456]]";
    // std::string test = "[123,456]";
    auto [testobj, index] = parse(test);
    std::cout << "test : " << std::endl;
    std::cout << testobj << std::endl;
    std::cout << "index : " << index << std::endl;
    /*
     * test string
     */
    // std::string test = R"JSON("hello\nasas")JSON";
    // std::string test = R"("testdesu")";
    // std::string_view test = R"("testdesu\ntest")";
    // std::cout << test << std::endl;
    // // auto res = get_num_type(test);
    // // std::cout << res << std::endl;
    // auto testobj = parse(test);
    // std::cout << "test : " << std::endl;
    // std::cout << test[0] << std::endl;
    // std::cout << testobj << std::endl;

    /*JSONDict dict;
    dict["hello"] = JSONObject{985};
    dict["world"] = JSONObject{211};
    JSONObject obj1;
    obj1.inner = dict;
    std:: cout << obj1 << std::endl;*/
}
