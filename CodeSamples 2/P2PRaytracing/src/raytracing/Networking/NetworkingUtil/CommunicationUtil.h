#pragma once 

#include <vector>
#include <string>
#include <math.h>

#include <boost/uuid/uuid.hpp>

using std::vector;
using std::string;
using boost::uuids::uuid;

namespace Engine
{
	namespace Networking
	{
		namespace Communication
		{
			namespace Util
			{
				class UtilClass
				{
				private:
				
				public:

					static string GetString(vector<unsigned char> &p_byteArray, int &p_currentIndex)
					{
						int stringSize = GetNum<int>(p_byteArray, p_currentIndex);
						
						string tempString = "";
						unsigned char * stringByteArray = (unsigned char *)&tempString;

						for (int index = 0; index < stringSize; index++, p_currentIndex++)
						{
							stringByteArray[index] = p_byteArray[p_currentIndex];
						}
						return tempString;
					}

					static string GetIpAddress(vector<unsigned char> &p_byteArray, int &p_currentIndex)
					{
						long ipRepresentation = GetLong(p_byteArray, p_currentIndex);
						string ip = LongToIP(ipRepresentation);
						return ip;
					}

					static string LongToIP(unsigned long longIP)
					{
						struct in_addr addr;
						addr.S_un.S_addr = longIP;
						char* dotIp = inet_ntoa(addr);

						return std::string(dotIp);
	
					}

					template<typename T>
					static T GetNum(vector<unsigned char> &p_byteArray, int &p_currentIndex)
					{
						T tempInt = 0;
						unsigned char * tempArray = (unsigned char *)&tempInt;
						for (int index = 0; index < 4; index++, p_currentIndex++)
						{
							tempArray[index] = p_byteArray[p_currentIndex];
						}
						return tempInt;
					}

					template<typename T>
					static void GetNumArray(vector<unsigned char> &p_byteArray, int &p_currentIndex, int p_arraySize, T* p_containerPointer)
					{
						for (int index = 0; index < p_arraySize; index++)
						{
							p_containerPointer[index] = GetNum<T>(p_byteArray, p_currentIndex);
						}
					}

					static unsigned long GetLong(vector<unsigned char> &p_byteArray, int &p_currentIndex)
					{
						long representation = 0;
						int sizeOFRepresentation = sizeof(long);

						unsigned char* temp = (unsigned char *)&representation;

						int sizeOfTemp = sizeof(temp) / sizeof(unsigned char);

						for (int index = 0; index < 4; index++, p_currentIndex++)
						{
							temp[index] = p_byteArray[p_currentIndex];
						}

						return representation;
					}


					static uuid GetUuid(vector<unsigned char*>& p_byteArray, int& p_currentPosition)
					{
						uuid output = *(uuid*)&p_byteArray[p_currentPosition];
						p_currentPosition += sizeof(uuid);
						return output;
					}

					void static IntToByteArray(vector<unsigned char>& p_vector, int toConvert)
					{
						unsigned char* tempPtr = (unsigned char*)&toConvert;
						p_vector.push_back(tempPtr[0]);
						p_vector.push_back(tempPtr[1]);
						p_vector.push_back(tempPtr[2]);
						p_vector.push_back(tempPtr[3]);
					}
					
				};
			}
		}
	}
}
