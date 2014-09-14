
#ifndef NEBUAPPMONGO_REPLICASET_H_
#define NEBUAPPMONGO_REPLICASET_H_

#include <memory>
#include <set>

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			class MongodDaemon;

			class ReplicaSet
			{
			public:
				ReplicaSet();
				virtual ~ReplicaSet() { };

				virtual void addDataServer(std::shared_ptr<MongodDaemon> dataServer);
				virtual void addDataServers(std::set<std::shared_ptr<MongodDaemon>> dataServers);
				virtual std::set<std::shared_ptr<MongodDaemon>> getDataServers() const;

				virtual std::string getName() const;
				virtual void setName(const std::string &name);

				virtual bool launch();
				virtual bool hasLaunched() const;
				virtual bool hasRegistered() const;
				virtual void setRegistered();

			private:
				std::set<std::shared_ptr<MongodDaemon>> dataServers;
				std::string name;
				bool registered;
				bool launched;
			};

		}
	}
}

#endif
