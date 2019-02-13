#include <iostream>
#include <vector>
#include <cstdio>
#include <string>
#include <fstream>
#include <cassert>
#include "thirdparty/fmt/format.h"

namespace CaramelVM {
    using ByteVector = std::vector<uint8_t>;

    void readFile(const std::string &filename, ByteVector &data) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << filename << " does not exist" << std::endl;
            return;
        }
        data = ByteVector(std::istreambuf_iterator<char>(file), {});
    }

    void printBytes(const ByteVector &data) {
        for (auto i : data) {
            printf("%hhx", i);
        }
        putchar('\n');
    }

    uint8_t readUint1(ByteVector::const_iterator &iter) {
        auto x = *iter++;
        return x;
    }

    uint16_t readUint2(ByteVector::const_iterator &iter) {
        uint16_t high = readUint1(iter);
        uint16_t low = readUint1(iter);
        return (high << 8) | low;
    }

    uint32_t readUint4(ByteVector::const_iterator &iter) {
        uint32_t high = readUint2(iter);
        uint32_t low = readUint2(iter);
        return (high << 16) | low;
    }

    std::string JavaSEVersionString(uint16_t major, uint16_t minor) {
        if (major < 45) { return "Unknown Java SE version"; }
        if (major > 50) {
            if (major <= 51) {
                return "7";
            } else if (major <= 52) {
                return "8";
            } else if (major <= 53) {
                return "9";
            } else if (major <= 54) {
                return "10";
            } else if (major <= 55) {
                return "11";
            }
        }
        return "Not supported :D";
    }

    struct ConstantClassInfo {
        uint16_t nameIndex;
    };
    struct ConstantFieldRefInfo {
        uint16_t classIndex;
        uint16_t nameAndTypeIndex;
    };

    struct ConstantMethodRefInfo {
        uint16_t classIndex;
        uint16_t nameAndTypeIndex;
    };

    struct ConstantInterfaceMethodRefInfo {
        uint16_t classIndex;
        uint16_t nameAndTypeIndex;
    };

    struct ConstantStringInfo {
        uint16_t stringIndex;
    };
    struct ConstantIntegerInfo {
        uint32_t bytes;
    };
    struct ConstantFloatInfo {
        uint32_t bytes;
    };
    struct ConstantLongInfo {
        uint64_t highBytes;
        uint64_t lowBytes;
    };
    struct ConstantDoubleInfo {
        uint64_t highBytes;
        uint64_t lowBytes;
    };
    struct ConstantNameAndTypeInfo {
        uint16_t nameIndex;
        uint16_t descriptorIndex;
    };
    struct ConstantUTF8Info {
        uint16_t length;
        uint8_t *bytes;
    };
    struct ConstantMethodHandleInfo {
        uint16_t referenceKind;
        uint16_t referenceIndex;
    };
    struct ConstantMethodTypeInfo {
        uint16_t descriptorIndex;
    };
    struct ConstantDynamicInfo {
        uint16_t boostrapMethodAttrIndex;
        uint16_t nameAndTypeIndex;
    };
    struct ConstantInvokeDynamicInfo {
        uint16_t boostrapMethodAttrIndex;
        uint16_t nameAndTypeIndex;
    };
    struct ConstantModuleInfo {
        uint16_t nameIndex;
    };
    struct ConstantPackageInfo {
        uint16_t nameIndex;
    };
    enum ConstantPoolTag {
        CONSTANT_Utf8 = 1,//	45.3	1.0.2
        CONSTANT_Integer = 3,//	45.3	1.0.2
        CONSTANT_Float = 4,//45.3	1.0.2
        CONSTANT_Long = 5,//45.3	1.0.2
        CONSTANT_Double = 6,//45.3	1.0.2
        CONSTANT_Class = 7,//45.3	1.0.2
        CONSTANT_String = 8,//45.3	1.0.2
        CONSTANT_Fieldref = 9,//45.3	1.0.2
        CONSTANT_Methodref = 10,//45.3	1.0.2
        CONSTANT_InterfaceMethodref = 11,//45.3	1.0.2
        CONSTANT_NameAndType = 12,//45.3	1.0.2
        CONSTANT_MethodHandle = 15,//51.0	7
        CONSTANT_MethodType = 16,//51.0	7
        CONSTANT_Dynamic = 17,//55.0	11
        CONSTANT_InvokeDynamic = 18,//	51.0	7
        CONSTANT_Module = 19,//53.0	9
        CONSTANT_Package = 20,//53.0	9
    };


    struct ConstantPoolInfo {
        ConstantPoolTag tag;
        union {
            ConstantClassInfo classInfo;
            ConstantFieldRefInfo fieldRefInfo;
            ConstantInterfaceMethodRefInfo interfaceMethodRefInfo;
            ConstantMethodRefInfo methodRefInfo;
            ConstantDoubleInfo doubleInfo;
            ConstantDynamicInfo dynamicInfo;
            ConstantFloatInfo floatInfo;
            ConstantIntegerInfo integerInfo;
            ConstantInvokeDynamicInfo invokeDynamicInfo;
            ConstantMethodHandleInfo methodHandleInfo;
            ConstantUTF8Info utf8Info;
            ConstantStringInfo stringInfo;
            ConstantModuleInfo moduleInfo;
            ConstantMethodTypeInfo methodTypeInfo;
            ConstantLongInfo longInfo;
            ConstantNameAndTypeInfo nameAndTypeInfo;
            ConstantPackageInfo packageInfo;
        };

        ConstantPoolInfo() {}
    };

    struct AttributeInfo {

        uint16_t attributeNameIndex;
        uint32_t attributeLength;
        ByteVector info;

    };
    struct FieldInfo {
        uint16_t accessFlags;
        uint16_t nameIndex;
        uint16_t descriptorIndex;
        uint16_t attributesCount;
        std::vector<AttributeInfo> attributes;
    };
    struct MethodInfo {
        uint16_t accessFlags;
        uint16_t nameIndex;
        uint16_t descriptorIndex;
        uint16_t attributesCount;
        std::vector<AttributeInfo> attributes;
    };

    struct ClassFile {
        std::string version;
        uint32_t constantPoolCount;
        std::vector<ConstantPoolInfo> constantPoolInfo;
        uint16_t accessFlags;
        uint16_t thisClass;
        uint16_t superClass;
        uint16_t interfaceCount;
        std::vector<uint16_t> interfaces;
        uint16_t fieldsCount;
        std::vector<FieldInfo> fields;
        uint16_t methodsCount;
        std::vector<MethodInfo> methods;
        uint16_t attributesCount;
        std::vector<AttributeInfo> attributes;

        ClassFile() {constantPoolInfo.emplace_back();}
    };

    void readConstantPool(ClassFile &classFile, ByteVector::const_iterator &iter) {
        auto &pool = classFile.constantPoolInfo;
        for (int i = 0; i < classFile.constantPoolCount - 1; i++) {
            uint8_t tag = readUint1(iter);
            ConstantPoolInfo info;
            info.tag = (ConstantPoolTag) tag;
            switch (info.tag) {

                case CONSTANT_Utf8:
                    info.utf8Info.length = readUint2(iter);
                    info.utf8Info.bytes = new uint8_t[info.utf8Info.length + 1];
                    for (auto j = 0; j < info.utf8Info.length; j++) {
                        info.utf8Info.bytes[j] = readUint1(iter);
                    }
                    info.utf8Info.bytes[info.utf8Info.length] = 0;
                    printf("%s\n", info.utf8Info.bytes);
                    break;
                case CONSTANT_Integer:
                    info.integerInfo.bytes = readUint4(iter);
                    break;
                case CONSTANT_Float:
                    info.floatInfo.bytes = readUint4(iter);
                    break;
                case CONSTANT_Long:
                    info.longInfo.highBytes = readUint4(iter);
                    info.longInfo.lowBytes = readUint4(iter);
                    break;
                case CONSTANT_Double:
                    info.doubleInfo.highBytes = readUint4(iter);
                    info.doubleInfo.lowBytes = readUint4(iter);
                    break;
                case CONSTANT_Class:
                    info.classInfo.nameIndex = readUint2(iter);
                    break;
                case CONSTANT_String:
                    info.stringInfo.stringIndex = readUint2(iter);
                    break;
                case CONSTANT_Fieldref:
                    info.fieldRefInfo.classIndex = readUint2(iter);
                    info.fieldRefInfo.nameAndTypeIndex = readUint2(iter);
                    break;
                case CONSTANT_Methodref:
                    info.methodRefInfo.classIndex = readUint2(iter);
                    info.methodRefInfo.nameAndTypeIndex = readUint2(iter);
                    break;
                case CONSTANT_InterfaceMethodref:
                    info.interfaceMethodRefInfo.classIndex = readUint2(iter);
                    info.interfaceMethodRefInfo.nameAndTypeIndex = readUint2(iter);
                    break;
                case CONSTANT_NameAndType:
                    info.nameAndTypeInfo.nameIndex = readUint2(iter);
                    info.nameAndTypeInfo.descriptorIndex = readUint2(iter);
                    break;
                case CONSTANT_MethodHandle:
                    info.methodHandleInfo.referenceKind = readUint2(iter);
                    info.methodHandleInfo.referenceIndex = readUint2(iter);
                    break;
                case CONSTANT_MethodType:
                    info.methodTypeInfo.descriptorIndex = readUint2(iter);
                    break;
                case CONSTANT_Dynamic:
                    info.dynamicInfo.boostrapMethodAttrIndex = readUint2(iter);
                    info.dynamicInfo.nameAndTypeIndex = readUint2(iter);
                    break;
                case CONSTANT_InvokeDynamic:
                    info.invokeDynamicInfo.boostrapMethodAttrIndex = readUint2(iter);
                    info.invokeDynamicInfo.nameAndTypeIndex = readUint2(iter);
                    break;
                case CONSTANT_Module:
                    info.moduleInfo.nameIndex = readUint2(iter);
                    break;
                case CONSTANT_Package:
                    info.packageInfo.nameIndex = readUint2(iter);
                    break;
                default:
                    throw std::runtime_error(fmt::format("Unknown constant info tag {}\n", (int) tag));
            }
            pool.emplace_back(info);
        }

    }

    void readAttribute(AttributeInfo &info, ByteVector::const_iterator &iter) {
        info.attributeNameIndex = readUint2(iter);
        info.attributeLength = readUint4(iter);
        for (int i = 0; i < info.attributeLength; i++)
            info.info.emplace_back(readUint1(iter));
    }

    void readClassFile(ClassFile &classFile, const ByteVector &data) {
        auto iter = data.begin();
        // read magic
        auto magic = readUint4(iter);
        if (magic != 0xcafebabe) {
            throw std::runtime_error("No a valid class file");
        }
        auto minor = readUint2(iter);
        auto major = readUint2(iter);
        classFile.version = JavaSEVersionString(major, minor);
        classFile.constantPoolCount = readUint2(iter);
        readConstantPool(classFile, iter);
        classFile.accessFlags = readUint2(iter);
        classFile.thisClass = readUint2(iter);
        classFile.superClass = readUint2(iter);
        classFile.interfaceCount = readUint2(iter);
        for (int i = 0; i < classFile.interfaceCount; i++) {
            classFile.interfaces.emplace_back(readUint2(iter));
        }
        classFile.fieldsCount = readUint2(iter);
        for (int i = 0; i < classFile.fieldsCount; i++) {
            FieldInfo info;
            info.accessFlags = readUint2(iter);
            info.nameIndex = readUint2(iter);
            info.descriptorIndex = readUint2(iter);
            info.attributesCount = readUint2(iter);
            for (int j = 0; j < info.attributesCount; j++) {
                info.attributes.emplace_back();
                readAttribute(info.attributes.back(), iter);
            }
            classFile.fields.emplace_back(info);
        }
        classFile.methodsCount = readUint2(iter);
        for (int i = 0; i < classFile.methodsCount; i++) {
            MethodInfo info;
            info.accessFlags = readUint2(iter);
            info.nameIndex = readUint2(iter);
            info.descriptorIndex = readUint2(iter);
            info.attributesCount = readUint2(iter);
            for (int j = 0; j < info.attributesCount; j++) {
                info.attributes.emplace_back();
                readAttribute(info.attributes.back(), iter);
            }
            classFile.methods.emplace_back(info);
        }
        classFile.attributesCount = readUint2(iter);
        for (int i = 0; i < classFile.attributesCount; i++) {
            classFile.attributes.emplace_back();
            readAttribute(classFile.attributes.back(), iter);
        }
        assert(iter == data.end());
    }

}

