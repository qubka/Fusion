#pragma once

namespace fe {
    using namespace std::chrono_literals;

    class Time {
    public:
        Time() = default;
        ~Time() = default;

        /**
         * Creates a new time. This function is internal. To construct time values, use {@link Time::Seconds}, {@link Time::Milliseconds} or {@link Time::Microseconds} instead.
         * @param duration The duration.
         */
        template<typename Rep, typename Period>
        constexpr Time(const std::chrono::duration<Rep, Period>& duration) : value{std::chrono::duration_cast<std::chrono::microseconds>(duration).count()} {}

        /**
         * Creates a time value from a number of seconds.
         * @tparam T The type of value to be casted to.
         * @param seconds Number of seconds.
         * @return Time value constructed from the amount of seconds.
         */
        template<typename T = float>
        constexpr static Time Seconds(const T& seconds) { return Time(std::chrono::duration<T>(seconds)); }

        /**
         * Creates a time value from a number of milliseconds
         * @tparam Rep The type of value to be casted to.
         * @param milliseconds Number of milliseconds.
         * @return Time value constructed from the amount of milliseconds.
         */
        template<typename T = int32_t>
        constexpr static Time Milliseconds(const T& milliseconds) { return Time(std::chrono::duration<T, std::micro>(milliseconds)); }

        /**
         * Creates a time value from a number of microseconds.
         * @tparam Rep The type of value to be casted to.
         * @param microseconds Number of microseconds.
         * @return Time value constructed from the amount of microseconds.
         */
        template<typename T = int64_t>
        constexpr static Time Microseconds(const T& microseconds) { return Time(std::chrono::duration<T, std::micro>(microseconds)); }

        /**
         * Gets the time value as a number of seconds.
         * @tparam T The type of value to be casted to.
         * @return Time in seconds.
         */
        template<typename T = float>
        constexpr auto asSeconds() const { return static_cast<T>(value.count()) / static_cast<T>(1000000); }

        /**
         * Gets the time value as a number of milliseconds.
         * @tparam T The type of value to be casted to.
         * @return Time in milliseconds.
         */
        template<typename T = int32_t>
        constexpr auto asMilliseconds() const { return static_cast<T>(value.count()) / static_cast<T>(1000); }

        /**
         * Gets the time value as a number of microseconds.
         * @tparam T The type of value to be casted to.
         * @return Time in microseconds.
         */
        template<typename T = int64_t>
        constexpr auto asMicroseconds() const { return static_cast<T>(value.count()); }

        /**
         * Gets the current time of this application.
         * @return The current time.
         */
        static Time Now() {
            static const auto LocalEpoch = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - LocalEpoch);
        }

        /**
         * Gets the current system time as a string. "%d-%m-%Y %I:%M:%S"
         * @tparam format The format to put the time into.
         * @return The date time as a string.
         */
        static std::string GetDateTime(const std::string& format = "%Y-%m-%d %H:%M:%S") {
            auto now = std::chrono::system_clock::now();
            auto timeT = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&timeT), format.data());
            return ss.str();
        }

        template<typename Rep, typename Period>
        constexpr operator std::chrono::duration<Rep, Period>() const {
            return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(value);
        }

        constexpr bool operator==(const Time& rhs) const { return value == rhs.value; }
        constexpr bool operator!=(const Time& rhs) const { return value != rhs.value; }
        constexpr bool operator<(const Time& rhs) const { return value < rhs.value; }
        constexpr bool operator<=(const Time& rhs) const { return value <= rhs.value; }
        constexpr bool operator>(const Time& rhs) const { return value > rhs.value; }
        constexpr bool operator>=(const Time& rhs) const { return value >= rhs.value; }
        constexpr Time operator-() const { return {-value}; }

        constexpr friend Time operator+(const Time& lhs, const Time& rhs) { return lhs.value + rhs.value; }
        constexpr friend Time operator-(const Time& lhs, const Time& rhs) { return lhs.value - rhs.value; }
        constexpr friend Time operator*(const Time& lhs, float rhs) { return lhs.value * rhs; }
        constexpr friend Time operator*(const Time& lhs, int64_t rhs) { return lhs.value * rhs; }
        constexpr friend Time operator*(float lhs, const Time& rhs) { return rhs * lhs; }
        constexpr friend Time operator*(int64_t lhs, const Time& rhs) { return rhs * lhs; }
        constexpr friend Time operator/(const Time& lhs, float rhs) { return lhs.value / rhs; }
        constexpr friend Time operator/(const Time& lhs, int64_t rhs) { return lhs.value / rhs; }
        constexpr friend double operator/(const Time& lhs, const Time& rhs) { return static_cast<double>(lhs.value.count()) / static_cast<double>(rhs.value.count()); }
        template<typename Period = std::ratio<1, 1>>
        constexpr friend double operator%(const Time& lhs, const Time& rhs) {
            return std::modf(std::chrono::duration_cast<std::chrono::duration<double, Period>>(lhs.value), std::chrono::duration_cast<std::chrono::duration<double, Period>>(rhs.value));
        }

        constexpr Time& operator+=(const Time& rhs) { return *this = *this + rhs; }
        constexpr Time& operator-=(const Time& rhs) { return *this = *this - rhs; }
        constexpr Time& operator*=(float rhs) { return *this = *this * rhs; }
        constexpr Time& operator*=(int64_t rhs) { return *this = *this * rhs; }
        constexpr Time& operator/=(float rhs) { return *this = *this / rhs; }
        constexpr Time& operator/=(int64_t rhs) { return *this = *this / rhs; }

    private:
        std::chrono::microseconds value{};
    };
}