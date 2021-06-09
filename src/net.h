#pragma once

#include "language.h"
#include <enet/enet.h>

typedef char *NetError;

void InitNet(void);

NetError InitServer(u16 port);
void CloseServer(void);
void ServerPollEvents(void);

NetError ClientConnect(const char *hostAddress, u16 port);
void ClientDisconnect(void);
NetError ClientSend(const char *message);
void ClientPollEvents(void);
