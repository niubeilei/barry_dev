#ifndef INET_CTRL_PRINTF_H
#define INET_CTRL_PRINTF_H

enum {
	NONE = 0,
	BLACK,
	DARK_GRAY,
	BLUE,
	LIGHT_BLUE,
	GREEN,
	LIGHT_GREEN,
	CYAN,
	LIGHT_CYAN,
	RED,
	LIGHT_RED,
	PURPLE,
	LIGHT_PURPLE,
	BROWN,
	YELLOW,
	LIGHT_GRAY,
	WHITE,
	COLOR_MAX
};

char* get_color_str(int idx);

#ifdef	__MY_COLOR__
#define my_sprintf(color, str, x...) do { \
	strcpy(str, get_color_str(color)); \
	sprintf(str+strlen(str), x); \
	strcat(str, get_color_str(NONE)); \
} while(0)
#else
#define my_sprintf(color, str, x...) do { \
	sprintf(str, x); \
} while(0)
#endif  /*#ifdef	__MY_COLOR__ */

#endif

