#include "facility.h"
#include <sys/utsname.h>
#include <string.h>


int ros_retrieve_system_id(char *data)
{
	struct utsname buf;
	uname( &buf );

	strncpy(data,buf.sysname,BUFLEN);
	return 0;
}

int ros_retrieve_motherboard_sn(char *data)
{
	strncpy(data,"JHJGNEHDKHGGJKDFHODIFHIDSIF",BUFLEN);

	return 0;
}

int ros_retrieve_hardware_info(struct ros_hardware_info *data)
{

	return 0;
}

int ros_retrieve_firmware_ver(char *version)
{
	strncpy(version,"VERSION 1.0",BUFLEN);
	return 0;
}

int ros_retrieve_sub_features(char **features)
{
	
	return 0;
}


