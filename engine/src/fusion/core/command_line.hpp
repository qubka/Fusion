#pragma once

namespace fe {
    class CommandLineArgs {
        std::map<std::string, std::string> arguments;
    public:
        CommandLineArgs() = default;
        CommandLineArgs(int count, char** args);

        std::pair<std::string, std::string> operator[](size_t index) const;

        //! Returns an iterator over the elements in this deque in proper sequence.
        std::map<std::string, std::string>::iterator begin() { return arguments.begin(); }
        std::map<std::string, std::string>::iterator end() { return arguments.end(); }
        std::map<std::string, std::string>::reverse_iterator rbegin() { return arguments.rbegin(); }
        std::map<std::string, std::string>::reverse_iterator rend() { return arguments.rend(); }
        std::map<std::string, std::string>::const_iterator begin() const { return arguments.begin(); }
        std::map<std::string, std::string>::const_iterator end() const { return arguments.end(); }
        std::map<std::string, std::string>::const_reverse_iterator rbegin() const { return arguments.rbegin(); }
        std::map<std::string, std::string>::const_reverse_iterator rend() const { return arguments.rend(); }
    };

    class CommandLineParser {
    public:
        CommandLineParser();
        ~CommandLineParser() = default;

        void add(const std::string& name, const std::vector<std::string>& commands, bool hasValue, const std::string& help = "");
        void parse(const CommandLineArgs& arguments);

        void printHelp();
        bool isSet(const std::string& name);

        template<typename T>
        T getValue(const std::string& name, const T& defaultValue);

    private:
        struct CommandLineOption {
            std::vector<std::string> commands;
            std::string value;
            std::string help;
            bool hasValue{ false };
            bool set{ false };
        };
        std::map<std::string, CommandLineOption> options;
    };
}
