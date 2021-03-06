#pragma once
#ifndef TYPETRAITS_H
#define TYPETRAITS_H

#include <type_traits>

#define STATIC_ASSERT( EXP, MSG ) static_assert( EXP, MSG )
#define IS_BASE_OF( BASE, DERIVED ) std::is_base_of< BASE, DERIVED >::value
#define IS_INTEGRAL( TYPE ) std::is_integral< TYPE >::value

#endif//TYPETRAITS_H