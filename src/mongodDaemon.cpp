
#include "mongoAdapter.h"
#include "mongodDaemon.h"

#include "nebu-app-framework/commandRunner.h"

#include "log4cxx/logger.h"

#include "mongo/bson/bson.h"

#include <sstream>

// Using declarations - mongo-cxx-driver
using mongo::BSONArrayBuilder;
using mongo::BSONObj;
// Using declarations - standard library
using std::set;
using std::shared_ptr;
using std::string;
using std::stringstream;

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nebu.app.mongo.MongodDaemon"));

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			bool MongodDaemon::launch()
			{
				stringstream ss;
				ss << "scripts/launch-dataserver.sh" <<
						" '" << this->getHostname() << "'" <<
						" '" << this->getPortNo() << "'" <<
						" '" << this->getReplSet() << "'";

				int result = NEBU_RUNCOMMAND(ss.str());

				if (result == 0) {
					this->launched = true;
					LOG4CXX_DEBUG(logger, "Launch Successfull: " << this->getHostname());
				}
				return this->launched;
			}

			bool MongodDaemon::initiateReplicaSet(set<shared_ptr<MongodDaemon>> daemons)
			{
				BSONArrayBuilder memberBuilder;
				int index = 0;
				for (set<shared_ptr<MongodDaemon>>::iterator it = daemons.begin();
					 it != daemons.end();
					 it++, index++)
				{
					memberBuilder << BSON("_id" << index << "host" << (*it)->getHostPortPair());
				}

				BSONObj shardingCommand = BSON(
							"replSetInitiate" << BSON(
								"_id" << this->getReplSet() <<
								"members" << memberBuilder.arr()
							)
						);
				BSONObj result = MongoAdapter::getInstance()->executeCommand(this->getHostPortPair(), "admin", shardingCommand);
				return true;
			}

		}
	}
}

