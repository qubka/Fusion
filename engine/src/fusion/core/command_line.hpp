#pragma once

namespace fe {
    struct CommandLineArgs {
        int count{ 0 };
        char** args{ nullptr };

        const char* operator[](size_t index) const {
            assert(index < count && "Invalid argument index");
            return args[index];
        }
    };

    class CommandLineParser
    {
    public:
        struct CommandLineOption {
            std::vector<std::string> commands;
            std::string value;
            bool hasValue{ false };
            std::string help;
            bool set{ false };
        };
        std::unordered_map<std::string, CommandLineOption> options;
        CommandLineParser();
        void add(const std::string& name, const std::vector<std::string>& commands, bool hasValue, const std::string& help);
        void printHelp();
        void parse(const CommandLineArgs& arguments);
        bool isSet(const std::string& name);
        std::string getValueAsString(const std::string& name, const std::string& defaultValue);
        int32_t getValueAsInt(const std::string& name, int32_t defaultValue);
    };
}
