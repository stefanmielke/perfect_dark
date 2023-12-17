#ifndef _IN_NETMSG_H
#define _IN_NETMSG_H

#include <PR/ultratypes.h>
#include "net/net.h"
#include "net/netbuf.h"

#define SVC_BAD         0 // trash
#define SVC_NOP         1 // does nothing
#define SVC_AUTH        2 // auth response, sent in response to CLC_AUTH
#define SVC_CHAT        3 // chat message
#define SVC_STAGE_START 4 // start level
#define SVC_STAGE_END   5 // end level
#define SVC_PLAYER_MOVE 6 // player movement

#define CLC_BAD  0 // trash
#define CLC_NOP  1 // does nothing
#define CLC_AUTH 2 // auth request, sent immediately after connecting
#define CLC_CHAT 3 // chat message
#define CLC_MOVE 4 // player input

u32 netmsgClcAuthWrite(struct netbuf *dst);
u32 netmsgClcAuthRead(struct netbuf *src, struct netclient *srccl);
u32 netmsgClcMoveWrite(struct netbuf *dst);
u32 netmsgClcMoveRead(struct netbuf *src, struct netclient *srccl);

u32 netmsgSvcAuthWrite(struct netbuf *dst, struct netclient *authcl);
u32 netmsgSvcAuthRead(struct netbuf *src, struct netclient *srccl);
u32 netmsgSvcStageStartWrite(struct netbuf *dst);
u32 netmsgSvcStageStartRead(struct netbuf *src, struct netclient *srccl);
u32 netmsgSvcStageEndWrite(struct netbuf *dst);
u32 netmsgSvcStageEndRead(struct netbuf *src, struct netclient *srccl);
u32 netmsgSvcPlayerMoveWrite(struct netbuf *dst, struct netclient *movecl);
u32 netmsgSvcPlayerMoveRead(struct netbuf *src, struct netclient *srccl);

#endif
