
#ifndef NEBUAPPMONGO_DAEMONDEPLOYER_H_
#define NEBUAPPMONGO_DAEMONDEPLOYER_H_

#include "nebu-app-framework/daemonCollection.h"
#include "nebu-app-framework/topologyManager.h"
#include "nebu/virtualMachine.h"

#include <map>
#include <memory>
#include <set>
#include <vector>

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			class MongodDaemon;
			class ReplicaSetManager;
			class ReplicaSet;

			class DaemonDeployer
			{
			public:
				DaemonDeployer(std::shared_ptr<framework::DaemonCollection> daemonCollection,
						std::shared_ptr<ReplicaSetManager> replicaSetManager,
						std::shared_ptr<framework::TopologyManager> topologyManager);
				virtual ~DaemonDeployer() { };

				void deployDaemons();

				bool configServersActive();
				bool queryRouterActive();
			private:
				std::shared_ptr<framework::DaemonCollection> daemonCollection;
				std::shared_ptr<ReplicaSetManager> replicaSetManager;
				std::shared_ptr<framework::TopologyManager> topologyManager;

				void deployConfigServers();
				void deployQueryRouters();
				void deployDataServers();

				bool isAcceptableHostLocality(const std::set<std::shared_ptr<MongodDaemon>> &daemonList);
				bool isAcceptableRackLocality(const std::set<std::shared_ptr<MongodDaemon>> &daemonList);
				bool isAcceptableDataCenterLocality(const std::set<std::shared_ptr<MongodDaemon>> &daemonList);
				bool isAcceptableReplicaSet(std::set<std::shared_ptr<MongodDaemon>> daemonList);
				std::shared_ptr<ReplicaSet> selectReplicaSet(std::set<std::shared_ptr<MongodDaemon>> daemonList,
						unsigned int replicationFactor);

				struct Distance;
				struct DistanceParams;
				Distance distanceFunction(const DistanceParams &params, std::set<std::shared_ptr<MongodDaemon>> daemons);
			};

		}
	}
}

#endif
