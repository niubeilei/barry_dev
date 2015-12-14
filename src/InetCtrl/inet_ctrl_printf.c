#include "inet_ctrl_printf.h"

#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/types.h>
#else
#ifndef NULL   
#define NULL    0 
#endif
#endif

struct _color_st {
	int color_index;
	char* color_str;
} g_colors[COLOR_MAX] = {
	{NONE,			"\033[0m"},
	{BLACK,			"\033[0;30m"},
	{DARK_GRAY,		"\033[1;30m"},
	{BLUE,			"\033[0;34m"},
	{LIGHT_BLUE,	"\033[1;34m"},
	{GREEN,			"\033[0;32m"},
	{LIGHT_GREEN,	"\033[1;32m"},
	{CYAN,			"\033[0;36m"},
	{LIGHT_CYAN,	"\033[1;36m"},
	{RED, 			"\033[0;31m"},
	{LIGHT_RED,		"\033[1;31m"},
	{PURPLE,		"\033[0;35m"},
	{LIGHT_PURPLE, 	"\033[1;35m"},
	{BROWN,			"\033[0;33m"},
	{YELLOW,		"\033[1;33m"},
	{LIGHT_GRAY,	"\033[0;37m"},
	{WHITE,			"\033[1;37m"},
};

char* get_color_str(int idx)
{
	int i;
	
	if (idx < NONE || idx >= COLOR_MAX)
		return NULL;
	
	if (idx == g_colors[idx].color_index)
		return g_colors[idx].color_str;

	for (i = NONE; i < COLOR_MAX; i++) {
		if (g_colors[i].color_index == idx)
			return g_colors[i].color_str;
	}
	
	return NULL;
}



