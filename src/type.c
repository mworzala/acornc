#include "type.h"

#include "string.h"

char *type_tag_to_string(TypeTag tag) {
    switch (tag) {
        case TypeUnknown:
            return "!!!";
//        case TypeU8:
//            return "u8";
        case TypeI8:
            return "i8";
//        case TypeU16:
//            return "u16";
        case TypeI16:
            return "i16";
//        case TypeU32:
//            return "u32";
        case TypeI32:
            return "i32";
//        case TypeU64:
//            return "u64";
        case TypeI64:
            return "i64";
//        case TypeU128:
//            return "u128";
        case TypeI128:
            return "i128";
//        case TypeUSize:
//            return "usize";
        case TypeISize:
            return "isize";
        case TypeF32:
            return "f32";
        case TypeF64:
            return "f64";
        case TypeBool:
            return "bool";

        case TypeIntUnsigned:
            return "int_unsigned";
        case TypeIntSigned:
            return "int_signed";

        case TypeRef:
            return "&";

        default:
            return "<?>";
    }
}

bool type_is_extended(Type type) {
    return type.tag >= __TYPE_LAST;
}

TypeTag type_tag(Type type) {
    if (type_is_extended(type)) {
        return type.extended->tag;
    } else {
        return type.tag;
    }
}

bool type_is_integer(Type type) {
    //todo support arbitrary sized ints
    return type.tag >= TypeI8 && type.tag <= TypeISize;
}

Type type_from_name(char *type_name) {
#define is_name(rhs) (strcmp(type_name, rhs) == 0)

//    if is_name("u8") {
//        return (Type) {.tag = TypeU8};
//    } else
    if is_name("i8") {
        return (Type) {.tag = TypeI8};
//    } else if is_name("u16") {
//        return (Type) {.tag = TypeU16};
    } else if is_name("i16") {
        return (Type) {.tag = TypeI16};
//    } else if is_name("u32") {
//        return (Type) {.tag = TypeU32};
    } else if is_name("i32") {
        return (Type) {.tag = TypeI32};
//    } else if is_name("u64") {
//        return (Type) {.tag = TypeU64};
    } else if is_name("i64") {
        return (Type) {.tag = TypeI64};
//    } else if is_name("u128") {
//        return (Type) {.tag = TypeU128};
    } else if is_name("i128") {
        return (Type) {.tag = TypeI128};
//    } else if is_name("usize") {
//        return (Type) {.tag = TypeUSize};
    } else if is_name("isize") {
        return (Type) {.tag = TypeISize};
    } else if is_name("f32") {
        return (Type) {.tag = TypeF32};
    } else if is_name("f64") {
        return (Type) {.tag = TypeF64};
    } else if is_name("bool") {
        return (Type) {.tag = TypeBool};
    }

    fprintf(stderr, "Unknown type name: %s\n", type_name);
    assert(false);

#undef is_name
}
