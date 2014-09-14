
#include "replicaSetManager.h"

#include "log4cxx/logger.h"

#include "mongosDaemon.h"
#include "replicaSet.h"

#include <sstream>

// Using declarations - standard library
using std::set;
using std::shared_ptr;
using std::stringstream;

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nebu.mongo.MongocfgDaemon"));

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			ReplicaSetManager::ReplicaSetManager() :
					replicaSets(), nextReplicaSetNo(1)
			{

			}

			void ReplicaSetManager::addReplicaSet(shared_ptr<ReplicaSet> replicaSet)
			{
				stringstream replSetName;
				replSetName << "rs" << this->nextReplicaSetNo;

				replicaSet->setName(replSetName.str());

				this->replicaSets.insert(replicaSet);
				this->nextReplicaSetNo++;
			}

			set<shared_ptr<ReplicaSet>> ReplicaSetManager::getReplicaSets() const
			{
				return this->replicaSets;
			}

			void ReplicaSetManager::registerReplicaSetsWithRouter(shared_ptr<MongosDaemon> queryRouter)
			{
				for (set<shared_ptr<ReplicaSet>>::iterator it = this->replicaSets.begin();
					 it != this->replicaSets.end();
					 it++)
				{
					if (!(*it)->hasLaunched()) {
						(*it)->launch();
					}

					if ((*it)->hasLaunched() && !(*it)->hasRegistered() && this->registerReplicaSetWithRouter(*it, queryRouter)) {
						(*it)->setRegistered();
						LOG4CXX_DEBUG(logger, "Replicaset Registered: " << (*it)->getName());
					}
				}
			}

			bool ReplicaSetManager::registerReplicaSetWithRouter(shared_ptr<ReplicaSet> replicaSet,
					shared_ptr<MongosDaemon> queryRouter)
			{
				return queryRouter->registerReplicaSet(replicaSet);
			}

		}
	}
}
