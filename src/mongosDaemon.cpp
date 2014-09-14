
#include "mongoAdapter.h"
#include "mongocfgDaemon.h"
#include "mongodDaemon.h"
#include "mongosDaemon.h"
#include "replicaSet.h"

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

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nebu.mongo.MongocfgDaemon"));

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			bool MongosDaemon::launch()
			{
				stringstream ss;
				ss << "scripts/launch-router.sh" <<
						" '" << this->getHostname() << "'" <<
						" '" << this->getPortNo() << "'" <<
						" '" << this->getConfigServers() << "'";

				int result = NEBU_RUNCOMMAND(ss.str());

				if (result == 0) {
					this->launched = true;
						LOG4CXX_DEBUG(logger, "Launch Successfull: " << this->getHostname());
				}
				return this->launched;
			}

			void MongosDaemon::setConfigServers(set<shared_ptr<MongocfgDaemon>> cfgServers)
			{
				stringstream configServers;
				for (set<shared_ptr<MongocfgDaemon>>::iterator cfg = cfgServers.begin();
					 cfg != cfgServers.end();
					 cfg++)
				{
					configServers << (*cfg)->getHostPortPair() << ",";
				}

				string configServerList = configServers.str();
				configServerList.erase(configServerList.end() - 1);

				this->setConfigServers(configServerList);
			}

			bool MongosDaemon::registerReplicaSet(shared_ptr<ReplicaSet> replicaSet)
			{
				stringstream shardID;
				shardID << replicaSet->getName() << "/" << (*replicaSet->getDataServers().begin())->getHostPortPair();

				BSONObj shardingCommand = BSON("addShard" << shardID.str());
				BSONObj result = MongoAdapter::getInstance()->executeCommand(this->getHostPortPair(), "admin", shardingCommand);

				return (result["ok"].Number() > 0);
			}

		}
	}
}

