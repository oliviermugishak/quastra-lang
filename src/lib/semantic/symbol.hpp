#pragma once

#include "type.hpp"

namespace Quastra {

// Represents a variable or function in the symbol table.
// It stores all the semantic information we know about an identifier.
struct Symbol {
    Type type;
    bool is_mutable;
    bool is_initialized;
};

} // namespace Quastra
