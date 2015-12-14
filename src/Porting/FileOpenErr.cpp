////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FileOpenErr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Porting/FileOpenErr.h"

#ifdef OMN_PLATFORM_UNIX

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>



OmnString 
OmnGetFileOpenError(const int c)
{
	switch (c)
	{
	case EEXIST:
		 return "Pathname already exists and O_CREAT and O_EXCL were used.";

	case EISDIR: 
		 return "Pathname  refers  to  a  directory  and  the access"
				" requested involved writing (that  is,  O_WRONLY  or"
				" O_RDWR is set).";

	case EACCES:
		 return "The requested access to the file is not allowed, or"
				" one of the directories in pathname  did  not  allow"
				" search  (execute)  permission,  or the file did not" 
				" exist yet and write access to the parent  directory" 
				" is not allowed.";

	case ENAMETOOLONG: 
		 return "Pathname was too long.";

	case ENOENT:
		 return "O_CREAT  is  not  set  and  the named file does not "
				" exist.  Or, a directory component in pathname  does" 
				" not exist or is a dangling symbolic link.";

	case ENOTDIR: 
		 return "A component used as a directory in pathname is not," 
				" in fact, a directory, or O_DIRECTORY was  specified" 
				" and pathname was not a directory.";

	case ENXIO:
	     return "O_NONBLOCK  |  O_WRONLY is set, the named file is a" 
				" FIFO and no process has the file open for  reading." 
				" Or, the file is a device special file and no "
				" corresponding device exists.";

	case ENODEV:
		 return "Pathname refers to a device  special  file  and  no "
				"corresponding device exists.  (This is a Linux kernel "
				"bug  -  in  this  situation  ENXIO   must   be returned.)";

	case EROFS:
	     return	"Pathname refers to a file on a read-only filesystem "
				"and write access was requested.";

	case ETXTBSY: 
		 return "Pathname refers to an  executable  image  which  is "
				"currently  being  executed  and  write  access  was "
				"requested.";

	case EFAULT:
		 return "Pathname points  outside  your  accessible  address space.";

	case ELOOP:
	     return "Too many symbolic links were encountered in resolving "
				"pathname, or O_NOFOLLOW was specified but pathname was "
				"a symbolic link."; 
	
	case ENOSPC:
		 return "Pathname  was to be created but the device containing "
				"pathname has no room for the New file."; 
	
	case ENOMEM: 
		 return "Insufficient kernel memory was available.";  

	case EMFILE:
		 return "The process already has the maximum number of files open.";

	case ENFILE:
		 return "The  limit on the total number of files open on the "
				"system has been reached.";

	default:
		 return OmnString("Unrecognized error code: ") << c;
	}
}

#elif OMN_PLATFORM_MICROSOFT
#define OMN_E_RESOURCE_N_A EDEADLOCK
#endif


