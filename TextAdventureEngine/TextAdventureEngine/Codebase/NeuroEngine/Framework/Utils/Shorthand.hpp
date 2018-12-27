#pragma once
#ifndef SHORTHAND_H
#define SHORTHAND_H

#define SETGETC( NAME, TYPE )\
	inline void Set##NAME( const TYPE & a_##NAME ) { m_##NAME = a_##NAME; }\
	inline const TYPE & Get##NAME() const { return m_##NAME; }\
	inline TYPE Get##NAME() { return m_##NAME; }

#define SETGET_FILTERSET( NAME, TYPE, FILTER_SET )\
	inline void Set##NAME( TYPE a_##NAME ) { m_##NAME = FILTER_SET; }\
	inline TYPE Get##NAME() const { return m_##NAME; }

#define SETGETC_V( NAME, TYPE, VAR )\
	inline void Set##NAME( const TYPE & a_##NAME ) { VAR = a_##NAME; }\
	inline const TYPE & Get##NAME() const { return VAR; }\
	inline TYPE Get##NAME() { return VAR; }

// Implied type (No const-correctness)
#define SETGET_V( NAME, TYPE, VAR )\
	inline void Set##NAME( TYPE a_##NAME ) { VAR = a_##NAME; }\
	inline TYPE Get##NAME() const { return VAR; }

#define SETGET_V_FILTERSET( NAME, TYPE, VAR, FILTER_SET )\
	inline void Set##NAME( TYPE a_##NAME ) { VAR = FILTER_SET; }\
	inline TYPE Get##NAME() const { return VAR; }


// Implied type (No const-correctness)
// Performs an operation after 'VAR' is set, only if the value changed
#define SETGET_V_POSTOPONCHANGESET( NAME, TYPE, VAR, POST_OP )\
	inline void Set##NAME( TYPE a_##NAME ) { bool diff = VAR != a_##NAME; VAR = a_##NAME; if ( diff ) POST_OP; }\
	inline TYPE Get##NAME() const { return VAR; }

// Performs an operation after 'VAR' is set, only if the value changed
#define SETGET_V_FILTERSET_POSTOPONCHANGESET( NAME, TYPE, VAR, FILTER_SET, POST_OP )\
	inline void Set##NAME( TYPE a_##NAME ) { bool diff = VAR != a_##NAME; VAR = FILTER_SET; if ( diff ) POST_OP; }\
	inline TYPE Get##NAME() const { return VAR; }

#endif//SHORTHAND_H