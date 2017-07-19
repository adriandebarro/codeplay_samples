/*
{
Packet *packet;
//update thread
//std::thread updateThread(&Peer::SendUpdate, this);
//InitPool();
HelperClass helperClass;

if (newToNetwork)
{
peerData->Connect(bootStrapAddress.c_str(), LISTENING_PORT, 0, 0);
}

while (1)
{
for (packet = peerData->Receive(); packet; peerData->DeallocatePacket(packet), packet = peerData->Receive())
{
switch (packet->data[0])
{
case ID_REMOTE_DISCONNECTION_NOTIFICATION:
printf("Another client has disconnected.\n");
break;

case ID_REMOTE_CONNECTION_LOST:
printf("Another client has lost the connection.\n");
break;

case ID_REMOTE_NEW_INCOMING_CONNECTION:
printf("Another client has connected.\n");
break;

//connection accepted by a peer
case ID_CONNECTION_REQUEST_ACCEPTED:
{
printf("Our connection request has been accepted");

//check if we need to exchnage neighbors
if (newToNetwork)
{
AddNeighbor(packet->guid, packet->systemAddress);
HandShakeRequest(packet->systemAddress);
newToNetwork = !newToNetwork;
}
}

printf("Our connection request has been accepted.\n");
break;

case ID_NEW_INCOMING_CONNECTION:
printf("A connection is incoming %s .\n", packet->systemAddress.ToString());
break;

case ID_NO_FREE_INCOMING_CONNECTIONS:
printf("The server is full .\n");
break;
case ID_DISCONNECTION_NOTIFICATION:
printf("We have been disconnected.\n");
break;
case ID_CONNECTION_LOST:
printf("Connection lost.\n");
//RemoveNeighbour(packet->systemAddress.ToString());
break;
//recieving update from bootstrpa request
case RECIEVE_BOOT_STRAP_REPLY:
{
Packet currentPacket = *packet;
DealWithBootStrap(currentPacket);
if ((portNumber - LISTENING_PORT) == 4)
{
RunUpdateProcedure();
}
}
break;

//recieving a handshake request
case  BOOT_HAND_SHAKE_REQUEST:
{
DealHandShake(packet->systemAddress, packet->guid);
}
break;

case HAND_SHAKE_INIT_UPDATE:
{
UpdateHandShakeInit(GetStringFromBitStream(*packet), packet->systemAddress);
}
break;

case HAND_SHAKE_REPLY_PEER:
{
//recieve peers parse them
UpdatePeerReplyer(GetStringFromBitStream(*packet), packet->systemAddress);
}
break;

case SHARE_LOCAL_UPDATES:
{
ProcessUpdatesRecieved(GetStringFromBitStream(*packet), packet->systemAddress);
}
break;

case SHARE_LOCAL_UPDATES_REPLY:
{
ReplyToUpdate(GetStringFromBitStream(*packet), packet->systemAddress);
}
break;

case TEST_MESSAGE:
printf("Incoming message");
break;

default:
//printf("Message with identifier %i has arrived. %s \n", packet->data[0], packet->data);
break;
}
}
}
}


*/

#pragma once 

#include <string>

#include "AbstractPeer.h"

namespace Network
{
	namespace Peer
	{
		class P2pPeer : public AbstarctPeer
		{
		public:
			P2pPeer() :AbstarctPeer()
			{

			}

			P2pPeer(string p_ipAddress, int p_socketNumber, string p_nameAssigned) : AbstarctPeer(p_ipAddress, p_socketNumber, p_nameAssigned)
			{

				BootUpSequence();
			}

			using AbstarctPeer::BootUpSequence;
			virtual void BootUpSequence()
			{

			}


			using AbstarctPeer::ListenOnNetwork;
			virtual void ListenOnNetwork()
			{

			}

			using AbstarctPeer::UpdateFunction;
			virtual void UpdateFunction()
			{

			}
		private:
		};
	}
}