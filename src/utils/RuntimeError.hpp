#pragma once

#include <stdexcept>
#include <source_location>

class RuntimeError : public std::runtime_error {
public:
    RuntimeError(const std::string& what_arg, const std::source_location& loc) : std::runtime_error(what_arg), loc_(loc) {}

    const std::source_location& get_location() const noexcept {
        return loc_;
    }

private:
    std::source_location loc_;
};