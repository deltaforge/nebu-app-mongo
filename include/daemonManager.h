
#ifndef NEBUAPPMONGO_DAEMONMANAGER_H_
#define NEBUAPPMONGO_DAEMONMANAGER_H_

#include "mongocfgDaemon.h"
#include "mongodDaemon.h"
#include "mongosDaemon.h"

#include "nebu-app-framework/daemonCollection.h"
#include "nebu-app-framework/daemonManager.h"
#include "nebu/virtualMachine.h"

#include <string>
#include <vector>

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			class DaemonDeployer;

			class DaemonManager : public framework::DaemonManager
			{
			public:
				DaemonManager(std::shared_ptr<framework::DaemonCollection> daemonCollection, std::shared_ptr<DaemonDeployer> deployer);
				virtual ~DaemonManager() { };

				virtual void refreshDaemons();
				virtual void deployDaemons();

				virtual void newVMAdded(std::shared_ptr<nebu::common::VirtualMachine> vm);
				virtual void existingVMChanged(std::shared_ptr<nebu::common::VirtualMachine> vm, const framework::VMEvent event);
				virtual void oldVMRemoved(const nebu::common::VirtualMachine &vm);

			protected:
				std::shared_ptr<framework::DaemonCollection> daemonCollection;
				std::shared_ptr<DaemonDeployer> deployer;
			};

		}
	}
}

#endif

