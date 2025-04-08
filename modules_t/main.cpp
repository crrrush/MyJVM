

#include "cmd.hpp"

#include "classpath/entry.hpp"
#include "classpath/classpath.hpp"

void startJVM(const Cmd& cmd)
{
    // Initialize JVM with the provided classpath and JRE path
    const std::string& classpath = cmd.get_class_path();
    const std::string& jre_path = cmd.get_jre_path();
    std::string java_class = cmd.get_java_class();
    const std::vector<std::string>& args = cmd.get_args();

    Classpath cp(jre_path, classpath);

    // Here you would typically initialize the JVM using JNI or similar APIs
    // For demonstration, we will just print the parameters
    std::cout << "Starting JVM with classpath: " << cp.toString() << std::endl;
    std::cout << "Using JRE path: " << jre_path << std::endl;
    std::cout << "Using Class path: " << classpath << std::endl;
    std::cout << "Main class: " << java_class << std::endl;
    std::cout << "Arguments: ";
    for (const auto& arg : args) {
        std::cout << arg << " ";
    }
    std::cout << std::endl;
    // Here you would typically call the JVM entry point, e.g., JNI_CreateJavaVM

    // 将java_class中的.替换为/
    for(auto& c : java_class) {
        if (c == '.') {
            c = '/';
        }
    }
    auto [data, entry, success] = cp.readClass(java_class);
    if (success) {
        std::cout << "Class data: [" << data << "]" << std::endl;
    } else {
        std::cerr << "Failed to read class data." << std::endl;
    }    
}

int main(int argc, char* argv[])
{
    Cmd& cmd = Cmd::parse_args(argc, argv);
    // cmd.test_parse_args();


    if(cmd.is_success() == false) {
        std::cout << cmd.get_error_msg() << std::endl;
        return -1;
    }
    if(cmd.is_help()) {
        cmd.print_usage();
        return 0;
    }
    if(cmd.is_version()) {
        cmd.print_version();
        return 0;
    }

    // cmd.test_parse_args();

    try{
        startJVM(cmd);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return -1;
    }


    return 0;
}
