#ifndef SH_EXCEPTION_H
#define SH_EXCEPTION_H

#include <exception>

class sh_not_implemented_exception : public std::exception	{};

class sh_invalid_params : public std::exception {};

class sh_not_support : public std::exception {};

#endif