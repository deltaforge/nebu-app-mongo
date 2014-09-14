
#ifndef NEBUAPPMONGO_MONGOADAPTER_H_
#define NEBUAPPMONGO_MONGOADAPTER_H_

#include "mongo/bson/bson.h"

#include <memory>
#include <string>

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			class MongoAdapter
			{
			public:
				MongoAdapter() { }
				virtual ~MongoAdapter() { }

				virtual ::mongo::BSONObj executeCommand(const std::string &host, const std::string &database,
						const ::mongo::BSONObj &command) const;

				static std::shared_ptr<MongoAdapter> getInstance();
				static void setInstance(std::shared_ptr<MongoAdapter> instance);

			private:
				static std::shared_ptr<MongoAdapter> instance;
			};

		}
	}
}

#endif
