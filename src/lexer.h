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
	T_STRLIT,
	T_NEWLN,
	T_NUM,
	T_DIREC,
	T_IDENT,
	T_END,
	MAX_T = T_END
};

struct tok
{
	u8* data;
	u32 data_sz : 29;
	u32 type : 3;
};

struct tok* lex( const char* );

#endif /* INC__SCRIP_LEXER_H */
