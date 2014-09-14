
#include "mongodDaemon.h"
#include "replicaSet.h"

#include "log4cxx/logger.h"

// Using declarations - standard library
using std::set;
using std::shared_ptr;
using std::string;

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nebu.mongo.ReplicaSet"));

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			ReplicaSet::ReplicaSet() :
					dataServers(), name(""), registered(false), launched(false)
			{

			}

			void ReplicaSet::addDataServer(shared_ptr<MongodDaemon> dataServer)
			{
				this->dataServers.insert(dataServer);
				dataServer->setReplSet(this->getName());
			}

			void ReplicaSet::addDataServers(set<shared_ptr<MongodDaemon>> dataServers)
			{
				for (set<shared_ptr<MongodDaemon>>::iterator it = dataServers.begin();
					 it != dataServers.end();
					 it++)
				{
					this->addDataServer(*it);
				}
			}

			set<shared_ptr<MongodDaemon>> ReplicaSet::getDataServers() const
			{
				return this->dataServers;
			}

			string ReplicaSet::getName() const
			{
				return this->name;
			}

			void ReplicaSet::setName(const string &name)
			{
				this->name = name;
				for (set<shared_ptr<MongodDaemon>>::iterator it = this->dataServers.begin();
					 it != this->dataServers.end();
					 it++)
				{
					(*it)->setReplSet(name);
				}
			}

			bool ReplicaSet::launch()
			{
				if (this->hasLaunched()) {
					return true;
				}

				for (set<shared_ptr<MongodDaemon>>::iterator it = this->dataServers.begin();
					 it != this->dataServers.end();
					 it++)
				{
					if (!(*it)->hasLaunched() && !(*it)->launch()) {
						LOG4CXX_DEBUG(logger, "Failed to launch data server " << (*it)->getHostname() <<
								" as part of replica set " << this->getName());
						return false;
					}
				}

				this->launched = (*this->dataServers.begin())->initiateReplicaSet(this->dataServers);
				return this->launched;
			}

			bool ReplicaSet::hasLaunched() const
			{
				return this->launched;
			}

			bool ReplicaSet::hasRegistered() const
			{
				return this->registered;
			}

			void ReplicaSet::setRegistered()
			{
				this->registered = true;
			}

		}
	}
}
