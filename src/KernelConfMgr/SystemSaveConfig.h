#ifndef  SYSTEM_SAVE_CONFIG_H
#define SYSTEM_SAVE_CONFIG_H

/*#define SAVE_CONFIG_DEBUG*/

#ifdef SAVE_CONFIG_DEBUG

#define CLI_EXE_LOCATION "/home/freedom/AOS/src/KernelInterface_exe/Cli.exe -cmd "
#define CLI_FILE_LOCATION "current"

#else

#define CLI_EXE_LOCATION "/usr/local/rhc/bin/Cli.exe -cmd "
#define CLI_FILE_LOCATION "current"

#endif

extern int system_save_config_register_clis(void);
extern int cli_system_save_config(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);
extern int cli_system_load_config(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);
extern int cli_system_clear_config(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);
extern int cli_system_save_config_for_webwall(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);
#endif
