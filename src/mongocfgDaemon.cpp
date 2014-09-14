
#include "mongocfgDaemon.h"

#include "nebu-app-framework/commandRunner.h"

#include "log4cxx/logger.h"

#include <sstream>

// Using declarations - standard library
using std::stringstream;

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nebu.app.mongo.MongocfgDaemon"));

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			bool MongocfgDaemon::launch()
			{
				stringstream ss;
				ss << "scripts/launch-configsvr.sh" <<
						" '" << this->getHostname() << "'" <<
						" '" << this->getPortNo() << "'";

				int result = NEBU_RUNCOMMAND(ss.str());

				if (result == 0) {
						this->launched = true;
						LOG4CXX_DEBUG(logger, "Launch Successfull: " << this->getHostname());
				}
				return this->hasLaunched();
			}

		}
	}
}

