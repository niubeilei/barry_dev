
#include <stdio.h>
#include <stdlib.h>
#include "KernelUtil/TArray.h"


void array_tester(void)
{
	void ** array;
	int ret;
	int i;

	ret = OmnTArray_init( &array, 4, 0 );
	if ( ret )
	{
		printf("array init error\n");
		exit(1);
	}

	for(i=0;i<1000;i++)
	{
		unsigned char tmp[4];
		unsigned char mask[4];
		tmp[0]=rand()%256;
		tmp[1]=rand()%256;
		tmp[2]=rand()%256;
		tmp[3]=rand()%256;
		mask[0]=mask[1]=mask[2]=mask[3]=255;
		OmnTArray_insert1( (void***)array,
				tmp, mask,0,0);

	}
}

int main( void )
{

	array_tester();
	return 0;
}


