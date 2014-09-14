
#ifndef NEBUAPPMONGO_MONGOAPPLICATION_H_
#define NEBUAPPMONGO_MONGOAPPLICATION_H_

#include "nebu-app-framework/applicationHooks.h"
#include "nebu-app-framework/topologyWriter.h"

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			class DaemonManager;
			class ReplicaSetManager;

			class MongoApplication : public framework::ApplicationHooks
			{
			public:
				MongoApplication() : framework::ApplicationHooks() { }
				virtual ~MongoApplication() { }

				virtual void registerConfigurationOptions();

				virtual std::shared_ptr<framework::DaemonManager> getDaemonManager();
				virtual std::shared_ptr<ReplicaSetManager> getReplicaSetManager();

			protected:
				std::shared_ptr<DaemonManager> daemonManager;
				std::shared_ptr<ReplicaSetManager> replicaSetManager;
			};

		}
	}
}

#endif
