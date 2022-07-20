#pragma once

namespace fe {
    using CommandArg = std::pair<std::string, std::string>;

    class CommandLineArgs {
    public:
        CommandLineArgs() = default;
        CommandLineArgs(int count, char** args);

        const CommandArg& operator[](size_t index) const;
        ITERATABLE(CommandArg, arguments);

    private:
        std::vector<CommandArg> arguments;
        static CommandArg Empty;
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
        std::unordered_map<std::string, CommandLineOption> options;
    };
}
