////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 12/06/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "semanobj/array_so.h"

#include "alarm/Alarm.h"
#include "random/random_util.h"
#include <stdio.h>

int aos_array_tester_1dim();
int aos_array_tester_2dim();
int aos_array_tester_3dim();
int aos_array_tester_4dim();
int aos_array_tester_5dim();

int 
aos_array_tester()
{
	// aos_array_tester_1dim();
	aos_array_tester_2dim();
	// aos_array_tester_3dim();
	// aos_array_tester_4dim();
	// aos_array_tester_5dim();
	return 0;
}


int aos_array_tester_1dim()
{
	int *a = aos_array_create("test", int, "int", 10, 1, 10, 0);

	aos_array_elem_set1(a, 0, 50);
	int i = aos_array_elem_read1(a, 0);
	printf("i = %d\n", i);

	aos_array_elem_set1(a, 9, 5);
	i = aos_array_elem_read1(a, 9);
	printf("i = %d\n", i);

	aos_array_print_stat(a);

	aos_array_decl(aaa, "aaa", int, "int", 50);
	for (i=0; i<50; i++)
	{
		aaa[i] = i;
	}

	for (i=0; i<50; i++)
	{
		printf("aaa = %d\n", aaa[i]);
	}

	// 
	// Stack arrays
	//
	aos_array_decl(array1, "array1", int, "int", 10);
	for (i=0; i<10; i++)
	{
		array1[i] = i;
	}

	for (i=0; i<10; i++)
	{
		if (array1[i] != i)
		{
			printf("**********Error: %d:%d\n", i, array1[i]);
		}
	}

	return 0;
} 


int aos_array_tester_2dim()
{
	int i, j;
	int **aaa = aos_array_create2("aaa", int, "int", 10, 20);
	for (i=0; i<10; i++)
	{
		for (j=0; j<20; j++)
		{
			aaa[i][j] = i * j;
		}
	}

	for (i=0; i<10; i++)
	{
		for (j=0; j<20; j++)
		{
			printf("aaa[i][j] = %d\n", aaa[i][j]);
		}
	}

	aos_array_decl2(array1, "array1", int, "int", 50, 10);
	for (i=0; i<50; i++)
	{
		for (j=0; j<10; j++)
		{
			array1[i][j] = i * j;
		}
	}

	for (i=0; i<50; i++)
	{
		for (j=0; j<10; j++)
		{
			if (array1[i][j] != i * j)
			{
				printf("***********Error: %d:%d:%d\n", i, j, array1[i][j]);
			}
		}
	}

	return 0;
}


int aos_array_tester_4dim()
{
	int i, j, k, l, index;
	int idx1, idx2, idx3, idx4;

	idx1 = aos_next_int(1, 200);
	idx2 = aos_next_int(1, 200);
	idx3 = aos_next_int(1, 200);
	idx4 = aos_next_int(1, 200);

	int ****aaa = aos_array_create4("aaa", int, "int", 
			idx1, idx2, idx3, idx4);
	for (i=0; i<idx1; i++)
	{
		for (j=0; j<idx2; j++)
		{
			for (k=0; k<idx3; k++)
			{
				for (l=0; l<idx4; l++)
				{
					aaa[i][j][k][l] = i * j * k * l;
				}
			}
		}
	}

	for (index=0; index<10000; index++)
	{
		if (index % 100 == 0) printf("index = %d\n", index);
		i = aos_next_int(0, idx1-1);
		j = aos_next_int(0, idx2-1);
		k = aos_next_int(0, idx3-1);
		l = aos_next_int(0, idx4-1);
		if (aaa[i][j][k][l] != i * j * k * l)
		{
			printf("********** Failed: %d:%d:%d:%d:%d\n", 
				i, j, k, l, aaa[i][j][k][l]);
		}
	}

	return 0;
}


int aos_array_tester_5dim()
{
	int i, j, k, l, m, index;
	int idx1, idx2, idx3, idx4, idx5;

	idx1 = aos_next_int(1, 20);
	idx2 = aos_next_int(1, 20);
	idx3 = aos_next_int(1, 20);
	idx4 = aos_next_int(1, 20);
	idx5 = aos_next_int(1, 20);

	printf("Indices: %d:%d:%d:%d:%d\n", idx1, idx2, idx3, idx4, idx5);
	int *****aaa = aos_array_create5("aaa", int, "int", 
			idx1, idx2, idx3, idx4, idx5);
	for (i=0; i<idx1; i++)
	{
		for (j=0; j<idx2; j++)
		{
			for (k=0; k<idx3; k++)
			{
				for (l=0; l<idx4; l++)
				{
					for (m=0; m<idx5; m++)
					{
						aaa[i][j][k][l][m] = i * j * k * l * m;
					}
				}
			}
		}
	}

	for (index=0; index<1000000; index++)
	{
		if (index % 1000 == 0) printf("index = %d\n", index);
		i = aos_next_int(0, idx1-1);
		j = aos_next_int(0, idx2-1);
		k = aos_next_int(0, idx3-1);
		l = aos_next_int(0, idx4-1);
		m = aos_next_int(0, idx5-1);
		if (aaa[i][j][k][l][m] != i * j * k * l * m)
		{
			printf("********** Failed: %d:%d:%d:%d:%d:%d\n", 
				i, j, k, l, m, aaa[i][j][k][l][m]);
		}
	}

	return 0;
}


