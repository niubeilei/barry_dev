////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemBridgeCli.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelInterface/CliSysCmd.h"
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/List.h"  
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
//#include "KernelInterface/CliProc.h"
//#include "KernelInterface/CliCmd.h"
//#include "KernelInterface/CliSysCmd.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"

#include <string.h>

#include "Global.h"

//kevin 07/28/2006 for uname
#include <sys/utsname.h>

// Global Para
struct aos_list_head sgBridgeConfigList;

int bridgeInit()
{
	AOS_INIT_LIST_HEAD(&sgBridgeConfigList);
	return 0;
}

int	bridgeGroupAdd(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;

	/* here real start */
	OmnString tmpName;
	OmnString cmd;
	
	struct bridgeConfigEntry * ptr;

//	curPos = mFileName.getWord(curPos, tmpName);
	tmpName = parms->mStrings[0];
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
    	if(ptr->bridgeName == tmpName) 
   		{
			rslt << "Error: The bridge already exists!\n";
			strncpy(errmsg,rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -1;
      	}
    }

	struct bridgeConfigEntry * pBCEntry;
	if((pBCEntry = new struct bridgeConfigEntry) == NULL)
	{
		rslt << "Error: Memory allocation failure!\n";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	pBCEntry->bridgeName = tmpName;
	
	pBCEntry->status = "down";
	//init array
	for(int i=0; i<16; i++)
	{
		pBCEntry->bridgeDev[i].dev = 0;
		pBCEntry->bridgeDev[i].ip = 0;
	}
	aos_list_add_tail((struct aos_list_head *)&pBCEntry->datalist, &sgBridgeConfigList);
	//AOS_INIT_LIST_HEAD(&bridgeDevList);

    cmd << "brctl addbr " << tmpName;
    OmnCliSysCmd::doShell(cmd, rslt);

	contents << "add new bridge name:" << pBCEntry->bridgeName << "\n";
	contents << "command: " << cmd << "\n";
	/* here real end */

	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}


int	bridgeGroupMemberAdd(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;

	/* here real start */
    OmnString cmd;
    OmnString tmpBridgeName;
    OmnString tmpDev;
    int status = 0;
	
	struct bridgeConfigEntry * ptr;

//	curPos = mFileName.getWord(curPos,tmpBridgeName);
//	curPos = mFileName.getWord(curPos, tmpDev);
	tmpBridgeName = parms->mStrings[0];
	tmpDev = parms->mStrings[1];
   // convert the outcard and incard to specific dev
	if(tmpDev == "outcard")
	{
		tmpDev = "eth0";
	}
	else if(tmpDev == "incard")
	{
		tmpDev = "br0";
	}
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {
    	if (ptr->bridgeName == tmpBridgeName)
		{
			// do bl array
			for(int i=0; i<16; i++)
			{
			
				if(ptr->bridgeDev[i].dev != 0)
				{
					if(ptr->bridgeDev[i].dev == tmpDev)
					{
						rslt << "Error: The device already exists!\n";
						strncpy(errmsg,rslt.data(),errlen-1);
						errmsg[errlen-1] = 0;
						return -1;
					}	
				}
				else if(ptr->bridgeDev[i].dev == 0)
				{
		    		ptr->bridgeDev[i].dev = tmpDev;
					status = 1;
					break;
				}
			}	 
			if (status == 1)
	   		{
	   			break;//goto success;
	   		}
	   		else
      	    {
				status = -1;
				rslt << "Error: You have added 16 devices!\n";
				strncpy(errmsg,rslt.data(),errlen-1);
				errmsg[errlen-1] = 0;
				return -1;
	  	    }
		}
	}

	//else
	if(status == 0)
	{
		rslt << "Error: The bridge does not exist!\n";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	// success
	//add ethernet interface to bridge and promisc ethernet interface!
	cmd << "brctl addif " << ptr->bridgeName << " " \
		<< tmpDev << "&&" << "ifconfig " \
		<< tmpDev << " down &&" << "ifconfig " \
		<< tmpDev << " 0.0.0.0 up;";
	OmnCliSysCmd::doShell(cmd, rslt);
	/* here real end */

	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}


int	bridgeGroupMemberDel(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;

	/* here real start */
    OmnString cmd;
    OmnString tmpBridgeName;
    OmnString tmpDev;
    int status = 0;
    int j = 0;
	
	struct bridgeConfigEntry * ptr;

//	curPos = mFileName.getWord(curPos,tmpBridgeName);
//	curPos = mFileName.getWord(curPos, tmpDev);
	tmpBridgeName = parms->mStrings[0];
	tmpDev = parms->mStrings[1];
   // convert the outcard and incard to specific dev
   if(tmpDev == "outcard")
	{
		tmpDev = "eth0";
	}
	else if(tmpDev == "incard")
	{
		tmpDev = "br0";
	}
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
		if (ptr->bridgeName == tmpBridgeName) 
		{
		// do bl array
			for(int i=0; i<16; i++)
			{
				if(ptr->bridgeDev[i].dev == tmpDev)
				{

					for(j=i; j< 15; j++)
					{
						 ptr->bridgeDev[j].dev = ptr->bridgeDev[j+1].dev; 		
						 ptr->bridgeDev[j].ip = ptr->bridgeDev[j+1].ip; 		
					}
					status = 1;
					ptr->bridgeDev[j].dev = "";
					ptr->bridgeDev[j].ip = "";

					break;
				}
				//else if(ptr->bridgeDev[i].dev == 0)
					
		    }
			if(status != 1)
			{
        		status = -1;
				rslt << "Error: The dev does not exist!\n";
				strncpy(errmsg,rslt.data(),errlen-1);
				errmsg[errlen-1] = 0;
				return -1;
			}
    	} 
		
	   if (status == 1)
	   {
			break;
	   } 
		//goto success;
    }
//success:
	if(status == 0)
	{
			rslt << "Error: The bridge does not exist!\n";
			strncpy(errmsg,rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -1;
	}

	//add ethernet interface to bridge and promisc ethernet interface!
    cmd << "brctl delif " << tmpBridgeName << " " << tmpDev;
	OmnCliSysCmd::doShell(cmd, rslt);
	/* here real end */

	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}


int	bridgeGroupDel(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;

	/* here real start */
	OmnString tmpName;
	OmnString cmd;
    int status = 0;	
	struct bridgeConfigEntry * ptr;

//	curPos = mFileName.getWord(curPos, tmpName);
	tmpName = parms->mStrings[0];
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
    	if(ptr->bridgeName == tmpName) 
   		{
			__aos_list_del((ptr->datalist.prev), (ptr->datalist.next));
		    status = 1;
	    	cmd << "ifconfig " << tmpName << " down &&" << "brctl delbr " << tmpName;
        	OmnCliSysCmd::doShell(cmd, rslt);
     
			delete ptr;
		    break;
      	}   
    }
   if(status == 0)
   {
		rslt << "Error: The bridge does not exist!\n";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
   }
	/* here real end */

	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}


int	bridgeGroupIp(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;

	/* here real start */
    OmnString cmd;
    OmnString tmpBridgeName;
    OmnString tmpIp;
    int status = 0;
	
	struct bridgeConfigEntry * ptr;

//	curPos = mFileName.getWord(curPos,tmpBridgeName);
    tmpBridgeName = parms->mStrings[0];
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
	
		if (ptr->bridgeName == tmpBridgeName) 
		{
			status = 1;
//			curPos = mFileName.getWord(curPos, tmpIp);
            tmpIp = parms->mStrings[1];
			ptr->ip = tmpIp;

			break;
    	}

	}

	if(status ==0 )
	{
		rslt << "Error: The bridge does not exist!\n";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	//set bridge ip !
	cmd << "ifconfig " << tmpBridgeName << " " << tmpIp ;
	contents << cmd << "\n";
	OmnCliSysCmd::doShell(cmd, rslt);
	/* here real end */

	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}


int	bridgeClearConfig(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;

	/* here real start */
    OmnString cmd;
	struct bridgeConfigEntry * ptr;

    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
       // down interface then del the bridge!
		if (ptr->bridgeName != 0)
		{
			cmd << "ifconfig " << ptr->bridgeName << " down && " << "brctl delbr " << ptr->bridgeName;
			contents << cmd << "\n";
			OmnCliSysCmd::doShell(cmd, rslt);
			// do bl array
			for(int i=0; i<16; i++)
			{
					ptr->bridgeDev[i].dev = "";
			}
		}
	}

	struct bridgeConfigEntry * tmp;

   	aos_list_for_each_entry_safe(ptr, tmp, &sgBridgeConfigList, datalist)
    {       
       	aos_list_del(&ptr->datalist);
        aos_free(ptr);
    }
	AOS_INIT_LIST_HEAD(&sgBridgeConfigList);

	/* here real end */

	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}


