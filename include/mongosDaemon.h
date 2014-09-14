
#ifndef NEBUAPPMONGO_MONGOSDAEMON_H_
#define NEBUAPPMONGO_MONGOSDAEMON_H_

#include "mongoDaemon.h"

#include <set>

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			class MongocfgDaemon;
			class ReplicaSet;

			class MongosDaemon : public MongoDaemon
			{
			public:
				MongosDaemon(std::shared_ptr<nebu::common::VirtualMachine> hostVM, int portNo) :
					MongoDaemon(hostVM, portNo), cfgList("") { }
				virtual ~MongosDaemon() { }

				virtual bool launch();

				virtual framework::DaemonType getType() const
				{
					return MongoDaemonType::QUERY_ROUTER;
				}

				virtual const std::string &getConfigServers() const { return this->cfgList; }
				virtual void setConfigServers(std::string serverList) { this->cfgList = serverList; }
				virtual void setConfigServers(std::set<std::shared_ptr<MongocfgDaemon>> cfgServers);

				virtual bool registerReplicaSet(std::shared_ptr<ReplicaSet> replicaSet);

			protected:
				std::string cfgList;
			};

		}
	}
}

#endif
