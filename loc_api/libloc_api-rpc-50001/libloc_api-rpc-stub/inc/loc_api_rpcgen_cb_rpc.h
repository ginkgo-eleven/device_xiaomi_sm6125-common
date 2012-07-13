/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _LOC_API_CB_RPC_H_RPCGEN
#define _LOC_API_CB_RPC_H_RPCGEN

#include "librpc.h"
#include "commondefs_rpcgen_rpc.h"
#include "loc_api_rpcgen_common_rpc.h"

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif


struct rpc_loc_event_cb_f_type_args {
    rpc_uint32 cb_id;
    rpc_loc_client_handle_type loc_handle;
    rpc_loc_event_mask_type loc_event;
    rpc_loc_event_payload_u_type *loc_event_payload;
};
typedef struct rpc_loc_event_cb_f_type_args rpc_loc_event_cb_f_type_args;

struct rpc_loc_event_cb_f_type_rets {
    rpc_int32 loc_event_cb_f_type_result;
};
typedef struct rpc_loc_event_cb_f_type_rets rpc_loc_event_cb_f_type_rets;
#define LOC_APICBVERS 0x00050006

#define LOC_APICBPROG 0x3100008C
#define LOC_APICBVERS_0001 0x00050001

#if defined(__STDC__) || defined(__cplusplus)
#define rpc_loc_event_cb_f_type 1
extern  enum clnt_stat rpc_loc_event_cb_f_type_0x00050001(rpc_loc_event_cb_f_type_args *, rpc_loc_event_cb_f_type_rets *, CLIENT *);
extern  bool_t rpc_loc_event_cb_f_type_0x00050001_svc(rpc_loc_event_cb_f_type_args *, rpc_loc_event_cb_f_type_rets *, struct svc_req *);
extern int loc_apicbprog_0x00050001_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define rpc_loc_event_cb_f_type 1
extern  enum clnt_stat rpc_loc_event_cb_f_type_0x00050001();
extern  bool_t rpc_loc_event_cb_f_type_0x00050001_svc();
extern int loc_apicbprog_0x00050001_freeresult ();
#endif /* K&R C */
#define LOC_APICBVERS_0002 0x00050002

#if defined(__STDC__) || defined(__cplusplus)
#define rpc_loc_api_cb_null 0xffffff00
extern  enum clnt_stat rpc_loc_api_cb_null_0x00050002(void *, int *, CLIENT *);
extern  bool_t rpc_loc_api_cb_null_0x00050002_svc(void *, int *, struct svc_req *);
extern int loc_apicbprog_0x00050002_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define rpc_loc_api_cb_null 0xffffff00
extern  enum clnt_stat rpc_loc_api_cb_null_0x00050002();
extern  bool_t rpc_loc_api_cb_null_0x00050002_svc();
extern int loc_apicbprog_0x00050002_freeresult ();
#endif /* K&R C */
#define LOC_APICBVERS_0003 0x00050003

#if defined(__STDC__) || defined(__cplusplus)
extern  enum clnt_stat rpc_loc_api_cb_null_0x00050003(void *, int *, CLIENT *);
extern  bool_t rpc_loc_api_cb_null_0x00050003_svc(void *, int *, struct svc_req *);
extern int loc_apicbprog_0x00050003_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
extern  enum clnt_stat rpc_loc_api_cb_null_0x00050003();
extern  bool_t rpc_loc_api_cb_null_0x00050003_svc();
extern int loc_apicbprog_0x00050003_freeresult ();
#endif /* K&R C */
#define LOC_APICBVERS_0004 0x00050004

#if defined(__STDC__) || defined(__cplusplus)
extern  enum clnt_stat rpc_loc_api_cb_null_0x00050004(void *, int *, CLIENT *);
extern  bool_t rpc_loc_api_cb_null_0x00050004_svc(void *, int *, struct svc_req *);
extern int loc_apicbprog_0x00050004_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
extern  enum clnt_stat rpc_loc_api_cb_null_0x00050004();
extern  bool_t rpc_loc_api_cb_null_0x00050004_svc();
extern int loc_apicbprog_0x00050004_freeresult ();
#endif /* K&R C */
#define LOC_APICBVERS_0005 0x00050005

#if defined(__STDC__) || defined(__cplusplus)
extern  enum clnt_stat rpc_loc_api_cb_null_0x00050005(void *, int *, CLIENT *);
extern  bool_t rpc_loc_api_cb_null_0x00050005_svc(void *, int *, struct svc_req *);
extern int loc_apicbprog_0x00050005_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
extern  enum clnt_stat rpc_loc_api_cb_null_0x00050005();
extern  bool_t rpc_loc_api_cb_null_0x00050005_svc();
extern int loc_apicbprog_0x00050005_freeresult ();
#endif /* K&R C */
#define LOC_APICBVERS_0006 0x00050006

#if defined(__STDC__) || defined(__cplusplus)
extern  enum clnt_stat rpc_loc_api_cb_null_0x00050006(void *, int *, CLIENT *);
extern  bool_t rpc_loc_api_cb_null_0x00050006_svc(void *, int *, struct svc_req *);
extern int loc_apicbprog_0x00050006_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
extern  enum clnt_stat rpc_loc_api_cb_null_0x00050006();
extern  bool_t rpc_loc_api_cb_null_0x00050006_svc();
extern int loc_apicbprog_0x00050006_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_rpc_loc_event_cb_f_type_args (XDR *, rpc_loc_event_cb_f_type_args*);
extern  bool_t xdr_rpc_loc_event_cb_f_type_rets (XDR *, rpc_loc_event_cb_f_type_rets*);

#else /* K&R C */
extern bool_t xdr_rpc_loc_event_cb_f_type_args ();
extern bool_t xdr_rpc_loc_event_cb_f_type_rets ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_LOC_API_CB_RPC_H_RPCGEN */
