#include <iostream>
#include <vector>
#include <string>


#include "../util.hpp"
#include "../log.hpp"
#include "../classfile/class_reader.hpp"

using namespace std;
using namespace jvm;
using namespace jvm::classfile;

int main()
{
    // string data;
    // string filename = "tt";
    // if(!util::util_file::read(filename, data))
    // {
    //     cout << "read file failed!" << endl;
    //     return -1;
    // }
    // vector<uint8_t> bytes(data.begin(), data.end());
    // for(auto& c : bytes)
    // {
    //     LOG(INFO, "%02X", c);
    // }

    cout << "is_big_endian: " << util::util_byte_order::isBigEndian() << endl;

    vector<uint8_t> bytes = {
        0xCA, 0xFE, 0xBA, 0xBE, // magic number
        0x00, 0x34, 0x00, 0x01, // major version, minor version
        0x12, 0x34, 0x56, 0x78,
        0x9A, 0xBC, 0xDE, 0xF0,
    };
    ClassReader reader(bytes);
    uint32_t magic = reader.read_uint32();
    LOG(INFO, "magic: 0x%X", magic);
    uint16_t major_version = reader.read_uint16();
    uint16_t minor_version = reader.read_uint16();
    uint64_t value = reader.read_uint64();
    LOG(INFO, "major_version: %d", major_version);
    LOG(INFO, "minor_version: %d", minor_version);
    LOG(INFO, "value: 0x%lX", value);


    return 0;
}



