#pragma once

#include <string>

namespace Quastra {

// An enumeration of all possible types in the Quastra language.
// We will expand this as we add more complex types like records and arrays.
enum class Type {
    Int,
    Bool,
    String,
    Float,
    Void, // Represents the absence of a type, e.g., for a function that doesn't return.
    Error // A special type to signify that an error occurred during analysis.
};

// Helper function to convert a Type to its string representation for debugging.
inline std::string to_string(Type type) {
    switch (type) {
        case Type::Int:    return "int";
        case Type::Bool:   return "bool";
        case Type::String: return "string";
        case Type::Float:  return "float";
        case Type::Void:   return "void";
        case Type::Error:  return "error";
    }
    return "unknown_type";
}

} // namespace Quastra