const char *format(CaramelVM::ConstantPoolTag tag) {
    using namespace CaramelVM;
    switch (tag) {
        case CONSTANT_Utf8:
            return "CONSTANT_Utf8";
        case CONSTANT_Integer:
            return "CONSTANT_Integer";
        case CONSTANT_Float:
            return "CONSTANT_Float";
        case CONSTANT_Long:
            return "CONSTANT_Long";
        case CONSTANT_Double:
            return "CONSTANT_Double";
        case CONSTANT_Class:
            return "CONSTANT_Class";
        case CONSTANT_String:
            return "CONSTANT_String";
        case CONSTANT_Fieldref:
            return "CONSTANT_Fieldref";
        case CONSTANT_Methodref:
            return "CONSTANT_Methodref";
        case CONSTANT_InterfaceMethodref:
            return "CONSTANT_InterfaceMethodref";
        case CONSTANT_NameAndType:
            return "CONSTANT_NameAndType";
        case CONSTANT_MethodHandle:
            return "CONSTANT_MethodHandle";
        case CONSTANT_MethodType:
            return "CONSTANT_MethodType";
        case CONSTANT_Dynamic:
            return "CONSTANT_Dynamic";
        case CONSTANT_InvokeDynamic:
            return "CONSTANT_InvokeDynamic";
        case CONSTANT_Module:
            return "CONSTANT_Module";
        case CONSTANT_Package:
            return "CONSTANT_Package";
    }
}

