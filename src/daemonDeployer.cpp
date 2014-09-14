
#include "daemonDeployer.h"
#include "mongoConfiguration.h"
#include "mongocfgDaemon.h"
#include "mongodDaemon.h"
#include "mongosDaemon.h"
#include "replicaSet.h"
#include "replicaSetManager.h"

#include "nebu/topology/physicalRack.h"

#include "log4cxx/logger.h"

#include <algorithm>
#include <list>
#include <map>
#include <sstream>
#include <vector>

// Using declarations - standard library
using std::list;
using std::make_shared;
using std::map;
using std::pair;
using std::set;
using std::shared_ptr;
using std::static_pointer_cast;
using std::string;
using std::stringstream;
using std::vector;
// Using declarations - nebu-common
using nebu::common::PhysicalRack;
using nebu::common::VirtualMachine;
// Using declarations - nebu-app-framework
using nebu::app::framework::Daemon;
using nebu::app::framework::DaemonType;

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nebu.app.mongo.DaemonDeployer"));

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			DaemonDeployer::DaemonDeployer(shared_ptr<framework::DaemonCollection> daemonCollection,
								shared_ptr<ReplicaSetManager> replicaSetManager,
								shared_ptr<framework::TopologyManager> topologyManager) :
					daemonCollection(daemonCollection), replicaSetManager(replicaSetManager),
					topologyManager(topologyManager)
			{

			}

			template<typename In, typename Out>
			set<shared_ptr<Out>> castSetToType(const set<shared_ptr<In>> &daemonSet)
			{
				set<shared_ptr<Out>> dataServers;
				for (typename set<shared_ptr<In>>::const_iterator daemon = daemonSet.begin();
						daemon != daemonSet.end();
						daemon++)
				{
					dataServers.insert(static_pointer_cast<Out>(*daemon));
				}
				return dataServers;
			}

			void DaemonDeployer::deployDaemons()
			{
				if (!this->configServersActive()) {
					this->deployConfigServers();
				}

				if (this->configServersActive()) {
					this->deployQueryRouters();
					this->deployDataServers();

					if (this->queryRouterActive()) {
						shared_ptr<MongosDaemon> queryRouter = static_pointer_cast<MongosDaemon>(
								*this->daemonCollection->getLaunchedDaemonsForType(MongoDaemonType::QUERY_ROUTER).begin());
						this->replicaSetManager->registerReplicaSetsWithRouter(queryRouter);
					}
				}
			}

			void DaemonDeployer::deployConfigServers()
			{
				// TODO: Hold out on launching config servers if they are not distributed well enough

				set<shared_ptr<Daemon>> unlaunchedConfigServers =
						this->daemonCollection->getUnlaunchedDaemonsForType(MongoDaemonType::CONFIG_SERVER);

				for (set<shared_ptr<Daemon>>::iterator unlaunchedConfigServer = unlaunchedConfigServers.begin();
					 unlaunchedConfigServer != unlaunchedConfigServers.end() && !this->configServersActive();
					 unlaunchedConfigServer++)
				{
					(*unlaunchedConfigServer)->launch();
				}
			}

			void DaemonDeployer::deployQueryRouters()
			{
				set<shared_ptr<Daemon>> unlaunchedQueryRouters =
						this->daemonCollection->getUnlaunchedDaemonsForType(MongoDaemonType::QUERY_ROUTER);

				for (set<shared_ptr<Daemon>>::iterator unlaunchedQueryRouter = unlaunchedQueryRouters.begin();
					 unlaunchedQueryRouter != unlaunchedQueryRouters.end();
					 unlaunchedQueryRouter++)
				{
					shared_ptr<MongosDaemon> queryRouter = static_pointer_cast<MongosDaemon>(*unlaunchedQueryRouter);
					queryRouter->setConfigServers(castSetToType<Daemon, MongocfgDaemon>(
							this->daemonCollection->getLaunchedDaemonsForType(MongoDaemonType::CONFIG_SERVER)));
					queryRouter->launch();
				}
			}

			struct DaemonDeployer::Distance
			{
			public:
				unsigned int uniqueHostsStores;
				unsigned int rackOverload;
				unsigned int dataCenterOverload;
				unsigned int uniqueRacks;
				unsigned int uniqueDataCenters;
				unsigned int busyHostsStores;

				Distance() : uniqueHostsStores(0), rackOverload(0), dataCenterOverload(0),
						uniqueRacks(0), uniqueDataCenters(0), busyHostsStores(0)
				{

				}

				bool operator<(const Distance &other) const
				{
					return (Distance::compare(*this, other) < 0);
				}
				bool operator<=(const Distance &other) const
				{
					return (Distance::compare(*this, other) <= 0);
				}
				bool operator>(const Distance &other) const
				{
					return (Distance::compare(*this, other) > 0);
				}
				bool operator>=(const Distance &other) const
				{
					return (Distance::compare(*this, other) >= 0);
				}
				bool operator==(const Distance &other) const
				{
					return (Distance::compare(*this, other) == 0);
				}
				bool operator!=(const Distance &other) const
				{
					return (Distance::compare(*this, other) != 0);
				}

			private:
				static int compare(const Distance &A, const Distance &B)
				{
					if (A.uniqueHostsStores < B.uniqueHostsStores) {
						return -1;
					} else if (A.uniqueHostsStores > B.uniqueHostsStores) {
						return 1;
					}

					if (A.rackOverload < B.rackOverload) {
						return -1;
					} else if (A.rackOverload > B.rackOverload) {
						return 1;
					}

					if (A.dataCenterOverload < B.dataCenterOverload) {
						return -1;
					} else if (A.dataCenterOverload > B.dataCenterOverload) {
						return 1;
					}

					if (A.uniqueRacks < B.uniqueRacks) {
						return -1;
					} else if (A.uniqueRacks > B.uniqueRacks) {
						return 1;
					}

					if (A.uniqueDataCenters < B.uniqueDataCenters) {
						return -1;
					} else if (A.uniqueDataCenters > B.uniqueDataCenters) {
						return 1;
					}

					if (A.busyHostsStores < B.busyHostsStores) {
						return -1;
					} else if (A.busyHostsStores > B.busyHostsStores) {
						return 1;
					}

					return 0;
				}
			};

			struct DaemonDeployer::DistanceParams
			{
			public:
				set<shared_ptr<MongodDaemon>> daemons;
				unsigned int daemonCount;
				unsigned int replicationFactor;
			};

			DaemonDeployer::Distance DaemonDeployer::distanceFunction(
					const DaemonDeployer::DistanceParams &params, set<shared_ptr<MongodDaemon>> daemons)
			{
				DaemonDeployer::Distance distance;

				for (set<shared_ptr<MongodDaemon>>::iterator daemonA = daemons.begin();
					 daemonA != daemons.end();
					 daemonA++)
				{
					set<shared_ptr<MongodDaemon>>::iterator daemonB = daemonA;
					for (daemonB++;
						 daemonB != daemons.end();
						 daemonB++)
					{
						// Count unique hosts and stores chosen
						string hostA = (*daemonA)->getHostVM()->getPhysicalHostID();
						string hostB = (*daemonB)->getHostVM()->getPhysicalHostID();
						string storeA = (*daemonA)->getHostVM()->getPhysicalStoreID();
						string storeB = (*daemonB)->getHostVM()->getPhysicalStoreID();
						if (hostA != hostB && storeA != storeB) {
							distance.uniqueHostsStores++;
						}

						// Count unique racks chosen
						string rackA = this->topologyManager->getRackIDForHost(
								(*daemonA)->getHostVM()->getPhysicalHostID());
						string rackB = this->topologyManager->getRackIDForHost(
								(*daemonB)->getHostVM()->getPhysicalHostID());
						if (rackA != rackB) {
							distance.uniqueRacks++;
						}

						// Count unique data centers chosen
						string dataCenterA = this->topologyManager->getDataCenterIDForHost(
								(*daemonA)->getHostVM()->getPhysicalHostID());
						string dataCenterB = this->topologyManager->getDataCenterIDForHost(
								(*daemonB)->getHostVM()->getPhysicalHostID());
						if (dataCenterA != dataCenterB) {
							distance.uniqueDataCenters++;
						}
					}
				}

				map<string, unsigned int> rackLoad;
				map<string, unsigned int> dataCenterLoad;
				map<string, unsigned int> hostLoad;
				map<string, unsigned int> storeLoad;
				for (set<shared_ptr<MongodDaemon>>::iterator daemon = params.daemons.begin();
					 daemon != params.daemons.end();
					 daemon++)
				{
					// Determine rack overload
					string rack = this->topologyManager->getRackIDForHost(
							(*daemon)->getHostVM()->getPhysicalHostID());
					if (rackLoad.find(rack) == rackLoad.end()) {
						rackLoad[rack] = 0;
					}
					rackLoad[rack]++;

					// Determine data center overload
					string dataCenter = this->topologyManager->getDataCenterIDForHost(
							(*daemon)->getHostVM()->getPhysicalHostID());
					if (dataCenterLoad.find(dataCenter) == dataCenterLoad.end()) {
						dataCenterLoad[dataCenter] = 0;
					}
					dataCenterLoad[dataCenter]++;

					// Determine host overload
					string host = (*daemon)->getHostVM()->getPhysicalHostID();
					if (hostLoad.find(host) == hostLoad.end()) {
						hostLoad[host] = 0;
					}
					hostLoad[host]++;

					// Determine store overload
					string store = (*daemon)->getHostVM()->getPhysicalStoreID();
					if (storeLoad.find(store) == storeLoad.end()) {
						storeLoad[store] = 0;
					}
					storeLoad[store]++;
				}

				unsigned int loadLimit = (params.daemonCount + params.replicationFactor - 1) /
						params.replicationFactor;
				for (set<shared_ptr<MongodDaemon>>::iterator daemon = daemons.begin();
					 daemon != daemons.end();
					 daemon++)
				{
					// Determine pressure relief per rack
					string rack = this->topologyManager->getRackIDForHost(
							(*daemon)->getHostVM()->getPhysicalHostID());
					if (rackLoad[rack] >= loadLimit) {
						rackLoad[rack] -= loadLimit;
						distance.rackOverload++;
					}

					// Determine pressure relief per data center
					string dataCenter = this->topologyManager->getDataCenterIDForHost(
							(*daemon)->getHostVM()->getPhysicalHostID());
					if (dataCenterLoad[dataCenter] >= loadLimit) {
						dataCenterLoad[dataCenter] -= loadLimit;
						distance.dataCenterOverload++;
					}

					// Determine pressure relief per host
					string host = (*daemon)->getHostVM()->getPhysicalHostID();
					if (hostLoad[host] >= loadLimit) {
						hostLoad[host] -= loadLimit;
						distance.busyHostsStores++;
					}

					// Determine pressure relief per store
					string store = (*daemon)->getHostVM()->getPhysicalStoreID();
					if (storeLoad[store] >= loadLimit) {
						storeLoad[store] -= loadLimit;
						distance.busyHostsStores++;
					}
				}

				return distance;
			}

			void firstIndexVector(vector<unsigned int> &indices)
			{
				for (unsigned int i = 0; i < indices.size(); i++) {
					indices[i] = i;
				}
			}

			bool nextIndexVector(vector<unsigned int> &indices, unsigned int maxIndex)
			{
				for (int i = indices.size() - 1; i >= 0; i--) {
					unsigned int nextIndex = indices[i] + 1;
					unsigned int nextHighestIndex = nextIndex + (indices.size() - 1 - i);
					if (nextHighestIndex <= maxIndex) {
						for (unsigned int j = i; j < indices.size(); j++) {
							indices[j] = nextIndex + (j - i);
						}
						return true;
					}
				}
				return false;
			}

			bool DaemonDeployer::isAcceptableHostLocality(const set<shared_ptr<MongodDaemon>> &daemonList)
			{
				for (set<shared_ptr<MongodDaemon>>::const_iterator daemonA = daemonList.begin();
					 daemonA != daemonList.end();
					 daemonA++)
				{
					set<shared_ptr<MongodDaemon>>::const_iterator daemonB = daemonA; daemonB++;
					for ( ;
						 daemonB != daemonList.end();
						 daemonB++)
					{
						string hostA = (*daemonA)->getHostVM()->getPhysicalHostID();
						string hostB = (*daemonB)->getHostVM()->getPhysicalHostID();
						string storeA = (*daemonA)->getHostVM()->getPhysicalStoreID();
						string storeB = (*daemonB)->getHostVM()->getPhysicalStoreID();
						if (hostA == hostB || storeA == storeB) {
							return false;
						}
					}
				}
				return true;
			}

			bool DaemonDeployer::isAcceptableRackLocality(const set<shared_ptr<MongodDaemon>> &daemonList)
			{
				set<string> rackList;
				for (set<shared_ptr<MongodDaemon>>::const_iterator daemon = daemonList.begin();
					 daemon != daemonList.end();
					 daemon++)
				{
					string rackID = this->topologyManager->getRackIDForHost((*daemon)->getHostVM()->getPhysicalHostID());
					rackList.insert(rackID);
				}

				stringstream minRacksStr(CONFIG_GET(CONFIG_APP_POLICY_REPLICATED_MINRACKS));
				unsigned int minRacks;
				minRacksStr >> minRacks;

				if (minRacks <= rackList.size()) {
					return true;
				}

				stringstream rackWaitTimeoutStr(CONFIG_GET(CONFIG_APP_POLICY_REPLICATED_TIMEOUTRACKS));
				int rackWaitTimeout;
				rackWaitTimeoutStr >> rackWaitTimeout;

				if (rackWaitTimeout < 0) {
					return false;
				}

				int age = INT32_MAX;
				for (set<shared_ptr<MongodDaemon>>::const_iterator daemon = daemonList.begin();
					 daemon != daemonList.end();
					 daemon++)
				{
					int daemonAge = (*daemon)->secondsSinceDiscovery();
					if (daemonAge < age) {
						age = daemonAge;
					}
				}
				return age > rackWaitTimeout;
			}

			bool DaemonDeployer::isAcceptableDataCenterLocality(const set<shared_ptr<MongodDaemon>> &daemonList __attribute__((unused)))
			{
				return true;
			}

			bool DaemonDeployer::isAcceptableReplicaSet(set<shared_ptr<MongodDaemon>> daemonList)
			{
				return (this->isAcceptableHostLocality(daemonList) &&
						this->isAcceptableRackLocality(daemonList) &&
						this->isAcceptableDataCenterLocality(daemonList));
			}

			shared_ptr<ReplicaSet> DaemonDeployer::selectReplicaSet(set<shared_ptr<MongodDaemon>> daemonList,
					unsigned int replicationFactor)
			{
				DistanceParams params;
				params.daemonCount = daemonList.size();
				params.daemons = daemonList;
				params.replicationFactor = replicationFactor;

				vector<shared_ptr<MongodDaemon>> daemonVector(daemonList.begin(), daemonList.end());

				vector<unsigned int> bestSelection;
				Distance bestDistance;

				vector<unsigned int> currentSelection(replicationFactor);
				firstIndexVector(currentSelection);
				do {
					set<shared_ptr<MongodDaemon>> currentSet;
					for (unsigned int i = 0; i < replicationFactor; i++) {
						currentSet.insert(daemonVector[currentSelection[i]]);
					}
					Distance currentDistance = distanceFunction(params, currentSet);
					if (currentDistance >= bestDistance) {
						bestDistance = currentDistance;
						bestSelection = currentSelection;
					}
				} while (nextIndexVector(currentSelection, daemonList.size() - 1));

				LOG4CXX_DEBUG(logger, "Found replica set with hosts:");
				set<shared_ptr<MongodDaemon>> replicaSetServers;
				for (unsigned int i = 0; i < replicationFactor; i++) {
					replicaSetServers.insert(daemonVector[bestSelection[i]]);
					LOG4CXX_DEBUG(logger, "\t" << daemonVector[bestSelection[i]]->getHostname());
				}

				LOG4CXX_DEBUG(logger, "With distance:" <<
						" uhs(" << bestDistance.uniqueHostsStores << ")" <<
						" ro(" << bestDistance.rackOverload << ")" <<
						" dco(" << bestDistance.dataCenterOverload << ")" <<
						" ur(" << bestDistance.uniqueRacks << ")" <<
						" udc(" << bestDistance.uniqueDataCenters << ")" <<
						" bhs(" << bestDistance.busyHostsStores << ")");

				if (isAcceptableReplicaSet(replicaSetServers)) {
					LOG4CXX_DEBUG(logger, "Replica set accepted");
					shared_ptr<ReplicaSet> replicaSet = make_shared<ReplicaSet>();
					replicaSet->addDataServers(replicaSetServers);
					return replicaSet;
				} else {
					LOG4CXX_DEBUG(logger, "Replica set not accepted");
					return shared_ptr<ReplicaSet>();
				}
			}

			bool filterDaemonsIncludeRogueDataServers(shared_ptr<Daemon> daemon)
			{
				if (daemon->getType() == MongoDaemonType::DATA_SERVER) {
					MongodDaemon *dataServer = (MongodDaemon *)daemon.get();
					if (dataServer->getReplSet() == "") {
						return true;
					}
				}
				return false;
			}

			void DaemonDeployer::deployDataServers()
			{
				unsigned int replicationFactor = CONFIG_GETINT(CONFIG_MONGO_REPLICATION_FACTOR);

				set<shared_ptr<MongodDaemon>> rogueDataServers = castSetToType<Daemon, MongodDaemon>(
						this->daemonCollection->getDaemonsFiltered(filterDaemonsIncludeRogueDataServers));
				while (rogueDataServers.size() >= replicationFactor) {
					shared_ptr<ReplicaSet> replicaSet = selectReplicaSet(rogueDataServers, replicationFactor);
					if (replicaSet) {
						this->replicaSetManager->addReplicaSet(replicaSet);

						rogueDataServers = castSetToType<Daemon, MongodDaemon>(
								this->daemonCollection->getDaemonsFiltered(filterDaemonsIncludeRogueDataServers));
					} else {
						return;
					}
				}
			}

			bool DaemonDeployer::configServersActive()
			{
				int requiredConfigServers = CONFIG_GETINT(CONFIG_MONGO_CONFIG_REQUIREDNUMBER);

				int activeConfigServers =
						this->daemonCollection->getLaunchedDaemonsForType(MongoDaemonType::CONFIG_SERVER).size();

				return activeConfigServers >= requiredConfigServers;
			}

			bool DaemonDeployer::queryRouterActive()
			{
				return !this->daemonCollection->getLaunchedDaemonsForType(MongoDaemonType::QUERY_ROUTER).empty();
			}

		}
	}
}
