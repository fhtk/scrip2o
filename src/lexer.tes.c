/****************************************************************************\
 *                                 scrip2o™                                 *
 *                                                                          *
 *                    Copyright © 2020 Alexander Nicholi                    *
 *                       Released under BSD-2-Clause.                       *
\****************************************************************************/

#include <tes/battery.h>

#include <stdio.h>

#include "lexer.h"

static const char* hiker_scrip = "\n#import \"dungeon/granitecave/hiker1.snip\"\n#import \"dungeon/granitecave/hiker2.snip\"\nhiker:\n\tlock\n\tfaceplayer\n\tcheckflag RECV_HM05\n\tjumpif 1 hiker2\n\tsetorcopyvar 0x8000 HM05\n\tsetorcopyvar 0x8001 1\n\tcallstd 0\n\tsetflag RECV_HM05\n";

static const char* toknames[MAX_T + 1] = {
	"STRLIT",
	"NEWLN",
	"NUM",
	"DIREC",
	"LABEL",
	"IDENT",
	"END"
};

TES_OPEN( );
{

struct tok* tokens = NULL;
unsigned i;

tokens = lex( hiker_scrip );

TES_ASSERT_NE( tokens, NULL );

for(i = 0; tokens[i].type != T_END; ++i)
{
	if(tokens[i].type == T_NUM)
	{
		printf( "%s: %i\n", toknames[tokens[i].type], *(int*)(tokens[i].data) );
	}
	else if(tokens[i].type == T_NEWLN)
	{
		printf( "%s: \\n\n", toknames[tokens[i].type] );
	}
	else
	{
		printf( "%s: %s\n", toknames[tokens[i].type], tokens[i].data );
	}
}

TES_ASSERT_EQ( tokens[i].type, T_END );

}
TES_CLOSE( );
