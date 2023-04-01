#include "error.hpp"

namespace cqlite {
    Error::Error (const std::string& what) : Base {what} {}

    Error::Error (const char* what) : Base {what} {}

} // namespace cqlite

