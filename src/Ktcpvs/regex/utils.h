////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: utils.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
/* utility definitions */
#ifdef _POSIX2_RE_DUP_MAX
#define	DUPMAX	_POSIX2_RE_DUP_MAX
#else
#define	DUPMAX	255
#endif
#define	INFINITY	(DUPMAX + 1)
#define	NC		(CHAR_MAX - CHAR_MIN + 1)
typedef unsigned char uch;

/* for old systems with bcopy() but no memmove() */
#ifdef USEBCOPY
#define	memmove(d, s, c)	bcopy(s, d, c)
#endif
