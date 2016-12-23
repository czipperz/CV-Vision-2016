#ifndef HEADER_GUARD_TO_STRING_H
#define HEADER_GUARD_TO_STRING_H

#include <string>
#include <sstream>

namespace patch {
template <class T>
std::string to_string(const T& n) {
    std::ostringstream stm;
    stm << n;
    return stm.str();
}
}

#endif
