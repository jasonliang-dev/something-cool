#pragma once

#include <enet/enet.h>

void ServerHandleConnect(ENetEvent event);
void ServerHandleRecieve(ENetEvent event);
void ServerHandleDisconnect(ENetEvent event);
