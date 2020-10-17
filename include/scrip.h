/****************************************************************************\
 *                                 scrip2o™                                 *
 *                                                                          *
 *                    Copyright © 2020 Alexander Nicholi                    *
 *                       Released under BSD-2-Clause.                       *
\****************************************************************************/

#ifndef INC_API__SCRIP_H
#define INC_API__SCRIP_H

#include <uni/types/int.h>

enum
{
	CMDSZ_1 = 0,
	CMDSZ_2
};

enum
{
	ARGSZ_1 = 0,
	ARGSZ_2,
	ARGSZ_4
};

struct scrip_instrdef
{
	u32 arg[128];
	u8 argsz[32];
	u8 argreq[16];
};

struct scrip_instrset
{
	u32 cmdsz : 1;
	u32 maxargs : 7;
	u32 cmd_ct : 16;
	struct scrip_instrdef* instr;
};

#endif /* INC_API__SCRIP_H */
