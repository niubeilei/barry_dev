#ifndef AOS_GETLINE_H
#define AOS_GETLINE_H

#ifdef __cplusplus  
extern "C" {
#endif
/* unix systems can #define POSIX to use termios, otherwise 
 * the bsd or sysv interface will be used 
 */

#ifdef __STDC__ 
#include <stddef.h>

typedef size_t (*gl_strwidth_proc)(char *);

/*change here*/
/*extern int gl_sock;*/

char           *getlineplus(char *, char *);		/* read a line of input */
void            gl_setwidth(int);		/* specify width of screen */
void            gl_histadd(char *);		/* adds entries to hist */
void		gl_strwidth(gl_strwidth_proc);	/* to bind gl_strlen */

extern int 	(*gl_in_hook)(char *);
extern int 	(*gl_out_hook)(char *);
extern int	(*gl_tab_hook)(char *, int, int *);

#else	/* not __STDC__ */

char           *getlineplus();	
void            gl_setwidth();
void            gl_histadd();
void		gl_strwidth();

extern int 	(*gl_in_hook)();
extern int 	(*gl_out_hook)();
extern int	(*gl_tab_hook)();

#endif /* __STDC__ */

/*added by fei pei for cli control*/
extern int more_write(char *buf, int len, int line);
extern void get_scr_size(int signo);
extern int set_scr_size(void);
extern int init_scr(void);
extern void init_term_mode(int signo);
extern int set_term_mode(void);
extern void reset_term_mode(void);
extern int gl_getwc(void);
extern int get_sc_height(void);
extern int get_sc_width(void);
extern int num_of_char(char *str, char ch, int len);

/*end of fei pei's adding*/
#ifdef __cplusplus  
}
#endif

#endif /* AOS_GETLINE_H */


