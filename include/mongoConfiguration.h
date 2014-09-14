
#ifndef NEBUAPPMONGO_MONGOCONFIGURATION_H_
#define NEBUAPPMONGO_MONGOCONFIGURATION_H_

#define CONFIG_APP_POLICY                               "app.policy"
#define CONFIG_APP_POLICY_REPLICATED_MINDATACENTERS     "app.policy.replicated.mindatacenters"
#define CONFIG_APP_POLICY_REPLICATED_MINRACKS           "app.policy.replicated.minracks"
#define CONFIG_APP_POLICY_REPLICATED_TIMEOUTDATACENTERS "app.policy.replicated.timeoutdatacenters"
#define CONFIG_APP_POLICY_REPLICATED_TIMEOUTRACKS       "app.policy.replicated.timeoutracks"

#define CONFIG_MONGO_CONFIG_HOSTPREFIX                  "mongo.config.hostprefix"
#define CONFIG_MONGO_CONFIG_PORT                        "mongo.config.port"
#define CONFIG_MONGO_CONFIG_REQUIREDNUMBER              "mongo.config.requirednumber"
#define CONFIG_MONGO_DATA_HOSTPREFIX                    "mongo.data.hostprefix"
#define CONFIG_MONGO_DATA_PORT                          "mongo.data.port"
#define CONFIG_MONGO_ROUTER_HOSTPREFIX                  "mongo.router.hostprefix"
#define CONFIG_MONGO_ROUTER_PORT                        "mongo.router.port"
#define CONFIG_MONGO_REPLICATION_FACTOR                 "mongo.replication.factor"

#include "nebu-app-framework/configuration.h"

#endif
