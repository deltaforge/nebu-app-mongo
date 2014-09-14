
#include "mongoAdapter.h"

#include "mongo/client/dbclient.h"
#include "log4cxx/logger.h"

// Using declarations - mongo-cxx-driver
using mongo::BSONObj;
using mongo::DBClientConnection;
// Using declarations - standard library
using std::make_shared;
using std::shared_ptr;
using std::string;

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("nebu.app.mongo.MongoAdapter"));

namespace nebu
{
	namespace app
	{
		namespace mongo
		{

			shared_ptr<MongoAdapter> MongoAdapter::instance;

			BSONObj MongoAdapter::executeCommand(const string &host, const string &database, const BSONObj &command) const
			{
				LOG4CXX_DEBUG(logger, "Executing mongo command on host '" << host << "', database '" <<
						database << "', '" << database << "\n\tCommand: " << command.toString() << "'");

				BSONObj output;
				DBClientConnection db;
				db.connect(host);
				db.runCommand(database, command, output);

				LOG4CXX_DEBUG(logger, "\tReturned with: '" << output.toString() << "'");
				return output;
			}

			shared_ptr<MongoAdapter> MongoAdapter::getInstance()
			{
				if (!MongoAdapter::instance) {
					MongoAdapter::setInstance(make_shared<MongoAdapter>());
				}
				return MongoAdapter::instance;
			}

			void MongoAdapter::setInstance(shared_ptr<MongoAdapter> instance)
			{
				MongoAdapter::instance = instance;
			}

		}
	}
}
