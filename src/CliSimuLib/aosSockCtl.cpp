////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSockCtl.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos_core/aosSockCtl.h"

#include "KernelSimu/socket.h"
#include "aos/aosCoreComm.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosKernelApi.h"
#include "aos/aosReturnCode.h"
#include "aosUtil/Tracer.h"
#include "KernelSimu/netfilter.h"
#include "KernelSimu/capability.h"
#include "KernelSimu/aosKernelMutex.h"
#include "KernelSimu/aosKernelDebug.h"
#include "KernelSimu/sched.h"
#include "KernelSimu/sched.h"
#include "KernelSimu/uaccess.h"
#include "aosUtil/Memory.h"

extern int AosCliSimu_kernelInit(void);


#define AOS_SO_ERRMSG_LENGTH 500

int aosSockSetopt(struct socket *sock, 
				  int cmd,
       			  char __user *user, 
				  unsigned int optlen)
{
    int ret = 0;
	unsigned int length = 0;
	char *userdata = 0;
	char errmsg[AOS_SO_ERRMSG_LENGTH];

	errmsg[0] = 0;

    // optlen > 128000 is a sanity check 
    if (optlen > 128000) 
	{
		strcpy(errmsg, "Data length too long");
		ret = aosAlarmInt(eAosAlarm, optlen);		
		goto out;
    }

	switch (cmd)
	{
    case AOS_SO_INIT_AOS:                                       
         ret = AosCliSimu_kernelInit();
         break;                                                
                                                              
    case AOS_SO_KAPI:                                        
         userdata = (char *) aos_malloc(optlen);  
aos_trace("Allocate memory: %x", userdata);
         if (!userdata)                                    
         {                                                
			 strcpy(errmsg, "Failed to allocate memory");
             ret = aosAlarm(eAosAlarmMemErr);            
             break;                                     
         }                                             
                                                             
         if (copy_from_user(userdata, user, optlen))
         {                                           
			 strcpy(errmsg, "Failed to copy from user");
             ret = aosAlarm(eAosAlarmCopyFromUserErr);       
             break;                                        
         }                                                
                                                                
         length = optlen;                               
         ret = OmnProcKernelApi(userdata, &length, errmsg, AOS_SO_ERRMSG_LENGTH);    
         break;                                        

	default:
		 strcpy(errmsg, "Unrecognized command");
		 ret = aosAlarmInt(eAosAlarm, cmd);
		 break;
	}

out:
	if (optlen == 0)
	{
		if (userdata)
		{
			aos_free(userdata);
		}
		return ret;
	}

    if (length > (unsigned int)optlen)                 
    {                                                 
        ret = aosAlarmInt2(eAosRc_ReturnDataTooLong,
                            length + 4, optlen);               
		length = optlen - 4;
    }

	if (length > 0 && userdata)
	{
    	((int*)userdata)[0] = length;          
    	if (copy_to_user(user, userdata, length+4))   
    	{                                              
        	ret = aosAlarm(eAosRc_CopyToUserErr);     
    	}                                            
	}
	else if (strlen(errmsg) > 0)
	{
		unsigned int errlen = strlen(errmsg);
		if (errlen+4 > optlen)
		{
			errlen = optlen-4;
		}

		if (copy_to_user(user, &errlen, sizeof(int)) ||
			copy_to_user(&((int*)user)[1], errmsg, errlen))
		{
			ret = aosAlarm(eAosRc_CopyToUserErr);
		}
	}
	else
	{
		length = 0;
		copy_to_user(user, &length, sizeof(int));
	}

aos_trace("To release: %x", userdata);
    aos_free(userdata);                       
	userdata = 0;
	return ret;
}


