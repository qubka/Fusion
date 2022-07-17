#include "string.hpp"

using namespace fe;

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> UTF8_TO_UTF16_CONVERTER;

std::string String::ConvertUtf8(const std::wstring& string) {
    return UTF8_TO_UTF16_CONVERTER.to_bytes(string.data(), string.data() + string.length());
}

char String::ConvertUtf8(wchar_t c) {
    return UTF8_TO_UTF16_CONVERTER.to_bytes(c)[0];
}

std::wstring String::ConvertUtf16(const std::string& string) {
    return UTF8_TO_UTF16_CONVERTER.from_bytes(string.data(), string.data() + string.length());
}

wchar_t String::ConvertUtf16(char c) {
    return UTF8_TO_UTF16_CONVERTER.from_bytes(c)[0];
}

bool String::ConvertBool(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is{str};
    bool b;
    is >> std::boolalpha >> b;
    return b;
}

std::vector<std::string> String::Split(const std::string& str, char sep) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream is{str};

    while (std::getline(is, token, sep))
        tokens.emplace_back(token.c_str());
    return tokens;
}

std::string String::Join(const std::vector<std::string>& strings, const std::string& separator) {
    switch (strings.size()) {
        case 0: return {};
        case 1: return strings.front();
        default:
            std::ostringstream os;
            std::copy(strings.begin(), strings.end() - 1, std::ostream_iterator<std::string>(os, separator.c_str()));
            os << *strings.rbegin();
            return os.str();
    }
}

bool String::StartsWith(const std::string& str, const std::string& token) {
    if (str.length() < token.length())
        return false;
    return str.compare(0, token.length(), token) == 0;
}

bool String::Contains(const std::string& str, const std::string& token) noexcept {
    return str.find(token) != std::string::npos;
}

bool String::IsWhitespace(char c) noexcept {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

bool String::IsNumber(const std::string& str) noexcept {
    return std::all_of(str.cbegin(), str.cend(), [](auto c) {
        return (c >= '0' && c <= '9') || c == '.' || c == '-';
    });
}

size_t String::FindCharPos(const std::string& str, char c) noexcept {
    auto res = str.find(c);
    return res == std::string::npos ? -1 : res;
}

std::string String::Trim(const std::string& str, const std::string& whitespace) {
    auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return "";

    auto strEnd = str.find_last_not_of(whitespace);
    auto strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
}

std::string String::Extract(const std::string& str, const std::string& start, const std::string& stop) {
    auto strBegin = str.find(start);
    if (strBegin != std::string::npos)
        strBegin += start.length();
    else
        strBegin = 0;

    auto strEnd = str.find(stop);
    if (strEnd == std::string::npos)
        strEnd = str.length() - 1;

    return str.substr(strBegin,  strEnd - strBegin);
}

std::string String::RemoveAll(std::string str, char token) {
    str.erase(std::remove(str.begin(), str.end(), token), str.end());
    return str;
}

std::string String::RemoveLast(std::string str, char token) {
    for (auto it = str.end(); it != str.begin(); --it) {
        if (*it == token) {
            str.erase(it);
            return str;
        }
    }

    return str;
}

std::string String::ReplaceAll(std::string str, const std::string& token, const std::string& to) {
    auto pos = str.find(token);
    while (pos != std::string::npos) {
        str.replace(pos, token.length(), to);
        pos = str.find(token, pos + token.length());
    }

    return str;
}

std::string String::ReplaceFirst(std::string str, const std::string& token, const std::string& to) {
    const auto startPos = str.find(token);
    if (startPos == std::string::npos)
        return str;

    str.replace(startPos, token.length(), to);
    return str;
}

std::string String::FixEscapedChars(std::string str) {
    static const std::vector<std::pair<char, std::string_view>> replaces = {{'\\', "\\\\"}, {'\n', "\\n"}, {'\r', "\\r"}, {'\t', "\\t"}, {'\"', "\\\""}};

    for (const auto& [from, to] : replaces) {
        auto pos = str.find(from);
        while (pos != std::string::npos) {
            str.replace(pos, 1, to);
            pos = str.find(from, pos + 2);
        }
    }

    return str;
}

std::string String::UnfixEscapedChars(std::string str) {
    static const std::vector<std::pair<std::string_view, char>> replaces = {{"\\n", '\n'}, {"\\r", '\r'}, {"\\t", '\t'}, {"\\\"", '\"'}, {"\\\\", '\\'}};

    for (const auto& [from, to] : replaces) {
        auto pos = str.find(from);
        while (pos != std::string::npos) {
            if (pos != 0 && str[pos - 1] == '\\')
                str.erase(str.begin() + --pos);
            else
                str.replace(pos, from.length(), 1, to);
            pos = str.find(from, pos + 1);
        }
    }

    return str;
}

std::string String::Lowercase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

std::string String::Uppercase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

size_t String::FindInsensitive(std::string str, std::string pattern, size_t pos) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
    return str.find(pattern, pos);
}

std::string String::Quoted(const std::string& str) {
    return '"' + str + '"';
}
