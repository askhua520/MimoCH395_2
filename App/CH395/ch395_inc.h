/**
 * @file    ch395_inc.h
 * @brief   CH395 command and register definitions
 */

#ifndef __CH395_INC_H
#define __CH395_INC_H

/* CH395 commands */
#define CMD_RESET_ALL           0x05
#define CMD_CHECK_EXIST         0x06
#define CMD_SET_MAC_ADDR        0x21
#define CMD_SET_IP_ADDR         0x22
#define CMD_SET_GWIP_ADDR       0x23
#define CMD_SET_MASK_ADDR       0x24
#define CMD_SET_TCP_SERVER_PORT 0x25
#define CMD_SET_DHCP_ENABLE     0x26
#define CMD_GET_IP_STATUS       0x27
#define CMD_GET_DHCP_STATUS     0x28
#define CMD_GET_IP_ADDR         0x29
#define CMD_GET_GWIP_ADDR       0x2A
#define CMD_GET_MASK_ADDR       0x2B
#define CMD_GET_REMOT_IP        0x2C
#define CMD_GET_REMOT_PORT      0x2D
#define CMD_GET_SOCKET_STATUS   0x2E
#define CMD_TCP_CONNECT         0x30
#define CMD_TCP_LISTEN          0x31
#define CMD_TCP_DISCONNECT      0x32
#define CMD_TCP_SEND            0x33
#define CMD_GET_RECV_LEN        0x34
#define CMD_GET_RECV_DATA       0x35
#define CMD_SET_PROTO_TYPE      0x36
#define CMD_SET_SOCKET_DEST_IP  0x37
#define CMD_SET_SOCKET_DEST_PORT 0x38
#define CMD_SET_SOCKET_SOUR_PORT 0x39
#define CMD_OPEN_SOCKET         0x3A
#define CMD_CLOSE_SOCKET        0x3B
#define CMD_GET_GLOBAL_INT      0x3C
#define CMD_SET_SOCKET_TTL      0x3D
#define CMD_GET_SOCKET_TTL      0x3E

/* Socket protocol types */
#define PROTO_TYPE_TCP          0x01
#define PROTO_TYPE_UDP          0x02

/* Socket status */
#define SOCK_STATUS_CLOSED      0x00
#define SOCK_STATUS_OPEN        0x01
#define SOCK_STATUS_LISTEN      0x02
#define SOCK_STATUS_ESTABLISHED 0x03
#define SOCK_STATUS_CLOSE_WAIT  0x04
#define SOCK_STATUS_SYN_SENT    0x05
#define SOCK_STATUS_FIN_WAIT    0x06

/* Global interrupt flags */
#define GINT_FLAG_SOCK0         0x01
#define GINT_FLAG_SOCK1         0x02
#define GINT_FLAG_SOCK2         0x04
#define GINT_FLAG_SOCK3         0x08
#define GINT_FLAG_DHCP          0x10
#define GINT_FLAG_PHY           0x20

#endif /* __CH395_INC_H */
