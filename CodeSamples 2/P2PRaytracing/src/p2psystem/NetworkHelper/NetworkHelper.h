#ifndef __NETWORKHELPER__
#define __NETWORKHELPER__

#include "Peer.h"
#include <string>
#include "Raknet\RakPeerInterface.h"
#include "Raknet\BitStream.h"

class NetworkHelper
{
public:
	NetworkHelper(){}
	void Connect(Peer* currentPeer, std::string ipAddress);
	void EndConnection();
	void SendData(RakPeerInterface*,SystemAddress&, BitStream&);
};


#endif // !__NETWORKHELPER__
