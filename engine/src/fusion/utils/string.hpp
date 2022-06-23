#pragma once

namespace fe {
    using namespace std::string_literals;
    /**
     * @brief Helper class for C++ strings.
     */
    class String {
    public:
        String() = delete;

        /**
         * Converts a CTF16 (wide) string to a UTF8 string.
         * @param str The the string to convert.
         * @return The converted string.
         */
        static std::string ConvertUtf8(const std::wstring& str);

        /**
         * Converts a CTF16 (wide) char to a UTF8 char.
         * @param c The string.
         * @return The converted char.
         */
        static char ConvertUtf8(wchar_t c);

        /**
         * Converts a CTF8 string to a UTF16 (wide) string.
         * @param str The string.
         * @return The converted string.
         */
        static std::wstring ConvertUtf16(const std::string& str);

        /**
         * Converts a CTF8 char to a UTF16 (wide) char.
         * @param c The char to convert.
         * @return The converted char.
         */
        static wchar_t ConvertUtf16(char c);

        /**
         * Converts a string to a valid boolean.
         * @param str The string.
         * @return The converted bool.
         */
        static bool ConvertBool(std::string str);

        /**
         * Splits a string by a separator.
         * @param str The string.
         * @param sep The separator.
         * @return The split string vector.
         */
        static std::vector <std::string> Split(const std::string& str, char sep);

        /**
         * Gets if a string starts with a token.
         * @param str The string.
         * @param token The token.
         * @return If a string starts with the token.
         */
        static bool StartsWith(const std::string& str, const std::string& token);

        /**
         * Gets if a string contains a token.
         * @param str The string.
         * @param token The token.
         * @return If a string contains the token.
         */
        static bool Contains(const std::string& str, const std::string& token) noexcept;

        /**
         * Gets if a character is a whitespace.
         * @param c The character.
         * @return If a character is a whitespace.
         */
        static bool IsWhitespace(char c) noexcept;

        /**
         * Gets if a string is a number.
         * @param str The string.
         * @return If a string is a number.
         */
        static bool IsNumber(const std::string& str) noexcept;

        /**
         * Gets the first char index in the string.
         * @param str The string.
         * @param c The char to look for.
         * @return The char index.
         */
        static size_t FindCharPos(const std::string& str, char c) noexcept;

        /**
         * Trims the left and right side of a string of whitespace.
         * @param str The string.
         * @param whitespace The whitespace type.
         * @return The trimmed string.
         */
        static std::string Trim(std::string str, const std::string& whitespace = " \t\n\r");

        /**
         * Removes all tokens from a string.
         * @param str The string.
         * @param token The token.
         * @return The string with the tokens removed.
         */
        static std::string RemoveAll(std::string str, char token);

        /**
         * Removes the last token from a string.
         * @param str The string.
         * @param token The token.
         * @return The string with the last token removed.
         */
        static std::string RemoveLast(std::string str, char token);

        /**
         * Replaces all tokens from a string.
         * @param str The string.
         * @param token The token.
         * @param to The string to replace the tokens with.
         * @return The string with the tokens replaced.
         */
        static std::string ReplaceAll(std::string str, const std::string& token, const std::string& to);

        /**
         * Replaces the first token from a string.
         * @param str The string.
         * @param token The token.
         * @param to The string to replace the tokens with.
         * @return The string with the tokens replaced.
         */
        static std::string ReplaceFirst(std::string str, const std::string& token, const std::string& to);

        /**
         * Fixes all tokens return line tokens from a string.
         * @param str The string.
         * @return The string with return lines fixed.
         */
        static std::string FixEscapedChars(std::string str);

        /**
         * Unfixes all tokens return line tokens from a string.
         * @param str The string.
         * @return The string with return lines unfixed.
         */
        static std::string UnfixEscapedChars(std::string str);

        /**
         * Lower cases a string.
         * @param str The string.
         * @return The lowercased string.
         */
        static std::string Lowercase(std::string str);

        /**
         * Uppercases a string.
         * @param str The string.
         * @return The uppercased string.
         */
        static std::string Uppercase(std::string str);

        /**
         * Find position of a string.
         * @param str The string.
         * @param pattern The pattern
         * @param pos Index of character to search from (default 0).
         * @return Index of start of first occurrence.
         */
        static size_t FindInsensitive(std::string str, std::string pattern, size_t pos = 0);
    };
}
