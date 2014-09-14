
#include "mongoDaemon.h"

#include <sstream>

// Using declarations - standard library
using std::string;
using std::stringstream;

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			string MongoDaemon::getHostPortPair()
			{
				stringstream ss;
				ss << this->getHostname() << ":" << this->getPortNo();
				return ss.str();
			}

		}
	}
}
