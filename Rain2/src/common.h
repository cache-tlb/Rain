#ifndef COMMON_H
#define COMMON_H


#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <map>
#include <set>
#include <functional>
#include <tuple>
#include <sstream>
#include <regex>
#include <climits>
#include <ctime>
#include <cassert>
#include <random>
#include <chrono>
#include <memory>
#include <GL/glew.h>

typedef std::map<std::string, std::string> Option;

template<typename _Tp>
_Tp parseTo(const std::string &str) {
    std::istringstream iss(str);
    _Tp ret;
    iss >> ret;
    return ret;
}

template<typename _Tp>
std::string toString(const _Tp &x) {
    std::ostringstream oss;
    oss << x;
    return oss.str();
}

template<typename _Tp>
_Tp clamp(const _Tp x, const _Tp low, const _Tp high) {
    return x < low ? low : (x > high ? high : x);
}

template<typename T, typename Tt>
T lerp(const T &a, const T &b, const Tt t) {
    return a*(1 - t) + b*t;
}

template<typename T>
T sqr(const T x) {
    return x*x;
}

template<typename _Tp>
_Tp random_float() {
    return (_Tp)rand() / (_Tp)RAND_MAX;
}

class QDebug {
public:
    QDebug() {}
    QDebug(const QDebug &) {}
    ~QDebug() {
        std::cout << oss.str() << std::endl;
    }

    template<typename T>
    inline QDebug & operator << (const T & rhs) {
        oss << rhs;
        oss << ' ';
        return *this;
    }
private:
    std::ostringstream oss;
};

inline QDebug qDebug() {
    return QDebug();
}

class ElapsedTimer {
public:
    ElapsedTimer() : begin_(clock_t::now()) {}
    void reset() { begin_ = clock_t::now(); }
    double elapsed_seconds() const {
        return std::chrono::duration_cast<second_t>(clock_t::now() - begin_).count();
    }

private:
    typedef std::chrono::high_resolution_clock clock_t;
    typedef std::chrono::duration<double, std::ratio<1> > second_t;
    std::chrono::time_point<clock_t> begin_;
};

/*class Later {
public:
    template <class callable, class... arguments>
    Later(int after, bool async, callable&& f, arguments&&... args) {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
        if (async) {
            std::thread([after, task]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(after));
                task();
            }).detach();
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(after));
            task();
        }
    }
};*/

#define PI M_PI

#endif // COMMON_H