int	bridgeGroupMemberIp(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;

	/* here real start */
    OmnString cmd;
    OmnString tmpIp;
    OmnString tmpDev;
    int status = 0;
	
	struct bridgeConfigEntry * ptr;

//	curPos = mFileName.getWord(curPos,tmpDev);
	tmpDev = parms->mStrings[0];
	if(tmpDev == "outcard")
	{
		tmpDev = "eth0";
	}
	else if(tmpDev == "incard")
	{
		tmpDev = "br0";
	}
//	curPos = mFileName.getWord(curPos, tmpIp);
	tmpIp = parms->mStrings[1];
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
		
		if (ptr->bridgeName != 0) 
		{
		// do bl array
			for(int i=0; i<16; i++)
			{	
				if(ptr->bridgeDev[i].dev == tmpDev)
				{
					ptr->bridgeDev[i].ip = tmpIp;
			        status = 1;
					break;
				}	
		 	}
            
	
		}
	   if (status == 1)
	   {
	   		break;//goto success;
	   }
    }
//success:
   if(status != 1)
	{
		status = -1;
		rslt << "Error: The device not added in bridge or not exist!\n";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	//add ethernet interface to bridge and promisc ethernet interface!
    cmd << "ifconfig  " << tmpDev << " " << tmpIp << " up ;";
    contents << "command: " << cmd << "\n";
	OmnCliSysCmd::doShell(cmd, rslt);

	/* here real end */

	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}


