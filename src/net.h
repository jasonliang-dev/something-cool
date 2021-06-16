#pragma once

#include "language.h"
#include <enet/enet.h>

typedef char *NetError;

void InitNet(void);

NetError InitServer(u16 port);
void CloseServer(void);
void ServerPollEvents(void);

void ClientConnect(const char *hostAddress, u16 port, void (*OnConnect)(NetError));
void ClientDisconnect(void);
void ClientForceDisconnect(void);
NetError ClientSend(const char *message);
void ClientOnReceive(void (*OnReceive)(ENetEvent));
void ClientOnDisconnect(void (*OnDisconnect)(void));
void ClientPollEvents(void);
