////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BindErr.cpp
// Description:
//	This function translates error code into its string. These errors
//  are from bind(...).
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Porting/BindErr.h"

#ifdef OMN_PLATFORM_UNIX

#include <errno.h>


OmnString	
OmnGetBindErr(const int errcode)
{
	switch (errcode)
	{
    case EBADF:
		 return "sockfd is not a valid descriptor";

    case EINVAL:
		 return "The socket is already bound to an address.  This  may  change  in"
              "the future: see linux/unix/sock.c for details. Or"
       		  "the addrlen is wrong, or the socket was not in the AF_UNIX family.";

    case EACCES:
		 return "The address is protected, and the user is not the super-user. Or"
    			"Search permission is denied on a component of the path prefix.";

    case ENOTSOCK:
         return "Argument is a descriptor for a file, not a socket.";


    case EROFS:  
		 return "The socket inode would reside on a read-only file system.";

    case EFAULT:
		 return "my_addr points outside the user's accessible address space.";

    case ENAMETOOLONG:
         return "my_addr is too long.";

    case ENOENT:
		 return "The file does not exist.";

    case ENOMEM:
		 return "Insufficient kernel memory was available.";

    case ENOTDIR:
		 return "A component of the path prefix is not a directory.";

    case ELOOP:
		 return "Too many symbolic links were enco";

	default:
		 return OmnString("Unrecognized bind error code: ") << errcode;
	}
}

#endif

