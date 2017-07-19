#pragma once 

#include <string>
#include <boost\tokenizer.hpp>
#include <boost\lexical_cast.hpp>
#include <boost\asio.hpp>

#include "..\Peer\CommunicationUtil.h"

using std::string;
namespace Network
{
	///-------------------------------------------------------------------------------------------------
	/// <summary>	An oracle interactor interface. </summary>
	///
	/// <remarks>	Adrian, 05/12/2015. </remarks>
	///-------------------------------------------------------------------------------------------------

	class IOracleInteractor
	{
	public:
		virtual void GetUpdates() = 0;
		virtual void SendUpdates(string p_update) = 0;
	protected:
		string m_serverIp;
		int m_serverPortNumber;
	};
}