// GB, 12/06/2006
int	bridgeShowConfig(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;

	/* here real start */
	OmnString bridgeInfo;
	struct bridgeConfigEntry * ptr;
    int i = 0;
    int status = 0;
 
	contents << "bridge       information" << "\n"
		<< "----------------------------" << "\n";
	aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
	{
	    if(0 != status)
		{
 			contents << "----------------------------" << "\n";
		}
  		contents << "            bridge name: " << ptr->bridgeName << "\n";
  		contents << "                    ip : " << ptr->ip << "\n";
  		contents << "                status : " << ptr->status << "\n";
  		contents << "\t\t\tDev\tIP" << "\n";
  		contents << "\t\t\t----------------------------" << "\n";
  		for (i = 0; i < 16; i++)
  		{
   			if (ptr->bridgeDev[i].dev == 0)
  		 	{	
    			break;
  		 	}
   			else
  		 	{
   				 contents << "                        "
		   			 	<< ptr->bridgeDev[i].dev << "\t"
		    			<< ptr->bridgeDev[i].ip << "\n";
  		 	}
 	 	}
		status++;
 	}
	/* here real end */

	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}

int	bridgeGroupUp(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;

	/* here real start */
    OmnString cmd;
    OmnString tmpBridgeName;
    OmnString tmpIp;
    int status = 0;
	
	struct bridgeConfigEntry * ptr;

//	curPos = mFileName.getWord(curPos,tmpBridgeName);
    tmpBridgeName = parms->mStrings[0];
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
	
		if (ptr->bridgeName == tmpBridgeName) 
		{
			status = 1;
            //set the bridge status is up
            ptr->status = "up";

			break;
    	}

	}

	if(status ==0 )
	{
		rslt << "Error: The bridge not exist!\n";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	//set bridge ip !
	cmd << "ifconfig " << tmpBridgeName << " " << "up && "<< "brctl stp " << tmpBridgeName << " on";
	contents << cmd << "\n";
	OmnCliSysCmd::doShell(cmd, rslt);
	/* here real end */

	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}


