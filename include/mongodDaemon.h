
#ifndef NEBUAPPMONGO_MONGODDAEMON_H_
#define NEBUAPPMONGO_MONGODDAEMON_H_

#include "mongoDaemon.h"

#include <set>

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			class MongodDaemon : public MongoDaemon
			{
			public:
				MongodDaemon(std::shared_ptr<nebu::common::VirtualMachine> hostVM, int portNo) :
					MongoDaemon(hostVM, portNo), replSet("") { }
				virtual ~MongodDaemon() { }

				virtual bool launch();

				virtual framework::DaemonType getType() const
				{
					return MongoDaemonType::DATA_SERVER;
				}

				virtual const std::string &getReplSet() const { return this->replSet; }
				virtual void setReplSet(std::string replSet) { this->replSet = replSet; }

				virtual bool initiateReplicaSet(std::set<std::shared_ptr<MongodDaemon>> daemons);

			protected:
				std::string replSet;
			};

		}
	}
}

#endif

