////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosProxyConn.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef __AOS_PROXY_CONN_H__
#define __AOS_PROXY_CONN_H__

#ifdef __KERNEL__
extern struct tcp_vs_conn * AosAppProxy_connCreate(struct socket *sock);
extern int AosAppProxy_connRelease(struct tcp_vs_conn *conn);

static inline int AosAppProxy_connPut(struct tcp_vs_conn *conn)
{
    if (atomic_dec_and_test(&conn->refcnt))
    {
        AosAppProxy_connRelease(conn);
    }

    return 0;
}

static inline int AosAppProxy_connHold(struct tcp_vs_conn *conn)
{
    atomic_inc(&conn->refcnt);
    return 0;
}
#else
static inline int AosAppProxy_connPut(struct tcp_vs_conn *conn)
{
    return 0;
}

static inline int AosAppProxy_connHold(struct tcp_vs_conn *conn)
{
    return 0;
}
#endif //ifdef __KERNEL__

#endif //#ifndef __AOS_PROXY_CONN_H__
