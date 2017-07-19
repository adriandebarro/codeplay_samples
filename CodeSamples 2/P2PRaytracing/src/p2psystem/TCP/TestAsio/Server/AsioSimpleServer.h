#pragma once 

#include <iostream>
#include <exception>
#include <array>
#include <boost/asio.hpp>


using namespace std;
namespace Network
{
	namespace Communication
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	A simple test server. </summary>
		///
		/// <remarks>	Adrian, 07/07/2017. </remarks>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		class SimpleTestServer
		{
		private:
			/*void SendData(std::string stringToUse, boost::asio::ip::tcp::socket &socket)
			{
				boost::asio::write(socket, boost::asio::buffer(stringToUse));
			}*/

			union byteInt
			{
				int i;
				unsigned char b[sizeof(int)];
			};


			bool AddDataToVector(vector<unsigned char>& p_outputArray, std::array<unsigned char, 1024>& p_buf)
			{
				for (int index = 0; index < p_buf.size(); index++)
				{
					p_outputArray.push_back(p_buf[index]);
				}

				if (p_buf.size() < 1024)
				{
					return false;
				}
			}

			void StartServer()
			{
				try
				{
					boost::asio::io_service aios;
					
					boost::asio::ip::tcp::acceptor acceptor(aios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 50013));
					{
						boost::asio::ip::tcp::socket socket(aios);
						std::cout << "Server Ready" << std::endl;

						vector<unsigned char> toChange;

						acceptor.accept(socket);

						byteInt test;

						for (;;)
						{
							std::array <unsigned char, 1024> buf;
							boost::system::error_code error;

							size_t len = socket.read_some(boost::asio::buffer(buf), error);

							AddDataToVector(toChange, buf);

							if (error == boost::asio::error::eof)
								break;
							else if (error)
								throw boost::system::system_error(error);

							
						}

						for (int index = 0; index < 8000; index+=4)
						{
							byteInt current;

							for (int subIndex = 0; subIndex < 4; subIndex++)
							{
								current.b[subIndex] = toChange[index + subIndex];
							}

							cout << current.i << endl;
						}


						/*std::string message("HellofromAsio |");

						boost::asio::write(socket, boost::asio::buffer(message));
						*/
						cout << socket.remote_endpoint().address().to_string() << endl;

					

					}

					system("pause");
				}
				catch (std::exception e )
				{}
			}

		public:
			SimpleTestServer()
			{
				StartServer();
			}
		};
	}
}