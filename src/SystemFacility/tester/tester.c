
#include "SystemFacility/facility.h"
#include <stdio.h>

int main(int argc, char ** argv)
{
	char data[BUFLEN];

	ros_retrieve_system_id( data );

	printf("system id=%s\n",data);
	return 0;
}
