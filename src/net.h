#pragma once

#include "language.h"
#include <enet/enet.h>

typedef char *NetError;

void InitNet(void);

void InitServer(u16 port);
void ServerPollEvents(void);

void InitClient(void);
NetError ClientConnect(const char *host, u16 port);
NetError ClientSend(const char *message);
void ClientPollEvents(void);
