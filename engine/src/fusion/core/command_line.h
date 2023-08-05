#pragma once

namespace fe {
    class FUSION_API CommandLineArgs {
    public:
        CommandLineArgs() = default;
        CommandLineArgs(int count, char** args);

        //const std::string& operator[](size_t index) const;
        std::optional<std::string> getParameter(const std::string& name) const;

    private:
        fst::unordered_flatmap<std::string, std::string> arguments;
    };

    class FUSION_API CommandLineParser {
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

    struct CommandResult {
        std::string output;
        int exitstatus;

        friend std::ostream &operator<<(std::ostream &os, const CommandResult &result) {
            os << "command exitstatus: " << result.exitstatus << " output: " << result.output;
            return os;
        }
        bool operator==(const CommandResult &rhs) const {
            return output == rhs.output && exitstatus == rhs.exitstatus;
        }

        bool operator!=(const CommandResult &rhs) const {
            return !(rhs == *this);
        }
    };

    class FUSION_API Command {
        /**
         * Execute system command and get STDOUT result.
         * Regular system() only gives back exit status, this gives back output as well.
         * @param command system command to execute
         * @return commandResult containing STDOUT (not stderr) output & exitstatus
         * of command. Empty if command failed (or has no output). If you want stderr,
         * use shell redirection (2&>1).
         */
        static CommandResult Execute(const char* cmd);
    };
}
