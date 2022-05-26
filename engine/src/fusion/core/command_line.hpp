#pragma once

namespace fe {
    class CommandLineArgs {
        std::map<std::string, std::string> arguments;
    public:
        CommandLineArgs() = default;
        CommandLineArgs(int count, char** args);

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
        struct CommandLineOption {
            std::vector<std::string> commands;
            std::string value;
            std::string help;
            bool hasValue{ false };
            bool set{ false };
        };
        std::unordered_map<std::string, CommandLineOption> options;

    public:
        CommandLineParser();
        void add(const std::string& name, const std::vector<std::string>& commands, bool hasValue, const std::string& help);
        void printHelp();
        void parse(const CommandLineArgs& arguments);
        bool isSet(const std::string& name);
        std::string getValueAsString(const std::string& name, const std::string& defaultValue);
        int32_t getValueAsInt(const std::string& name, int32_t defaultValue);
    };
}
