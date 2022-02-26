#include "command_line.hpp"

using namespace fe;

CommandLineParser::CommandLineParser() {
    add("help", { "--help" }, 0, "Show help");
    add("validation", {"-v", "--validation"}, 0, "Enable validation layers");
    add("vsync", {"-vs", "--vsync"}, 0, "Enable V-Sync");
    add("fullscreen", { "-f", "--fullscreen" }, 0, "Start in fullscreen mode");
    add("width", { "-w", "--width" }, 1, "Set window width");
    add("height", { "-h", "--height" }, 1, "Set window height");
    add("shaders", { "-s", "--shaders" }, 1, "Select shader type to use (glsl or hlsl)");
    add("gpuselection", { "-g", "--gpu" }, 1, "Select GPU to run on");
    add("gpulist", { "-gl", "--listgpus" }, 0, "Display a list of available Vulkan devices");
    add("benchmark", { "-b", "--benchmark" }, 0, "Run example in benchmark mode");
    add("benchmarkwarmup", { "-bw", "--benchwarmup" }, 1, "Set warmup time for benchmark mode in seconds");
    add("benchmarkruntime", { "-br", "--benchruntime" }, 1, "Set duration time for benchmark mode in seconds");
    add("benchmarkresultfile", { "-bf", "--benchfilename" }, 1, "Set file name for benchmark results");
    add("benchmarkresultframes", { "-bt", "--benchframetimes" }, 0, "Save frame times to benchmark results file");
    add("benchmarkframes", { "-bfs", "--benchmarkframes" }, 1, "Only render the given number of frames");
}

void CommandLineParser::add(const std::string& name, const std::vector<std::string>& commands, bool hasValue, const std::string& help) {
    options[name].commands = commands;
    options[name].help = help;
    options[name].set = false;
    options[name].hasValue = hasValue;
    options[name].value = "";
}

void CommandLineParser::printHelp() {
    std::cout << "Available command line options:\n";
    for (auto option : options) {
        std::cout << " ";
        for (size_t i = 0; i < option.second.commands.size(); i++) {
            std::cout << option.second.commands[i];
            if (i < option.second.commands.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << ": " << option.second.help << "\n";
    }
    std::cout << "Press any key to close...";
}

void CommandLineParser::parse(const CommandLineArgs& arguments) {
    bool printHelp = false;
    // Known arguments
    for (auto& option : options) {
        for (auto& command : option.second.commands) {
            for (size_t i = 0; i < arguments.count; i++) {
                if (strcmp(arguments[i], command.c_str()) == 0) {
                    option.second.set = true;
                    // Get value
                    if (option.second.hasValue) {
                        if (arguments.count > i + 1) {
                            option.second.value = arguments[i + 1];
                        }
                        if (option.second.value == "") {
                            printHelp = true;
                            break;
                        }
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

bool CommandLineParser::isSet(const std::string& name) {
    return ((options.find(name) != options.end()) && options[name].set);
}

std::string CommandLineParser::getValueAsString(const std::string& name, const std::string& defaultValue) {
    assert(options.find(name) != options.end());
    std::string value = options[name].value;
    return (!value.empty()) ? value : defaultValue;
}

int32_t CommandLineParser::getValueAsInt(const std::string& name, int32_t defaultValue) {
    assert(options.find(name) != options.end());
    std::string value = options[name].value;
    if (!value.empty()) {
        char* numConvPtr;
        int32_t intVal = strtol(value.c_str(), &numConvPtr, 10);
        return (intVal > 0) ? intVal : defaultValue;
    } else {
        return defaultValue;
    }
    return int32_t();
}