int	bridgeGroupDown(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;

	/* here real start */
    OmnString cmd;
    OmnString tmpBridgeName;
    OmnString tmpIp;
    int status = 0;
	
	struct bridgeConfigEntry * ptr;

    tmpBridgeName = parms->mStrings[0];
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
	
		if (ptr->bridgeName == tmpBridgeName) 
		{
			status = 1;
            //set the bridge status is up
            ptr->status = "down";

			break;
    	}

	}

	if(status ==0 )
	{
		rslt << "Error: The bridge not exist!\n";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	//set bridge ip !
	cmd << "ifconfig " << tmpBridgeName << " " << "down" ;
	contents << cmd << "\n";
	OmnCliSysCmd::doShell(cmd, rslt);
	/* here real end */

	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}

int	bridgeSaveConfig(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	
	OmnString contents;
	char local[4096];

	/* here real start */
	OmnString bridgeInfo;
	struct bridgeConfigEntry * ptr;
    int i = 0;
   
	aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
	{
	    if (ptr->bridgeName != "")
	    {
	    	contents <<"<Cmd>";      
			contents << "bridge group add " << ptr->bridgeName; 
			contents <<"</Cmd>\n"; 
	    	
	    	if (ptr->ip != "")
	    	{
	    		contents <<"<Cmd>";      
				contents << "bridge group set ip " << ptr->bridgeName << " " << ptr->ip; 
				contents <<"</Cmd>\n"; 
	   		}
	    	contents <<"<Cmd>";      
			contents << "bridge group " << ptr->status << " " 
			         << ptr->bridgeName; 
			contents <<"</Cmd>\n"; 
  		
  			for (i = 0; i < 16; i++)
  			{
   				if (ptr->bridgeDev[i].dev == 0)
  		 		{	
    				break;
  		 		}
   				else
  		 		{
   				 	
   				 	contents <<"<Cmd>";      
					contents << "bridge group member add " << ptr->bridgeName
							 << " " << ptr->bridgeDev[i].dev ; 
					contents <<"</Cmd>\n"; 
   
   				 	if ( ptr->bridgeDev[i].ip != "") 
   				 	{
   				 		contents <<"<Cmd>";      
						contents << "bridge member set ip " << ptr->bridgeDev[i].dev
								 << " " << ptr->bridgeDev[i].ip; 
						contents <<"</Cmd>\n"; 	
   				 	}                  
		   			 	
  		 		}
 	 		}
		}
 	}
 	
	strcpy(local, contents.data());
	
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;

	return 0;
}

int Bridge_regCliCmd(void)                                  
{	                                                           
	int ret;                                                   
	                                                           
	ret = CliUtil_regCliCmd("bridge group add",bridgeGroupAdd);
	ret |= CliUtil_regCliCmd("bridge group del",bridgeGroupDel);
	ret |= CliUtil_regCliCmd("bridge group member add",bridgeGroupMemberAdd);
	ret |= CliUtil_regCliCmd("bridge group set ip",bridgeGroupIp);
	ret |= CliUtil_regCliCmd("bridge show config",bridgeShowConfig);
	ret |= CliUtil_regCliCmd("bridge save config",bridgeSaveConfig);
	ret |= CliUtil_regCliCmd("bridge group member del",bridgeGroupMemberDel);
	ret |= CliUtil_regCliCmd("bridge clear config",bridgeClearConfig);
	ret |= CliUtil_regCliCmd("bridge group member set ip",bridgeGroupMemberIp);
	ret |= CliUtil_regCliCmd("bridge group up",bridgeGroupUp);
	ret |= CliUtil_regCliCmd("bridge group down",bridgeGroupDown);
	                                                           
	return ret;                                                
}                                                              
