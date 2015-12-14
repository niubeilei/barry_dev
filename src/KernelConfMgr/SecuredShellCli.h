////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SecuredShellCli.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef SystemCli_SecuredShellCli_h
#define SystemCli_SecuredShellCli_h

extern int securedShellCreateShell(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms,	char *errmsg, const int errlen);


extern int securedShellStartShell(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);


extern int securedShellStopShell(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg,	const int errlen);


extern int securedShellRemoveCommands(char *data, unsigned int *optlen,	struct aosUserLandApiParms *parms,char *errmsg, const int errlen);


extern int securedShellRestoreCommands(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int securedShellClearShell(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);
#endif
