
#include <memory>

#include "cmd.hpp"
#include "classpath/class_path.hpp"
#include "classfile/class_file.hpp"

using namespace jvm;
using namespace jvm::classpath;
using namespace jvm::classfile;

std::shared_ptr<ClassFile> loadClass(const std::string& class_name, ClassPath& cp)
{
    auto [data, entry, success] = cp.read_class(class_name);
    if(!success)
    {
        LOG(ERROR, "Failed to read class data for %s", class_name.c_str());
        return nullptr;
    }

    if(data.empty())
    {
        LOG(ERROR, "Class data is empty for %s", class_name.c_str());
        return nullptr;
    }

    std::vector<uint8_t> class_data(data.begin(), data.end());
    auto [p_class_file, success_parse] = ClassFile::parse(class_data);
    if(!success_parse)
    {
        LOG(ERROR, "Failed to parse class file for %s", class_name.c_str());
        return nullptr;
    }

    return p_class_file;
}

void printClassInfo(std::shared_ptr<ClassFile> p_cf)
{
    // LOG(INFO, "--------------------------------");
    // LOG(INFO, "version: %s", p_cf->class_name().c_str());
    std::cout << "--------------------------------" << std::endl;
    std::cout << "version: " << p_cf->major_version() << "." << p_cf->minor_version() << std::endl;
    std::cout << "constants count: " << (p_cf->constant_pool()).size() << std::endl;
    std::cout << "access flags: " << p_cf->access_flags() << std::endl;
    std::cout << "this class: " << p_cf->class_name() << std::endl;
    std::cout << "super class: " << p_cf->super_class_name() << std::endl;
    std::cout << "interfaces: " << p_cf->interface_names().size() << std::endl;
    std::cout << "fields: " << p_cf->fields().size() << std::endl;
    for (const auto& field : p_cf->fields()) {
        std::cout << "  " << field->name() << ": " << field->descriptor() << std::endl;
    }
    std::cout << "methods: " << p_cf->methods().size() << std::endl;
    for (const auto& method : p_cf->methods()) {
        std::cout << "  " << method->name() << ": " << method->descriptor() << std::endl;
    }
    std::cout << "--------------------------------" << std::endl;
}


void startJVM(const Cmd& cmd)
{
    // Initialize JVM with the provided classpath and JRE path
    const std::string& classpath = cmd.get_class_path();
    const std::string& jre_path = cmd.get_jre_path();
    std::string java_class = cmd.get_java_class();
    const std::vector<std::string>& args = cmd.get_args();

    ClassPath cp(jre_path, classpath);

    // Here you would typically initialize the JVM using JNI or similar APIs
    // For demonstration, we will just print the parameters
    std::cout << "Starting JVM with classpath: " << cp.to_string() << std::endl;
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
    // auto [data, entry, success] = cp.read_class(java_class);
    // if (success) {
    //     std::cout << "Class data: [" << data << "]" << std::endl;
    // } else {
    //     std::cerr << "Failed to read class data." << std::endl;
    // }

    auto p_cf = loadClass(java_class, cp);
    if(!p_cf) {
        std::cerr << "Failed to load class: " << java_class << std::endl;
        return;
    }
    printClassInfo(p_cf);
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
