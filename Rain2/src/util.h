#ifndef UTIL_H
#define UTIL_H

#include "common.h"

template<typename T>
T random(T from, T to) {
    T delta = to - from;
    return from + random_float<T>()*delta;
}

template<typename T>
T random(T to) {
    return random_float<T>()*to;
}

template<typename T>
T random(T from, T to, const std::function<T(T)> &interpolation) {
    T delta = to - from;
    return from + interpolation(random_float<T>())*delta;
}

template<typename T>
bool chance(T c) {
    return random<double>(0.0, 1.0) <= c;
}

#endif
