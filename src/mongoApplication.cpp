
#include "daemonDeployer.h"
#include "daemonManager.h"
#include "mongoApplication.h"
#include "mongoConfiguration.h"
#include "replicaSetManager.h"

#include "nebu-app-framework/topologyManager.h"
#include "nebu-app-framework/vmManager.h"

// Using declarations - standard library
using std::make_shared;
using std::shared_ptr;
// Using declarations - nebu-app-framework
using nebu::app::framework::Configuration;

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			void MongoApplication::registerConfigurationOptions()
			{
				Configuration::addCommandLineOption("--config-prefix", CONFIG_MONGO_CONFIG_HOSTPREFIX);
				Configuration::addCommandLineOption("--data-prefix", CONFIG_MONGO_DATA_HOSTPREFIX);
				Configuration::addCommandLineOption("--router-prefix", CONFIG_MONGO_ROUTER_HOSTPREFIX);

				Configuration::addDefaultValue(CONFIG_APP_POLICY, "replicated");
				Configuration::addDefaultValue(CONFIG_APP_POLICY_REPLICATED_MINDATACENTERS, "1");
				Configuration::addDefaultValue(CONFIG_APP_POLICY_REPLICATED_MINRACKS, "2");
				Configuration::addDefaultValue(CONFIG_APP_POLICY_REPLICATED_TIMEOUTDATACENTERS, "0");
				Configuration::addDefaultValue(CONFIG_APP_POLICY_REPLICATED_TIMEOUTRACKS, "300");

				Configuration::addDefaultValue(CONFIG_MONGO_CONFIG_HOSTPREFIX, "mongo-config-");
				Configuration::addDefaultValue(CONFIG_MONGO_CONFIG_PORT, "27019");
				Configuration::addDefaultValue(CONFIG_MONGO_CONFIG_REQUIREDNUMBER, "3");
				Configuration::addDefaultValue(CONFIG_MONGO_DATA_HOSTPREFIX, "mongo-data-");
				Configuration::addDefaultValue(CONFIG_MONGO_DATA_PORT, "27018");
				Configuration::addDefaultValue(CONFIG_MONGO_ROUTER_HOSTPREFIX, "mongo-router-");
				Configuration::addDefaultValue(CONFIG_MONGO_ROUTER_PORT, "27017");
				Configuration::addDefaultValue(CONFIG_MONGO_REPLICATION_FACTOR, "3");
			}

			shared_ptr<framework::DaemonManager> MongoApplication::getDaemonManager()
			{
				if (!this->daemonManager) {
					shared_ptr<DaemonDeployer> daemonDeployer = make_shared<DaemonDeployer>(this->getDaemonCollection(),
							this->getReplicaSetManager(),
							this->getTopologyManager());
					this->daemonManager = make_shared<DaemonManager>(this->getDaemonCollection(), daemonDeployer);
				}
				return this->daemonManager;
			}

			shared_ptr<ReplicaSetManager> MongoApplication::getReplicaSetManager()
			{
				if (!this->replicaSetManager) {
					this->replicaSetManager = make_shared<ReplicaSetManager>();
				}
				return this->replicaSetManager;
			}

		}
	}
}

namespace nebu
{
	namespace app
	{
		namespace framework
		{

			shared_ptr<ApplicationHooks> initApplication()
			{
				return make_shared<nebu::app::mongo::MongoApplication>();
			}

		}
	}
}
