
#include "daemonDeployer.h"
#include "daemonManager.h"
#include "mongoConfiguration.h"
#include "mongoDaemon.h"

#include "log4cxx/logger.h"

#include <sstream>

// Using declarations - standard library
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::stringstream;
// Using declarations - nebu-common
using nebu::common::VirtualMachine;
// Using declarations - nebu-app-framework
using nebu::app::framework::Daemon;
using nebu::app::framework::DaemonCollection;
using nebu::app::framework::DaemonType;
using nebu::app::framework::VMEvent;

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nebu.app.mongo.DaemonManager"));

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			DaemonManager::DaemonManager(shared_ptr<DaemonCollection> daemonCollection, shared_ptr<DaemonDeployer> deployer) :
					daemonCollection(daemonCollection), deployer(deployer)
			{

			}

			void DaemonManager::refreshDaemons()
			{

			}

			void DaemonManager::deployDaemons()
			{
				this->deployer->deployDaemons();
			}

			DaemonType getTypeFromHostname(const string &hostname)
			{
				if (hostname.find(CONFIG_GET(CONFIG_MONGO_CONFIG_HOSTPREFIX)) == 0) {
					return MongoDaemonType::CONFIG_SERVER;
				} else if (hostname.find(CONFIG_GET(CONFIG_MONGO_ROUTER_HOSTPREFIX)) == 0) {
					return MongoDaemonType::QUERY_ROUTER;
				} else if (hostname.find(CONFIG_GET(CONFIG_MONGO_DATA_HOSTPREFIX)) == 0) {
					return MongoDaemonType::DATA_SERVER;
				}

				return MongoDaemonType::UNKNOWN;
			}

			void DaemonManager::newVMAdded(shared_ptr<VirtualMachine> vm)
			{
				DaemonType daemonType = getTypeFromHostname(vm->getHostname());
				if (daemonType == MongoDaemonType::UNKNOWN) {
					LOG4CXX_INFO(logger, "Attempt to register VM of unknown type");
					return;
				}

				shared_ptr<Daemon> daemon;
				switch (daemonType) {
				case MongoDaemonType::CONFIG_SERVER:
					daemon = make_shared<MongocfgDaemon>(vm, CONFIG_GETINT(CONFIG_MONGO_CONFIG_PORT));
					LOG4CXX_INFO(logger, "Registering config server: " << daemon->getHostname());
					break;
				case MongoDaemonType::DATA_SERVER:
					daemon = make_shared<MongodDaemon>(vm, CONFIG_GETINT(CONFIG_MONGO_DATA_PORT));
					LOG4CXX_INFO(logger, "Registering data server: " << daemon->getHostname());
					break;
				case MongoDaemonType::QUERY_ROUTER:
					daemon = make_shared<MongosDaemon>(vm, CONFIG_GETINT(CONFIG_MONGO_ROUTER_PORT));
					LOG4CXX_INFO(logger, "Registering query router: " << daemon->getHostname());
					break;
				default:
					LOG4CXX_ERROR(logger, "Encountered unknown Daemon::Type");
					return;
				}

				this->daemonCollection->addDaemon(daemon);
			}
			void DaemonManager::existingVMChanged(shared_ptr<VirtualMachine> vm __attribute__((unused)),
					const VMEvent event __attribute__((unused)))
			{
				// NOT USED
			}
			void DaemonManager::oldVMRemoved(const VirtualMachine &vm __attribute__((unused)))
			{
				// NOT USED
			}

		}
	}
}

