#ifndef CQLITE_ERROR_INC
#define CQLITE_ERROR_INC

#include <cqlite/cqlite_export.hpp>

#include <stdexcept>
#include <string>

namespace cqlite {

    /**
     * The base class of all cqlite errors.
     */
    class CQLITE_EXPORT Error : public std::runtime_error
    {
        using Base = std::runtime_error;

      public:
        explicit Error (const std::string&);
        explicit Error (const char*);
    };
} // namespace cqlite

#endif /* CQLITE_ERROR_INC */

