#pragma once 

#include <vector>
#include <string>


using std::vector;
using std::string;
typedef vector<unsigned char>::iterator Iter;
namespace Engine
{
	namespace Communication
	{
		namespace Serialization
		{
			union byteInt
			{
				int i;
				unsigned char b[sizeof(int)];
			};

			union byteFloat
			{
				float f;
				unsigned char b[sizeof(float)];
			};

			template<typename T>
			union Converter
			{
				T toConvert;
				unsigned char b[sizeof(T)];
			};

			class ConvertionClass
			{
			public:

				static float ByteArrayToFloat(vector<unsigned char> p_array) //TODO: V813 http://www.viva64.com/en/V813 Decreased performance. The 'p_array' argument should probably be rendered as a constant reference.
				{
					byteFloat temp;

					for (int index = 0; index < 4; index++)
					{
						temp.b[index] = p_array[index];
					}

					return temp.f;
				}

				static float ByteArrayToFloat(unsigned char p_array[])
				{
					byteFloat temp;

					for (int index = 0; index < 4; index++)
					{
						temp.b[index] = p_array[index];
					}

					return temp.f;
				}

				static void ByteArrayToInt(int& p_output, vector<unsigned char>& p_array, int &p_startingPoint, int p_length)
				{
					byteInt temp;
					int inc = p_startingPoint;

					for (int index = 0; index < p_length; index++, p_startingPoint++)
					{
						temp.b[index] = p_array[p_startingPoint];
					}
					p_output = temp.i;

				}

				static void ByteArrayToFloat(float& p_output, vector<unsigned char>& p_array, int &p_startingPoint, int p_length)
				{
					byteFloat temp;

					for (int index = 0; index < p_length; index++, p_startingPoint++)
					{
						temp.b[index] = p_array[p_startingPoint];
					}

					p_output = temp.f;
				}

				/*static void StreamToFloat(float& p_output, std::ifstream& p_stream, int p_length)
				{
					char* buff = new char[4];

					p_stream.get(buff, 4);

					byteFloat temp;

					for (int index = 0; index < p_length; index++)
					{
						temp.b[index] = (unsigned char)buff[index];
					}

					p_output = temp.f;
				}*/



				static void ConvertInt(int p_toConvert, vector<unsigned char>& p_byteArray)
				{
					unsigned char* temp = (unsigned char*)&p_toConvert;

					for (int index = 0; index < 4; index++)
					{
						p_byteArray.push_back(temp[index]);
					}
				}

				static void ConvertFloat(float p_toConvert, unsigned char p_byteArray[4])
				{
					unsigned char* temp = (unsigned char*)&p_toConvert;

					for (int index = 0; index < 4; index++)
					{
						p_byteArray[index] = temp[index];
					}
				}

				static void ConvertFloat(float p_toConvert, int p_startingPosition, unsigned char* p_byteArray)
				{
					int inc = p_startingPosition;

					unsigned char* temp = (unsigned char*)&p_toConvert;

					for (int index = 0; index < 4; index++)
					{
						p_byteArray[inc] = temp[index];
						inc++;
					}
				}

				static void ConvertFloat(float p_toConvert, vector<unsigned char>& p_byteArray)
				{
					byteFloat temp;
					temp.f = p_toConvert;

					float tempNum = p_toConvert;

					unsigned char* charArr = (unsigned char*)&tempNum;

					for (int index = 0; index < 4; index++)
					{
						if (temp.b[index] != charArr[index])
						{
							cout << "priblem here" << endl;
						}

						p_byteArray.push_back(temp.b[index]);
					}
				}


				static void ConvertInt(int p_toConvert, int p_startingPosition, unsigned char p_byteArray[])
				{
					int inc = p_startingPosition;

					byteInt temp;
					temp.i = p_toConvert;

					for (int index = 0; index < 4; index++)
					{
						p_byteArray[inc] = temp.b[index];
						inc++;
					}
				}

				static void ConvertInt(int p_toConvert, int &p_startingPosition, vector<unsigned char>& p_byteArray)
				{

					byteInt temp;
					temp.i = p_toConvert;

					for (int index = 0; index < 4; index++, p_startingPosition)
					{
						p_byteArray[p_startingPosition] = temp.b[index];
					}
				}

				static void GetChunk(int p_startingPosition, int p_endingPosition, vector<unsigned char>& p_toTransfer, unsigned char* p_toPush)
				{
					int inc = 0;
					for (int index = p_startingPosition; index < p_endingPosition; index++)
					{
						p_toPush[inc] = p_toTransfer[index];
						inc++;
					}
				}

				static vector<unsigned char> GetSubset(int p_startingPosition, int p_amount, vector<unsigned char>& p_toTransfer)
				{
					Iter iterator = p_toTransfer.begin() + p_startingPosition;
					vector<unsigned char> toReturn(iterator, iterator + p_amount);
					return toReturn;
				}

				/*static unsigned char* GetSubset(int p_startingPosition, int p_amount, vector<unsigned char>& p_toTransfer)
				{
					unsigned char* output = new unsigned char[p_amount];

					int inc = p_startingAmount;

					for (int index = 0; index < p_amount; index++)
					{
						output[index] = p_toTransfer[inc];
						inc++;
					}

					return output;
				}
				*/

				static void AddArrayToVector(unsigned char p_array[], int p_size, vector<unsigned char> p_byteArray)
				{
					for (int index = 0; index < p_size; index++)
					{
						p_byteArray.push_back(p_array[index]);
					}
				}
			};
		}
	}
}