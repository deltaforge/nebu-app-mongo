
#ifndef NEBUAPPMONGO_REPLICASETMANAGER_H_
#define NEBUAPPMONGO_REPLICASETMANAGER_H_

#include <memory>
#include <set>

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			class MongosDaemon;
			class ReplicaSet;

			class ReplicaSetManager
			{
			public:
				ReplicaSetManager();
				virtual ~ReplicaSetManager() { };

				virtual void addReplicaSet(std::shared_ptr<ReplicaSet> replicaSet);
				virtual std::set<std::shared_ptr<ReplicaSet>> getReplicaSets() const;

				virtual void registerReplicaSetsWithRouter(std::shared_ptr<MongosDaemon> queryRouter);

			private:
				std::set<std::shared_ptr<ReplicaSet>> replicaSets;
				unsigned int nextReplicaSetNo;

				virtual bool registerReplicaSetWithRouter(
						std::shared_ptr<ReplicaSet> replicaSet,
						std::shared_ptr<MongosDaemon> queryRouter);
			};

		}
	}
}

#endif
