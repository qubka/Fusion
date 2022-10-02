#pragma once

namespace fe {
    class CommandLineArgs {
    public:
        CommandLineArgs() = default;
        CommandLineArgs(int count, char** args);

        //const std::string& operator[](size_t index) const;
        std::optional<std::string> getParameter(const std::string& name) const;

    private:
        fst::unordered_flatmap<std::string, std::string> arguments;
    };

    class CommandLineParser {
    public:
        CommandLineParser();
        ~CommandLineParser() = default;

        void add(std::string_view name, std::vector<std::string>&& commands, bool hasValue, std::string&& help = "");
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
        fst::unordered_flatmap<std::string, CommandLineOption> options;
    };
}
