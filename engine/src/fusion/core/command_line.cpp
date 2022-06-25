#include "command_line.hpp"
#include "fusion/utils/string.hpp"

using namespace fe;

CommandLineArgs::CommandLineArgs(int count, char** args) {
    for (int i = 0; i < count; i++) {
        std::string argument{ args[i] };
        if (auto pos = argument.find('='); pos != std::string::npos) {
            arguments.emplace(argument.substr(0, pos), argument.substr(pos + 1));
        } else {
            arguments.emplace(argument, "");
        }
    }
}

CommandLineParser::CommandLineParser() {
    add("help", { "--help" }, false, "Show help");
    add("validation", {"-v", "--validation"}, false, "Enable validation layers");
    add("vsync", {"-vs", "--vsync"}, false, "Enable V-Sync");
    add("fullscreen", { "-f", "--fullscreen" }, false, "Start in fullscreen mode");
    add("width", { "-w", "--width" }, true, "Set window width");
    add("height", { "-h", "--height" }, true, "Set window height");
    add("shaders", { "-s", "--shaders" }, true, "Select shader type to use (glsl or hlsl)");
    add("gpuselection", { "-g", "--gpu" }, true, "Select GPU to run on");
    add("gpulist", { "-gl", "--listgpus" }, false, "Display a list of available Vulkan devices");
    add("benchmark", { "-b", "--benchmark" }, false, "Run example in benchmark mode");
    add("benchmarkwarmup", { "-bw", "--benchwarmup" }, true, "Set warmup time for benchmark mode in seconds");
    add("benchmarkruntime", { "-br", "--benchruntime" }, true, "Set duration time for benchmark mode in seconds");
    add("benchmarkresultfile", { "-bf", "--benchfilename" }, true, "Set file name for benchmark results");
    add("benchmarkresultframes", { "-bt", "--benchframetimes" }, false, "Save frame times to benchmark results file");
    add("benchmarkframes", { "-bfs", "--benchmarkframes" }, true, "Only render the given number of frames");
}

void CommandLineParser::add(const std::string& name, const std::vector<std::string>& commands, bool hasValue, const std::string& help) {
    options.emplace(name, CommandLineOption{commands, "", help, hasValue, false});
}

void CommandLineParser::parse(const CommandLineArgs& arguments) {
    bool printHelp = false;
    // Known arguments
    for (auto& [alias, option] : options) {
        for (const auto& command : option.commands) {
            for (const auto& [argument, parameter] : arguments) {
                if (argument == command) {
                    option.set = true;
                    // Get value
                    if (option.hasValue) {
                        if (parameter.empty()) {
                            printHelp = true;
                            break;
                        }
                        option.value = parameter;
                    }
                }
            }
        }
    }
    // Print help for unknown arguments or missing argument values
    if (printHelp) {
        options["help"].set = true;
    }
}

void CommandLineParser::printHelp() {
    std::cout << "Available command line options:\n";

    for (const auto& [alias, option] : options) {
        std::cout << " ";
        for (size_t i = 0; i < option.commands.size(); i++) {
            std::cout << option.commands[i];
            if (i < option.commands.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << ": " << option.help << "\n";
    }

    std::cout << "Press any key to close...";
}

bool CommandLineParser::isSet(const std::string& name) {
    return ((options.find(name) != options.end()) && options[name].set);
}

template<typename T>
T CommandLineParser::getValue(const std::string& name, const T& defaultValue) {
    throw std::runtime_error("Unknown value");
}

template<>
std::string CommandLineParser::getValue<std::string>(const std::string& name, const std::string& defaultValue) {
    assert(options.find(name) != options.end());
    const std::string& value = options[name].value;
    return !value.empty() ? value : defaultValue;
}

template<>
int CommandLineParser::getValue<int>(const std::string& name, const int& defaultValue) {
    assert(options.find(name) != options.end());
    const std::string& value = options[name].value;
    if (!value.empty()) {
        int result = std::stoi(value);
        return (result > 0) ? result : defaultValue;
    } else {
        return defaultValue;
    }
}

template<>
float CommandLineParser::getValue<float>(const std::string& name, const float& defaultValue) {
    assert(options.find(name) != options.end());
    const std::string& value = options[name].value;
    if (!value.empty()) {
        float result = std::stof(value);
        return (result > 0) ? result : defaultValue;
    } else {
        return defaultValue;
    }
}

template<>
bool CommandLineParser::getValue<bool>(const std::string& name, const bool& defaultValue) {
    assert(options.find(name) != options.end());
    const std::string& value = options[name].value;
    if (!value.empty()) {
        return String::ConvertBool(value);
    } else {
        return defaultValue;
    }
}

