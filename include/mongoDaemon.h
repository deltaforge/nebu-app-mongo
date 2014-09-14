
#ifndef NEBUAPPMONGO_MONGODAEMON_H_
#define NEBUAPPMONGO_MONGODAEMON_H_

#include "nebu-app-framework/daemon.h"

namespace nebu
{
	namespace app
	{
		namespace mongo
		{
			namespace MongoDaemonType
			{
				enum {
					UNKNOWN,
					CONFIG_SERVER,
					DATA_SERVER,
					QUERY_ROUTER
				};
			}

			class MongoDaemon : public framework::Daemon
			{
			public:
				MongoDaemon(std::shared_ptr<nebu::common::VirtualMachine> hostVM, int portNo) :
					framework::Daemon(hostVM), portNo(portNo) { }
				virtual ~MongoDaemon() { }

				virtual int getPortNo() { return this->portNo; }
				virtual std::string getHostPortPair();

			protected:
				int portNo;
			};

		}
	}
}

#endif
