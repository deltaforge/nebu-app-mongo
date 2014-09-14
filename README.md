Nebu MongoDB Extension
===

The Nebu MongoDB Extension (nebu-mongo) is an application that uses the Nebu system to provide virtualization-awareness to MongoDB (requires MongoDB version >= 2.6). It must be deployed on a machine that has SSH access to the machines that will run MongoDB. Nebu-mongo polls the Nebu core system for virtual machines. Whenever new machines are deployed through Nebu, the MongoDB extension will deploy MongoDB on those machines and add them to the cluster.

Deploying the Nebu MongoDB Extension
---

To deploy the Nebu MongoDB Extension, you need to have a running instance of the [Nebu core system](https://github.com/deltaforge/nebu-core). For this guide, we will assume this instance is running at nebu-master:1234 (can be anywhere as long as the host is accessible and the port is open). Before starting the MongoDB extension, you will need to create an application in the Nebu system (using the /app POST request). We will assume this application's id is "my-mongo-application". The Nebu MongoDB Extension must have SSH access to the machines that will be launched by Nebu (i.e. it should be placed in the same VLAN), and it is recommended to run the Nebu MongoDB Extension on a small, separate machine. You can start the application using:

	nebu-mongo --app my-mongo-application --nebu nebu-master:1234

Note that nebu-mongo does not currently daemonize, so it is recommended to run it using e.g. screen.

To add nodes to the MongoDB extension using the default configuration, create and deploy three VM templates in Nebu:

| Daemon type | Hostname prefix | Amount |
| Config Server | mongo-config- | 3 |
| Query Router | mongo-router- | 1+ |
| Datanode | mongo-data- | 3N |

Requirements for Virtual Machine template
---

The virtual machines for the different daemons can all use the same base template (a VM to clone). The requirements for this template are:

 * The virtual machine must have MongoDB version 2.6 or higher (can be deployed through e.g. puppet).
 * The virtual machine must have a user called "mongodb" with full access to the MongoDB directory.
 * The mongodb user must be accessible through passwordless SSH from the account and machine running the Nebu MongoDB Extension.
 * The home directory of the mongodb user should contain:
   * An (empty) file called ".puppetcomplete" to signal that MongoDB is available (a hack used during development to detect when puppet had completed, will be changed in the future).
 * The directory /data/mongo exists and is owned by the mongodb user.
 * The directory containing the mongodb executables (mongod, mongos, etc.) is part of the mongodb user's PATH variable.

The specific directory structure, names, etc. can be changed by editing the bash files in the scripts/ directory of this repository.

Dependencies
---

Runtime dependencies:

 * [libnebu-app-framework](https://github.com/deltaforge/nebu-app-framework-cpp), version 0.1.
 
Compile dependencies:

 * automake, version 2.61
 * g++, version 4.4.7
 * Development packages of the runtime dependencies.
 
Compiling
---

To compile nebu-mongo, execute the following commands:

	./bootstrap
    ./configure
    make
    make install

License
---

The Nebu MongoDB Extension is distributed under the LGPL version 3 license. See the provided LICENSE file for more information.
