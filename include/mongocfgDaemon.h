
#ifndef NEBUAPPMONGO_MONGOCFGDAEMON_H_
#define NEBUAPPMONGO_MONGOCFGDAEMON_H_

#include "mongoDaemon.h"

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			class MongocfgDaemon : public MongoDaemon
			{
			public:
				MongocfgDaemon(std::shared_ptr<nebu::common::VirtualMachine> hostVM, int portNo) :
					MongoDaemon(hostVM, portNo) { }
				virtual ~MongocfgDaemon() { }

				virtual bool launch();

				virtual framework::DaemonType getType() const
				{
					return MongoDaemonType::CONFIG_SERVER;
				}
			};

		}
	}
}

#endif

