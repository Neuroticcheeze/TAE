#pragma once
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <Framework/Utils/VFormat.hpp>

typedef std::range_error InvalidLogicalRangeException;
typedef std::out_of_range OutOfRangeAccessException;

#endif//EXCEPTION_H