template<>
struct fmt::formatter<CaramelVM::ClassFile> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const CaramelVM::ClassFile &c, FormatContext &ctx) {
        auto version = fmt::format("Java SE version: {}\n", c.version);
        auto constPool = fmt::format("Constant Pool Count: {}\n", c.constantPoolCount);
//        for (const auto &i:c.constantPoolInfo) {
//            constPool = fmt::format("{}{}\n", constPool, ::format(i.tag));
//        }
        auto methods = fmt::format("Method count: {}\n",c.methodsCount);
        for(const auto&i:c.methods){
            for(const auto&attr:i.attributes) {
                assert(c.constantPoolInfo[attr.attributeNameIndex].tag == CaramelVM::CONSTANT_Utf8);
                methods = fmt::format("{}{}\n", methods, c.constantPoolInfo[attr.attributeNameIndex].utf8Info.bytes);
            }
        }
        auto attributes =fmt::format("Attributes count: {}\n",c.attributesCount);
        for(const auto&attr:c.attributes) {
            assert(c.constantPoolInfo[attr.attributeNameIndex].tag == CaramelVM::CONSTANT_Utf8);
            attributes = fmt::format("{}{}\n", attributes, c.constantPoolInfo[attr.attributeNameIndex].utf8Info.bytes);
        }
        return fmt::format_to(ctx.out(), "{}{}{}{}\n", version, constPool, methods,attributes);
    }
};

using namespace CaramelVM;

int main() {
    ByteVector data;
    ClassFile classFile;
    readFile("data/hello.class", data);
    readClassFile(classFile, data);
    fmt::print("{}", classFile);
    return 0;
}