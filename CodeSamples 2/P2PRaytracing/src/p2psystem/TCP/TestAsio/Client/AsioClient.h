#pragma once 

#include <iostream>
#include <exception>
#include <array>
#include <boost/asio.hpp>
#include <vector>


using std::vector;
namespace Network
{
	namespace Communication
	{
		///-------------------------------------------------------------------------------------------------
		/// <summary>	A test client. </summary>
		///
		/// <remarks>	Adrian, 22/10/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		const char* HELLO_PORT_STR = "50013";

		class TestClient
		{
		private:

			bool CheckContent(char* myArray, int length)
			{
				for (int index = 0; index < length; index++)
				{
					if (myArray[index] == '|')
						return true;
				}
				
				return false;
			}

			bool CheckContent(std::array<unsigned char, 1024>& p_buf, int length)
			{
				for (int index = 0; index < length; index++)
				{
					if (p_buf[index] == '|')
						return true;
				}

				return false;
			}

			bool AddDataToVector(vector<unsigned char>& p_outputArray, std::array<unsigned char, 1024>& p_buf )
			{
				for (int index = 0; index < p_buf.size(); index)
				{
					p_outputArray.push_back(p_buf[index]);
				}

				if (p_buf.size() < 1024)
				{
					return false;
				}
			}

			void ReceiveData(boost::asio::ip::tcp::socket &socket, vector<unsigned char>& p_outputArray)
			{

				for (;;)
				{
					std::array <unsigned char, 1024> buf;
					boost::system::error_code error;

					size_t len = socket.read_some(boost::asio::buffer(buf), error);

					if (error == boost::asio::error::eof)
						break;
					else if (error)
						throw boost::system::system_error(error);

					AddDataToVector(p_outputArray, buf);

					if (len < buf.size() || CheckContent(buf, len))
						break;
				}
			}

			void SendData(std::string stringToUse, boost::asio::ip::tcp::socket &socket)
			{
				boost::asio::write(socket, boost::asio::buffer(stringToUse));
			}

			void AddDataToArray()
			{

			}

			void  StartClient(const char* p_host)
			{
				try
				{
					vector<int> toChange;

					for (int index = 0; index < 2000;index++   )
					{
						toChange.push_back(index);
					}

					
					vector<unsigned char> testingVector;

					for (int index = 0; index < 2000; index++)
					{
						unsigned char* testerBytes = (unsigned char*)&toChange[index];
						for (int index = 0; index < 4; index++)
						{
							testingVector.push_back(testerBytes[index]);
						}
					}


					boost::asio::io_service aios;

					boost::asio::ip::tcp::resolver resolver(aios);
					boost::asio::ip::tcp::resolver::iterator endpoint = resolver.resolve(boost::asio::ip::tcp::resolver::query(p_host, HELLO_PORT_STR));
					
					boost::asio::ip::tcp::socket sockett(aios);
					boost::asio::connect(sockett, endpoint);
					
					boost::asio::write(sockett, boost::asio::buffer(testingVector));

					//string receivedString = ReceiveData(sockett);
					
				}
				catch (std::exception e)
				{
					std::cout << "Error occured " << e.what() <<std::endl;
				}
			}

		public:
			TestClient()
			{
				StartClient("127.0.0.1");
			}
		};
	}
}
