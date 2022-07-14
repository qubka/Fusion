#pragma once

namespace fe {
    using CommandArg = std::pair<std::string, std::string>;

    class CommandLineArgs {
        std::vector<CommandArg> arguments;
    public:
        CommandLineArgs() = default;
        CommandLineArgs(int count, char** args);

        const CommandArg& operator[](size_t index) const;

        //! Returns an iterator over the elements in this deque in proper sequence.
        std::vector<CommandArg>::iterator begin() { return arguments.begin(); }
        std::vector<CommandArg>::iterator end() { return arguments.end(); }
        std::vector<CommandArg>::reverse_iterator rbegin() { return arguments.rbegin(); }
        std::vector<CommandArg>::reverse_iterator rend() { return arguments.rend(); }
        [[nodiscard]] std::vector<CommandArg>::const_iterator begin() const { return arguments.begin(); }
        [[nodiscard]] std::vector<CommandArg>::const_iterator end() const { return arguments.end(); }
        [[nodiscard]] std::vector<CommandArg>::const_reverse_iterator rbegin() const { return arguments.rbegin(); }
        [[nodiscard]] std::vector<CommandArg>::const_reverse_iterator rend() const { return arguments.rend(); }

    private:
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
