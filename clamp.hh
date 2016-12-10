#ifndef HEADER_GUARD_CLAMP_H
#define HEADER_GUARD_CLAMP_H

template <class T>
const T& clamp(const T& v, const T& low, const T& high) {
    if (v < low) {
        return low;
    } else if (high < v) {
        return high;
    } else {
        return v;
    }
}

#endif
