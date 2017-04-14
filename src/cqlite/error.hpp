#ifndef CQLITE_ERROR_INC
#define CQLITE_ERROR_INC

#include    <stdexcept>

namespace cqlite {

    class Error : public std::runtime_error
    {
        public:
            using runtime_error::runtime_error;
    };
}

#endif /* CQLITE_ERROR_INC */

