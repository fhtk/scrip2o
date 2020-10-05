/****************************************************************************\
 *                                 scrip2o™                                 *
 *                                                                          *
 *                    Copyright © 2020 Alexander Nicholi                    *
 *                       Released under BSD-2-Clause.                       *
\****************************************************************************/

#ifndef INC__SCRIP_LEXER_H
#define INC__SCRIP_LEXER_H

#include <uni/types/int.h>

typedef u8 tokid_t;

enum
{
	T_END = 0,
	T_LF,
	T_DIREC,
	T_STRLIT,
	T_IDENT,
	T_NUM
};

struct tok
{
	u8* data;
	u32 data_sz : 29;
	u32 id : 3;
};

struct tok* lex( const char* );

#endif /* INC__SCRIP_LEXER_H */
