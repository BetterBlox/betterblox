#pragma once

#include <stdexcept>

class RuntimeError : public std::runtime_error {
public:
    RuntimeError(const std::string& what_arg, const char* file, const int line) : std::runtime_error(what_arg), file_(file), line_(line) {}

    const std::string& get_file() const noexcept {
        return file_;
    }
    int get_line() const noexcept {
        return line_;
    }

private:
    std::string file_;
    int line_;
};