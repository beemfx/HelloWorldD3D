/*********************************************************/
/**               Microsoft LAN Manager                 **/
/**       Copyright(c) Microsoft Corp., 1987-1999       **/
/**                                                     **/
/**     Rpc Error Codes from the compiler and runtime   **/
/**                                                     **/
/*********************************************************/

/*
If you change this file, you must also change rpcerr.h.
*/

#ifndef __RPCNTERR_H__
#define __RPCNTERR_H__

#if _MSC_VER > 1000
#pragma once
#endif

#define RPC_S_OK                          ERROR_SUCCESS
#define RPC_S_INVALID_ARG                 ERROR_INVALID_PARAMETER
#define RPC_S_OUT_OF_MEMORY               ERROR_OUTOFMEMORY
#define RPC_S_OUT_OF_THREADS              ERROR_MAX_THRDS_REACHED
#define RPC_S_INVALID_LEVEL               ERROR_INVALID_PARAMETER
#define RPC_S_BUFFER_TOO_SMALL            ERROR_INSUFFICIENT_BUFFER
#define RPC_S_INVALID_SECURITY_DESC       ERROR_INVALID_SECURITY_DESCR
#define RPC_S_ACCESS_DENIED               ERROR_ACCESS_DENIED
#define RPC_S_SERVER_OUT_OF_MEMORY        ERROR_NOT_ENOUGH_SERVER_MEMORY
#define RPC_S_ASYNC_CALL_PENDING          ERROR_IO_PENDING
#define RPC_S_UNKNOWN_PRINCIPAL           ERROR_NONE_MAPPED
#define RPC_S_TIMEOUT                     ERROR_TIMEOUT

#define RPC_X_NO_MEMORY                   RPC_S_OUT_OF_MEMORY
#define RPC_X_INVALID_BOUND               RPC_S_INVALID_BOUND
#define RPC_X_INVALID_TAG                 RPC_S_INVALID_TAG
#define RPC_X_ENUM_VALUE_TOO_LARGE        RPC_X_ENUM_VALUE_OUT_OF_RANGE
#define RPC_X_SS_CONTEXT_MISMATCH         ERROR_INVALID_HANDLE
#define RPC_X_INVALID_BUFFER              ERROR_INVALID_USER_BUFFER
#define RPC_X_PIPE_APP_MEMORY             ERROR_OUTOFMEMORY
#define RPC_X_INVALID_PIPE_OPERATION      RPC_X_WRONG_PIPE_ORDER

#endif /* __RPCNTERR_H__ */
