#ifndef CONFIG_TYPE_H
#define CONFIG_TYPE_H

#include "common.h"

typedef enum type_tag_s {
    // Placeholder type, may not be present in any MIR node.
    TypeUnknown,

    // This section does not have an ExtendedType

    TypeU8,
    TypeI8,
    TypeU16,
    TypeI16,
    TypeU32,
    TypeI32,
    TypeU64,
    TypeI64,
    TypeU128,
    TypeI128,
    TypeUSize,
    TypeISize,
    TypeF32,
    TypeF64,
    TypeBool,

    // All following have an ExtendedType

    // Uses bits to represent the number of bits in the integer
    TypeIntUnsigned,
    TypeIntSigned,

    // Uses inner_type to represent the type being referenced
    TypeRef,

    __TYPE_LAST,
} TypeTag;

char *type_tag_to_string(TypeTag tag);

// Can determine which one of these is active by checking if the int value is < __TYPE_LAST
// The first page of memory is unmapped (reliably 4k or more) so the pointer will never have
// a value in that range.
// https://github.com/ziglang/zig/blob/1dd710947696ed11e35e9fc98b7dab4a1188c0d5/src/type.zig#L18
typedef union type_s {
    TypeTag tag;
    struct extended_type_s *extended;
} Type;

#define type(tag) ({.tag = tag})

bool type_is_extended(Type type);
TypeTag type_tag(Type type);

// Limited function to get a type from a name. Will panic if the name is not known.
Type type_from_name(char *type_name);

typedef struct extended_type_s {
    TypeTag tag;
    union {
        // Type pointed to by a reference
        Type *inner_type;
        // Number of bits in an integer type
        uint16_t bits;
    } data;
} ExtendedType;

#endif //CONFIG_TYPE_H
