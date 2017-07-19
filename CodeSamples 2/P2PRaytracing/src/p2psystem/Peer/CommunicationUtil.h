#pragma once

#include <string>
#include <hash_map>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <tuple>

#include <boost\tokenizer.hpp>
#include <boost\lexical_cast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

using std::hash_map;
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::stringstream;

typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
typedef boost::asio::ip::tcp::socket Socket;
typedef boost::asio::io_service IoSrevice;
typedef boost::system::error_code BoostSystemErrorCode;

namespace Network
{
	namespace Communication
	{
		class CommunicationUtil
		{
		public:
			static void SendData(std::string stringToUse, boost::asio::ip::tcp::socket &socket)
			{
				boost::asio::write(socket, boost::asio::buffer(stringToUse));
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

			static bool CheckContent(char* myArray, int length)
			{
				for (int index = 0; index < length; index++)
				{
					if (myArray[index] == '|')
						return true;
				}

				return false;
			}

			static string ReceiveData(boost::asio::ip::tcp::socket &socket)
			{
				stringstream stream;

				for (;;)
				{
					std::array <char, 2048> buf;
					boost::system::error_code error;

					size_t len = socket.read_some(boost::asio::buffer(buf), error);

					if (error == boost::asio::error::eof)
						break;
					else if (error)
						throw boost::system::system_error(error);

					if (CheckContent(buf.data(), len))
					{
						stream.write(buf.data(), len - 1);
						break;
					}
					else
						stream.write(buf.data(), len);

				}

				return stream.str();
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
				int test = 52;

				unsigned char * testArray = (unsigned char *) &test;

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
						throw boost::system::system_error(error);
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

		private:
		};
	}
}