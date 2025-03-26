#pragma once

#include <iostream>
#include <vector>
#include <string>

// Enum for different option types
enum class OptionType {
    HELP,
    VERSION,
    CP,
    XJR,
    UNKNOWN
};

// Enum for parsing states
enum class ParseState {
    EXPECT_OPTION,
    EXPECT_CP_VALUE,
    EXPECT_CLASS_NAME,
    EXPECT_XJR_VALUE,
    EXPECT_ARGS
};

// Command-line argument parser class
class Cmd
{
public:
    // Parse command-line arguments and return a singleton instance of Cmd
    static Cmd& parse_args(int argc, char* argv[])
    {
        // Singleton pattern to ensure only one Cmd instance
        if(nullptr != _pcmd) return *_pcmd;

        Cmd* _pcmd = new Cmd();
        Cmd& cmd = *_pcmd;
        std::vector<std::string> args;

        // Step 1: Preprocess arguments (handle quotes)
        for (int i = 0; i < argc; ++i) {
            std::string arg = argv[i];
            if (!arg.empty() && arg.front() == '"') {
                arg = arg.substr(1, arg.find_last_of("\"") - 1);
            }
            args.push_back(arg);
        }

        // Step 2: Parse program name (remove path prefix)
        size_t pos = args[0].find_last_of("/\\");
        cmd._pname = (pos != std::string::npos) ? 
                        args[0].substr(pos+1) : args[0];

        // Step 3: Parse arguments using a state machine
        ParseState state = ParseState::EXPECT_OPTION;
        for (size_t i = 1; i < args.size(); ++i) 
        {
            const auto& arg = args[i];
            switch (state) 
            {
                case ParseState::EXPECT_OPTION:
                {
                    if (arg == "-h" || arg == "-help") {
                        cmd._help_flag = true;
                        return cmd;
                    } 
                    else if (arg == "-v" || arg == "-version") {
                        cmd._version_flag = true;
                        return cmd;
                    } 
                    else if (arg == "-cp" || arg == "-classpath") {
                        cmd._cp_option = arg;
                        state = ParseState::EXPECT_CP_VALUE;
                    } 
                    else if (arg == "-Xjre") {
                        cmd._Xjre_option = arg;
                        state = ParseState::EXPECT_XJR_VALUE;
                    } 
                    else if (arg[0] != '-') {
                        // Treat non-option argument as class name
                        cmd._java_class = arg;
                        state = ParseState::EXPECT_ARGS;
                    } 
                    else 
                    {
                        cmd._parse_sucess = false;
                        cmd._error_msg = "Unknown option: " + arg;
                        return cmd;
                    }
                    break;
                }
                case ParseState::EXPECT_CP_VALUE:
                {
                    cmd._class_path = arg;
                    state = ParseState::EXPECT_CLASS_NAME;
                    break;
                }
                case ParseState::EXPECT_XJR_VALUE:
                {
                    cmd._Xjre_path = arg;
                    state = ParseState::EXPECT_OPTION;
                    break;
                }
                case ParseState::EXPECT_CLASS_NAME:
                {
                    cmd._java_class = arg;
                    state = ParseState::EXPECT_ARGS;
                    break;
                }
                case ParseState::EXPECT_ARGS:
                {
                    cmd._args.push_back(arg);
                    break;
                }
                default:
                {
                    cmd._parse_sucess = false;
                    cmd._error_msg = "Invalid state encountered.";
                    return cmd;
                }
            }
        }

        // Step 4: Final validation
        
        // if (cmd._cp_option.empty() || cmd._class_path.empty()) {
        //     cmd._parse_sucess = false;
        //     cmd._error_msg = "Missing classpath";
        // }

        if (cmd._java_class.empty()) {
            cmd._parse_sucess = false;
            cmd._error_msg = "Missing main class name";
        }

        return cmd;
    }

    // Print usage information
    void print_usage()
    {
        std::cout << "Usage: " << _pname << " [-options] class [_args...]" << std::endl;
        generate_help();
    }

    // Print version information
    void print_version()
    {
        std::cout << "version 0.0.1" << std::endl;
    }

    // Test parsed arguments
    void test_parse_args()
    {
        if (!_parse_sucess) 
        {
            return print_usage();
        }
        if (_help_flag) 
        {
            return print_usage();
        }
        if (_version_flag) 
        {
            return print_version();
        }

        std::cout << "jrepath:" << _Xjre_path << std::endl;
        std::cout << "classpath:" << _class_path << "  class:" << _java_class << "  args:";
        for(const auto& e: _args) std::cout << e << " ";
        std::cout << std::endl;
    }

private:
    void generate_help() 
    {
        std::cout << "Available options:\n"
                << "  -h|--help         Show this help\n"
                << "  -v|--version      Show version\n"
                << "  -cp <path>        Set classpath\n"
                << "  -Xjre <path>      Specify JRE path\n";
    }

    // Private member variables
private:
    static Cmd* _pcmd;

    const std::string DEFAULT_JRE_PATH = "/usr/lib/jvm/java-17-openjdk-amd64";

    bool _parse_sucess;
    bool _help_flag;
    bool _version_flag;
    
    std::string _pname;
    
    std::string _cp_option;
    std::string _Xjre_option;

    std::string _Xjre_path;
    std::string _class_path;
    
    std::string _java_class;
    std::vector<std::string> _args;

    std::string _error_msg;

    // Hide constructor, copy constructor, and assignment operator
    explicit Cmd() :_parse_sucess(true),
                    _help_flag(false),
                    _version_flag(false),
                    _Xjre_path(DEFAULT_JRE_PATH) {}
    ~Cmd() = default;
    Cmd(const Cmd&) = delete;
    Cmd& operator=(const Cmd&) = delete;
};

// Initialize static member
Cmd* Cmd::_pcmd = nullptr;
