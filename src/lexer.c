/****************************************************************************\
 *                                 scrip2o™                                 *
 *                                                                          *
 *                    Copyright © 2020 Alexander Nicholi                    *
 *                       Released under BSD-2-Clause.                       *
\****************************************************************************/

#include "lexer.h"

#include <stdlib.h>
#include <uni/err.h>
#include <uni/memory.h>
#include <uni/str.h>

static ptri scan_direc( const char* in )
{
	/* regex := ^#[A-Za-z]+(?!(\s)) */
	unsigned i = 0;

	if( in[i] != '#' )
	{
		return 0;
	}

	i++;

	while( (in[i] >= 'a' && in[i] <= 'z') || (in[i] >= 'A' && in[i] <= 'Z') )
	{
		i++;
	}

	if( in[i] == ' ' || in[i] == '\t' )
	{
		return i;
	}

	return 0;
}

static ptri scan_ident( const char* in )
{
	/* regex := [A-Za-z_][A-Za-z0-9_]* */
	unsigned i = 0;

	if( !((in[i] >= 'a' && in[i] <= 'z') || (in[i] >= 'A' && in[i] <= 'Z') || in[i] == '_') )
	{
		return 0;
	}

	i++;

	while( ((in[i] >= 'a' && in[i] <= 'z') || (in[i] >= 'A' && in[i] <= 'Z') || (in[i] >= '0' && in[i] <= '9') || in[i] == '_') )
	{
		i++;
	}

	return i;
}

static ptri scan_strlit( const char* in )
{
	unsigned i = 0;

	if( in[i] != '"' )
	{
		return 0;
	}

	i++;

	while( in[i] != '"' || (i > 0 && in[i] == '"' && in[i - 1] == '\\') )
	{
		i++;
	}

	if(in[i] == '"')
	{
		return i + 1;
	}

	return 0;
}

enum
{
	NUM_DEC = 0,
	NUM_OCT,
	NUM_HEX,
	NUM_BIN
};

static ptri scan_num( const char* in )
{
	unsigned i = 0;
	unsigned numtyp;

	if( in[i] == '0' )
	{
		switch(in[i])
		{
		case 'x':
		case 'X':
			numtyp = NUM_HEX;
			break;
		case 'o':
		case 'O':
			numtyp = NUM_OCT;
			break;
		case 'b':
		case 'B':
			numtyp = NUM_BIN;
			break;
		case '\0':
			return 0;
		default:
			numtyp = NUM_DEC;
			break;
		}

		i += 2;
	}
	else
	{
		numtyp = NUM_DEC;
	}

	while( (numtyp == NUM_DEC && in[i] >= '0' && in[i] <= '9')
	|| (numtyp == NUM_OCT && in[i] >= '0' && in[i] <= '7')
	|| (numtyp == NUM_BIN && in[i] >= '0' && in[i] <= '1')
	|| (numtyp == NUM_HEX && ((in[i] >= '0' && in[i] <= '9')
	|| (in[i] >= 'A' && in[i] <= 'F') || (in[i] >= 'a' && in[i] <= 'f'))))
	{
		i++;
	}

	return i;
}

static ptri scan_label( const char* in )
{
	return in[0] == ':' ? 1 : 0;
}

static ptri scan_newln( const char* in )
{
	return (in[0] == '\r' && in[1] == '\n') ? 2 :
		(in[0] == '\r' || in[0] == '\n') ? 1 : 0;
}

/* this is called directly by lex(), unlike other scanners */
static ptri scan_wspc( const char* in )
{
	unsigned i = 0;

	while( in[i] == ' ' || in[i] == '\t' )
	{
		i++;
	}

	return i;
}

typedef ptri (*PFN_scan)( const char* );

static const PFN_scan scanners[MAX_T] = {
	scan_strlit,
	scan_newln,
	scan_num,
	scan_direc,
	scan_label,
	scan_ident
};

static s32 strtos32( const char* in, ptri in_sz )
{
	long ret;

	char* str = uni_alloc( sizeof(char) * (in_sz + 1) );
	uni_memcpy( str, in, in_sz );

	if(in_sz >= 3 && str[0] == '0')
	{
		switch(str[1])
		{
		case 'x':
		case 'X':
			ret = strtol( str + 2, NULL, 16 );
			break;
		case 'o':
		case 'O':
			ret = strtol( str + 2, NULL, 8 );
			break;
		case 'b':
		case 'B':
			ret = strtol( str + 2, NULL, 2 );
			break;
		case '\0':
			return 0;
		default:
			ret = strtol( str, NULL, 10 );
			break;
		}
	}
	else
	{
		ret = strtol( str, NULL, 10 );
	}

	uni_free( str );

	return ret >= S32_MAX ? S32_MAX : (s32)ret;
}

struct tok* lex( const char* in )
{
	struct tok* ret;
	u8* str;
	ptri ret_sz = 0, ret_cap = 16;
	const ptri in_sz = uni_strlen( in );
	ptri i;

	if(!in)
	{
		uni_die( );
	}

	ret = uni_alloc( sizeof(struct tok) * ret_cap );
	str = (u8*)in;

	for(i = 0; i < in_sz; ++i)
	{
		ptri j;

		if(ret_sz >= ret_cap)
		{
			ret_cap <<= 1; /* *= 2 */
			ret = uni_realloc( ret, sizeof(struct tok) * ret_cap );
		}

		/* scan ahead for any whitespace first */
		i += scan_wspc( (const char*)(str + i) );

		for(j = 0; j < MAX_T; ++j)
		{
			/* run through token-producing scanners */
			const ptri len = scanners[j]( (const char*)(str + i) );

			if( len > 0 )
			{
				struct tok curtok;

				curtok.type = j;

				if( j == T_NUM )
				{
					s32 num = strtos32( (const char*)(str + i), len );
					curtok.data = uni_alloc( sizeof(s32) );
					*(s32*)curtok.data = num;
					curtok.data_sz = sizeof(s32);
				}
				else
				{
					curtok.data_sz = len + 1;
					curtok.data = uni_alloc( curtok.data_sz );
					uni_memcpy( curtok.data, (u8*)str + i, len );
					curtok.data[len] = '\0';
				}

				uni_memcpy( &(ret[ret_sz]), &curtok, sizeof(struct tok) );

				ret_sz++;
				i += len - 1;

				break;
			}
		}
	}

	{
		struct tok end;

		end.data = NULL;
		end.data_sz = 0;
		end.type = T_END;

		if(ret_sz >= ret_cap)
		{
			ret_cap <<= 1; /* *= 2 */
			ret = uni_realloc( ret, sizeof(struct tok) * ret_cap );
		}

		uni_memcpy( &(ret[ret_sz]), &end, sizeof(struct tok) );

		ret_sz++;
	}

	return ret;
}
