#pragma once

#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <thread>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#include <boost\tokenizer.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/asio.hpp>

#include <string>
#include <math.h>
#include <vector>
#include <iostream>

typedef boost::tokenizer<boost::char_separator<char>> tokeniz;

using namespace  boost::uuids;
using namespace boost::asio::ip;

typedef boost::system::error_code BoostSystemErrorCode;
typedef boost::asio::ip::tcp::socket Socket;
typedef boost::asio::io_service IoSrevice;

using std::cout;
using std::endl;

namespace Network
{
	///-------------------------------------------------------------------------------------------------
	/// <summary>	A helper class. </summary>
	///
	/// <remarks>	Adrian, 31/08/2015. </remarks>
	///-------------------------------------------------------------------------------------------------

	class HelperClass
	{
	private:
		union byteInt
		{
			int representingNumber;
			unsigned char rep[sizeof(int)];
		};

	public:

		static ULONG PingUtil(string p_ip, PICMP_ECHO_REPLY& p_result)
		{
			HANDLE hIcmpFile;
			unsigned long ipaddr = INADDR_NONE;
			DWORD dwRetVal = 0;
			char SendData[32] = "Data Buffer";
			LPVOID ReplyBuffer = NULL;
			DWORD ReplySize = 0;


			ipaddr = inet_addr(p_ip.c_str());
			if (ipaddr == INADDR_NONE) {
				return -5;
			}

			hIcmpFile = IcmpCreateFile();
			if (hIcmpFile == INVALID_HANDLE_VALUE) {
				printf("Could not connect to host ip\n", GetLastError());
				return -5;
			}

			ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
			ReplyBuffer = (VOID*)malloc(ReplySize);
			if (ReplyBuffer == NULL) {
				printf("\tUnable to allocate memory\n");
				return -5;
			}


			dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData),
				NULL, ReplyBuffer, ReplySize, 1000);
			if (dwRetVal != 0) {
				p_result = (PICMP_ECHO_REPLY)ReplyBuffer;
				struct in_addr ReplyAddr;
				ReplyAddr.S_un.S_addr = p_result->Address;
				return p_result->RoundTripTime;
			}
			else {
				printf("\tCall to IcmpSendEcho failed.\n");
				printf("\tIcmpSendEcho returned error: %ld\n", GetLastError());
				return -5;
			}
		}


		///-------------------------------------------------------------------------------------------------
		/// <summary>	Parse string sentence. </summary>
		///
		/// <remarks>	Adrian, 31/08/2015. </remarks>
		///
		/// <param name="toParse">		  	to parse. </param>
		/// <param name="splittingString">	The splitting string. </param>
		/// <param name="output">		  	The output. </param>
		///-------------------------------------------------------------------------------------------------

		static void ParseStringSentence(std::string toParse, std::string splittingString, std::string output[])
		{
			boost::char_separator<char> sep(splittingString.c_str());
			tokeniz tok(toParse, sep);


			int index = 0;
			for (tokeniz::iterator tokIter = tok.begin(); tokIter != tok.end(); ++tokIter)
			{
				output[index] = tokIter.current_token().c_str();
				++index;
			}
		}

		static void ParseStringSentence(std::string toParse, std::string splittingString, std::vector<std::string>& output)
		{
			boost::char_separator<char> sep(splittingString.c_str());
			tokeniz tok(toParse, sep);

			for (tokeniz::iterator tokIter = tok.begin(); tokIter != tok.end(); ++tokIter)
			{
				output.push_back(tokIter.current_token().c_str());
			}
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Logxes. </summary>
		///
		/// <remarks>	Adrian, 31/08/2015. </remarks>
		///
		/// <param name="x">	The x coordinate. </param>
		/// <param name="a">	The double to process. </param>
		///
		/// <returns>	A double. </returns>
		///-------------------------------------------------------------------------------------------------

		static double logx(double x, double a)
		{
			return log(x) / log(a);
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Sets amoun time sleep. </summary>
		///
		/// <remarks>	Adrian, 31/08/2015. </remarks>
		///
		/// <param name="numberPeers">	Number of peers. </param>
		///
		/// <returns>	An int. </returns>
		///-------------------------------------------------------------------------------------------------

		static int SetAmounTimeSleep(const int& numberPeers)
		{
			return (int)(logx(numberPeers, 2) + logx(numberPeers, 2.718281828459045235));
		}

		static void AddIntBytes(vector<unsigned char>& p_toAddVector, int p_toConvert)
		{
			unsigned char* toConvert = (unsigned char*)&p_toConvert;

			for (int index = 0; index < 4; index++)
			{
				p_toAddVector.push_back(toConvert[index]);
			}
		}

		static void AddFloatBytes(vector<unsigned char>& p_toAddVector, float p_toConvert)
		{
			unsigned char* toConvert = (unsigned char*)&p_toConvert;

			for (int index = 0; index < 4; index++)
			{
				p_toAddVector.push_back(toConvert[index]);
			}
		}

		static void ByteArrayToInt(int& p_output, vector<unsigned char>& p_array)
		{
			byteInt temp;

			for (int index = 0; index < sizeof(int); index++)
			{
				temp.rep[index] = p_array[index];
			}

			p_output = temp.representingNumber;
		}

		static void ByteArrayToInt(int& p_output, vector<unsigned char>& p_array, int& p_startingPoint)
		{
			p_output = *(int*)&p_array[p_startingPoint];
			p_startingPoint += sizeof(int);
		}


		static uuid GetUuid(std::vector<unsigned char>& p_byteArray, int& p_currentPosition)
		{
			uuid output = *(uuid*)&p_byteArray[p_currentPosition];
			p_currentPosition += sizeof(uuid);
			return output;
		}

		static void GetByteArrayFromLong(std::vector<unsigned char>& p_byteArray, unsigned long p_longNumber)
		{
			unsigned char* tempPtr = (unsigned char*)&p_longNumber;

			for (int index = 0; index < sizeof(unsigned long); index++)
			{
				p_byteArray.push_back(tempPtr[index]);
			}

		}

		static unsigned long GetLongFromByte(std::vector<unsigned char>& p_byteArray, int& p_currentPosition)
		{
			long output = *(long*)& p_byteArray[p_currentPosition];
			p_currentPosition += sizeof(unsigned long);
			return output;
		}

		static void UuidToByteArray(std::vector<unsigned char>& p_byteArray, uuid p_uuid)
		{
			unsigned char* tempPtr = (unsigned char*)&p_uuid;

			for (int index = 0; index < sizeof(uuid); index++)
			{
				p_byteArray.push_back(tempPtr[index]);
			}
		}

		static bool CloseConnection(Socket &socket)
		{
			try
			{
				BoostSystemErrorCode ec;
				socket.close(ec);

				if (ec)
					return false;
				else
					return true;
			}
			catch (std::exception ex)
			{
				cout << " Error occured" << ex.what() << endl;
				return false;
			}
		}

		static bool OpenConnection(char* p_host, char* p_port, Socket &p_socket, IoSrevice &aios)
		{
			/*boost::asio::io_service aios;
			boost::asio::ip::tcp::socket sockett(aios);*/
			try
			{
				cout << "connecting to " << p_host << ":::" << p_port << endl;
				boost::system::error_code ec;
				boost::asio::ip::tcp::resolver resolver(aios);
				boost::asio::ip::tcp::resolver::iterator endpoint = resolver.resolve(boost::asio::ip::tcp::resolver::query(p_host, p_port));
				boost::asio::connect(p_socket, endpoint, ec);
				if (ec)
				{
					cout << ec.message() << endl;
					return false;
				}

				return true;
			}
			catch (std::exception e)
			{
				cout << e.what() << endl;
				return false;
			}
		}


		static bool OpenConnection(const char* p_host, const char* p_port, Socket &p_socket, IoSrevice &aios)
		{
			try
			{
				boost::system::error_code ec;
				boost::asio::ip::tcp::resolver resolver(aios);
				boost::asio::ip::tcp::resolver::iterator endpoint = resolver.resolve(boost::asio::ip::tcp::resolver::query(p_host, p_port));
				boost::asio::connect(p_socket, endpoint, ec);
				if (ec)
				{
					cout << ec.message() << endl;
					return false;
				}

				return true;
			}
			catch (std::exception e)
			{
				cout << e.what() << endl;
				return false;
			}
		}

		static void AddDataToVector(vector<unsigned char>& p_outputArray, std::array<unsigned char, 2048>& p_buf, size_t len, int& amount)
		{
			for (int index = 0; index < len; index++)
			{
				p_outputArray.push_back(p_buf[index]);
				amount--;
			}
		}

		static void ReceiveData(boost::asio::ip::tcp::socket &socket, vector<unsigned char>& p_toOutput)
		{
			boost::system::error_code error;
			std::array<unsigned char, 4> numberArray;

			int amount = 0;

			unsigned char* amountTest = (unsigned char*)&amount;

			size_t len = socket.read_some(boost::asio::buffer(numberArray), error);

			for (int index = 0; index < 4; index++)
			{
				amountTest[index] = numberArray[index];
			}

			for (;;)
			{
				std::array <unsigned char, 2048> buf;

				size_t len = socket.read_some(boost::asio::buffer(buf), error);

				if (error == boost::asio::error::eof)
				{
					break;
				}
				else if (error)
				{
					
					std::cout << "Error occured during communication" << socket.remote_endpoint().port() << std::endl;
					std::cout << error.message() << std::endl;
					break;
				}
				else
				{
					AddDataToVector(p_toOutput, buf, len, amount);
				}

				if (amount <= 0)
				{
					break;
				}
			}
		}

		static void SendData(vector<unsigned char> p_dataToSend, boost::asio::ip::tcp::socket &socket)
		{
			//boost::exception ec;
			vector<unsigned char> amountToSend;

			int size = p_dataToSend.size();

			unsigned char* amountBytes = (unsigned char*)&size;
			//parse the amount of bytes sent on network
			for (int index = 0; index < 4; index++)
			{
				amountToSend.push_back(amountBytes[index]);
			}

			boost::asio::write(socket, boost::asio::buffer(amountToSend), boost::asio::transfer_all());
			boost::asio::write(socket, boost::asio::buffer(p_dataToSend), boost::asio::transfer_all());
		}

		static void SendData(std::string stringToUse, boost::asio::ip::tcp::socket &socket)
		{
			boost::asio::write(socket, boost::asio::buffer(stringToUse));
		}
	};
}
