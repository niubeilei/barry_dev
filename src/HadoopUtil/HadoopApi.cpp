////////////////////////////////////////////////////////////////////////////    
////                                                                              
//// Copyright (C) 2005                                                           
//// Packet Engineering, Inc. All rights reserved.                                
////                                                                              
//// Redistribution and use in source and binary forms, with or without           
//// modification is not permitted unless authorized in writing by a duly         
//// appointed officer of Packet Engineering, Inc. or its derivatives             
////                                                                              
//// File Name: HadoopApi.cpp                                               
//// Description:                                                                 
////                                                                              
////                                                                              
//// Modification History:                                                        
////                                                                              
//////////////////////////////////////////////////////////////////////////////    

#include "HadoopUtil/HadoopApi.h"                                         

int64_t AosReadHadoopFile(
        const AosRundataPtr &rdata,
        char *buff,
		hdfsFS &fs,
        hdfsFile &file,
        const int64_t start_pos,
        const int64_t bytes_to_read)
{
	signal(SIGPIPE, SIG_IGN);
	return hdfsPread(fs, file, start_pos, (void*)buff, bytes_to_read);
}

int64_t AosGetHadoopFileLength(
		const AosRundataPtr &rdata,
		hdfsFS &fs,
		hdfsFile &file)
{
	signal(SIGPIPE, SIG_IGN);
	return hdfsAvailable(fs, file); 
}

