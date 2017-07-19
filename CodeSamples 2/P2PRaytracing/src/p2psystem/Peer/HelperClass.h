#ifndef __HELPERCLASS__
#define __HELPERCLASS__

#include <vector>
#include <boost\tokenizer.hpp>
#include <math.h>


typedef boost::tokenizer<boost::char_separator<char>> tokeniz;

using std::vector;

class HelperClass
{
public:
	static void ParseStringSentence(std::string toParse, std::string splittingString, std::string output[])
	{
		boost::char_separator<char> sep(splittingString.c_str());
		tokeniz tok(toParse, sep);
		
		int index = 0;
		for (tokeniz::iterator tokIter = tok.begin(); tokIter != tok.end(); ++ tokIter)
		{
			output[index] = tokIter.current_token().c_str();
			++index;
		}
	}

	static void ParseStringSentence(std::string toParse, std::string splittingString, vector<std::string>& output)
	{
		boost::char_separator<char> sep(splittingString.c_str());
		tokeniz tok(toParse, sep);

		int index = 0;
		for (tokeniz::iterator tokIter = tok.begin(); tokIter != tok.end(); ++tokIter)
		{
			output.push_back(tokIter.current_token().c_str());
		}
	}

	static double logx(double x, double a)
	{
		return log(x) / log(a);
	}

	static int SetAmounTimeSleep(const int& numberPeers)
	{
		return (int)(logx(numberPeers,2) + logx(numberPeers, 2.718281828459045235));
	}
};

#endif // !__HELPERCLASS__
