////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ptrs.h
// Description:
//   
//
// Modification History:
// 12/08/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Semantics_ListSemantics_h
#define Aos_Semantics_ListSemantics_h

#ifndef AOS_SEMANTICS

#define aos_assert_list_consistency_linux()
#define aos_assert_list_consistency_stl()

#else

#define aos_assert_list_consistency_linux(list, size)			\
		aos_check_linux_list(list, size);

#define aos_assert_list_consistency_stl(list, size)				\
		aos_check_stl_list(list, size);

#define aos_list_integrity_check_generic(						\
				list, first, elem, next, size, ret_statement) 	\
		{														\
			int error = 0;
			void *found = OmnNew void *[size];
			if (!found)
			{
				aos_raise_error("Run out of memory");
			}
			else
			{
				int count = 0;
				while (count != size && elem)
				{
					for (int i=0; i<count; i++)
					{
						if (found[i] == (void *)elem)
						{
							aos_raise_error("loop found");
							error = 1;
							break;
						}
					}
					found[count++] = (void *)elem;
					elem = elem->next;
				}

				if (error) 
				{
					ret_statement;
				}

				if (count != size)
				{
					aos_raise_error("loop found");
					ret_statement;
				}

				if (elem && elem != first)
				{
					aos_raise_error("loop found");
					ret_statement;
				}
			}
		}

#endif

#endif

