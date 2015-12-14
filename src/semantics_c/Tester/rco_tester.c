////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 12/14/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#include "alarm/Alarm.h"
#include "semantics/rco.h"
#include <stdio.h>



typedef struct 
{
	int data1;
	char data2[10];
	aos_rco_define;
} chen_test_t;

void dest(void *data)
{
	printf("To delete object: %x\n", (unsigned int)data);
}

chen_test_t *
chen_test_constructor()
{
	chen_test_t *obj = malloc(sizeof(chen_test_t));
	aos_assert_r(obj, 0);
	aos_rco_init(obj, dest);
	obj->data1 = 0;
	return obj;
}



int rco_tester()
{
	chen_test_t *data1 = chen_test_constructor();
	chen_test_t *data2 = chen_test_constructor();

	chen_test_t *ptr = data1;	
	
	aos_rco_hold(&ptr, data1);
	aos_rco_dump(data1);

	aos_rco_put(&ptr, data1);
	ptr = 0;

	aos_rco_assign(&ptr, data2);

	aos_rco_dump(data1);
	aos_rco_dump(data2);
	return 0;
}

