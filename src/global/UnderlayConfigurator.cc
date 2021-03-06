//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "UnderlayConfigurator.h"

#include "../ctrl/ChordCtrl.h"

#include "../server/connection.h"

#include "../server/utility.h"
#include "../server/clientserver.h"

#include "../app/ClientApp.h"

using namespace con;

Define_Module(UnderlayConfigurator);

#define LOGIN_PORT 20000

UnderlayConfigurator::UnderlayConfigurator() {
    login_con = NULL;
    lcCreated = 0;
    clientCreated = 0;
    neighborCreated = 0;
    loginMsg = NULL;
    stopwatch = 0;
    chord_counter = 0;
}

UnderlayConfigurator::~UnderlayConfigurator() {
    disposeFailures();
    if (loginMsg != NULL) {
        cancelAndDelete(loginMsg);
    }
    if (login_con != NULL) {
        delete login_con;
    }
}

int UnderlayConfigurator::numInitStages() const {
    return 1;
}

void UnderlayConfigurator::initialize(int stage) {
    sockets_init();
    login_con = new con::Connection(PROTOCOL_ID, 512);
    globalNodeList = GlobalNodeListAccess().get();
    parameterList = GlobalParametersAccess().get();
    coordinator = CoordinatorAccess().get();

    login_con->setTimeoutMs(0);
    login_con->ServeLogin(LOGIN_PORT);
    cout << "Login Server started" << endl;
    loginLoop = par("login_loop").doubleValue();
    loginMsg = new cMessage(msg::LOGIN_LOOP);

    remove_wait = par("remove_wait").doubleValue();

    WATCH_MAP(death_schedule);

    // initialize Chord overlay
    cMessage* clientInit = new cMessage(msg::INIT_CHORD);
    scheduleAt(0, clientInit);
    scheduleAt(simTime() + loginLoop, loginMsg);

    // initialize clients
    cMessage* npcInit = new cMessage(msg::NPC_INIT);
    scheduleAt(0, npcInit);
}

void UnderlayConfigurator::finish() {
    sockets_cleanup();
}

void UnderlayConfigurator::handleMessage(cMessage* msg) {
    if (msg->isName(msg::FAILURE)) {
        Failure* failure = check_and_cast<Failure*>(msg);
        handleNodeFailure(failure);
    } else if (msg->isName(msg::LOGIN_LOOP)) {
        handleClientLogin(msg);
    } else if (msg->isName(msg::REVOKE_HOST)) {
        Termination* t = check_and_cast<Termination*>(msg);
        removeHost(t);
    } else if (msg->isName(msg::INIT_CHORD)) {
        initChordOverlay(msg);
    } else if (msg->isName(msg::NPC_INIT)) {
        handleNPCInit(msg);
    }
}

void UnderlayConfigurator::handleNodeFailure(Failure* failure) {
    std::string hostAddress = failure->getHostAddress();
    EV << "[" << simTime() << "s] " << hostAddress << " is failed" << endl;
    IPvXAddress ipAddress(hostAddress.c_str());
    revokeNode(ipAddress, true);
}

bool compare(ChordCtrl* a, ChordCtrl* b) {
    return a->chordId < b->chordId;
}

void UnderlayConfigurator::initChordOverlay(cMessage* msg) {
    vector<ChordCtrl*> overlay;
    int size = parameterList->getChordInitSize();

    const char* chordType = par("chord_type");
    const char* chordName = par("chord_name");

    for (int i = 0; i < size; i++) {
        // create a new node
        cModuleType *moduleType = cModuleType::get(chordType);
        cModule* parent = getParentModule()->getSubmodule("ContentStorage");
        // create (possibly compound) module and build its submodules (if any)
        cModule* chord = moduleType->create(chordName, parent,
                chord_counter + 1, chord_counter);
        chord_counter++;
        // set up parameters, if any
        chord->finalizeParameters();
        chord->buildInside();
        // create activation messages
        chord->scheduleStart(0);
        chord->callInitialize(0);
        // create address for the chord control protocol
        ChordCtrl* ctrl = check_and_cast<ChordCtrl*>(
                chord->getSubmodule("ctrl"));
        IPvXAddress addr = IPAddress(NetworkAddress::freeAddress());
        ctrl->setIPAddress(addr);
        ctrl->chordId = util::getSHA1(addr.get4().str() + "4000",
                parameterList->getAddrSpaceSize()); // Hash(IP || port)
        chord->callInitialize(1);
        // create meta information
        SimpleNodeEntry* entry = new SimpleNodeEntry(chord);
        ChordInfo* info = new ChordInfo(chord->getId(), chord->getFullName());
        info->setEntry(entry);
        info->setChordId(ctrl->chordId);
        //add host to bootstrap oracle
        globalNodeList->addPeer(addr, info);
        overlay.push_back(ctrl);
    }

    // sort chord nodes by chordId
    std::sort(overlay.begin(), overlay.end(), compare);

    // create finger table and successor list for each chord node
    for (int i = 0; i < overlay.size(); i++) {
        ChordCtrl* ctrl = overlay[i];
        for (int a = 0; a < parameterList->getSuccListSize(); a++) {
            int index = (a + i + 1) % overlay.size();
            ctrl->successorList[a] = overlay[index]->chordId;
        }
        if (i > 0) {
            ctrl->predecessor = overlay[i - 1]->chordId;
        } else {
            ctrl->predecessor = overlay[overlay.size() - 1]->chordId;
        }
        int fingerTableSize = parameterList->getAddrSpaceSize();
        for (int j = 0; j < fingerTableSize; j++) {
            unsigned long a = (unsigned long) (ctrl->chordId
                    + (unsigned long) pow(2, j))
                    % (unsigned long) pow(2, fingerTableSize);
            // find the chord node by for the given ID
            ChordCtrl* cp = overlay[0];
            for (int k = 0; k < overlay.size(); k++) {
                ChordCtrl* temp = overlay[k];
                if (temp->chordId >= a) {
                    cp = temp;
                    break;
                }
            }
            ctrl->fingerTable[j] = cp->chordId;
        }
    }

    for (auto elem : overlay) {
        GlobalNodeListAccess().get()->ready(elem->chordId);
        elem->startMaint();
    }

    delete msg;
}

void UnderlayConfigurator::handleClientLogin(cMessage* msg) {

    // for simulation delay estimation
    simtime_t clock = simTime();
    double base = 5.0;   // 5s
    if (clock >= base * stopwatch) {
        cout << endl << "current time: " << clock << "s" << endl << endl;
        stopwatch++;
    }

    u32 data_maxsize = 10000;
    Buffer<u8> data(data_maxsize);
    u32 datasize;
    u16 peer_id;

    try {
        {
            datasize = login_con->Receive(peer_id, *data, data_maxsize);
        }
        if (datasize == 6) {
            ToServerCommand command = (ToServerCommand) readU16(&data[0]);
            if (command == TOSERVER_PLAYERPORT) {
                unsigned short port = readS16(&data[2]);
                unsigned short playerId = readS16(&data[4]);

                std::cout << "received TOSERVER_PLAYERPORT from " << peer_id
                        << " for port: " << port << endl;
                std::cout << "received playerId " << playerId << endl;

                // create client
                // initialize clients
                const char* clientType = par("clientType");
                const char* clientName = par("clientName");
                // create a new client
                cModuleType* moduleType = cModuleType::get(clientType);
                cModule* parent = getParentModule();
                // create (possibly compound) module and build its submodules (if any)
                cModule* client = moduleType->create(clientName, parent,
                        clientCreated + 1, clientCreated);
                clientCreated++;
                // set up parameters, if any
                client->finalizeParameters();
                client->buildInside();
                // create activation message
                client->scheduleStart(simTime());
                client->callInitialize(0);
                // configure server port
                check_and_cast<ClientApp*>(client->getSubmodule("app"))->port =
                        port;
                // set peer ID
                check_and_cast<ClientApp*>(client->getSubmodule("app"))->setPeerId(
                        playerId);
                client->callInitialize(1);
//                ClientInfo info = coordinator->addClient(client->getFullName());
                // initialize the client location
                client->getDisplayString().setTagArg("p", 0, 0L);
                client->getDisplayString().setTagArg("p", 1, 0L);

                // reply to the client
                SharedBuffer<u8> reply(2 + 2);
                writeU16(&reply[0], TOCLIENT_LOGIN);
                unsigned short ack = 1;
                writeU16(&reply[2], ack);
                // Send as unreliable
                login_con->Send(peer_id, 0, reply, true);
            }
        }
    } catch (con::InvalidIncomingDataException &e) {
        std::cout << "Login server::Receive(): "
                "InvalidIncomingDataException: what()=" << e.what()
                << std::endl;
    } catch (con::NoIncomingDataException &e) {
        ;
    }
    scheduleAt(simTime() + loginLoop, loginMsg);
}

void UnderlayConfigurator::handleNPCInit(cMessage* msg) {
    // create NPCs
    int neighborNum = getSimulation()->getModuleByPath("VirtualNet")->par(
            "npc_num");
    const char* neighborType = par("npcType");
    const char* neighborName = par("npcName");
    for (int i = 0; i < neighborNum; i++) {
        // create a new neighbor
        cModuleType* moduleType = cModuleType::get(neighborType);
        cModule* parent = getParentModule();
        // create (possibly compound) module and build its submodules (if any)
        cModule* neighbor = moduleType->create(neighborName, parent,
                neighborCreated + 1, neighborCreated);
        neighborCreated++;
        // set up parameters, if any
        neighbor->finalizeParameters();
        neighbor->buildInside();
        // create activation message
        neighbor->scheduleStart(simTime());
        neighbor->callInitialize(0);
        neighbor->callInitialize(1);
        // initialize the client location
        ClientInfo info = coordinator->getClientInfo(neighbor->getFullName());
        neighbor->getDisplayString().setTagArg("p", 0, info.getX());
        neighbor->getDisplayString().setTagArg("p", 1, info.getY());
    }

    delete msg;
}

TransportAddress* UnderlayConfigurator::createLogicComputer() {
    const char* hostType = par("meshType");
    const char* hostName = par("meshName");

// create a new node
    cModuleType *moduleType = cModuleType::get(hostType);
    cModule* parent = getParentModule();
// create (possibly compound) module and build its submodules (if any)
    cModule *LC = moduleType->create(hostName, parent, lcCreated + 1,
            lcCreated);
    lcCreated++;
// set up parameters, if any
    LC->finalizeParameters();
    LC->buildInside();
// create activation message
    LC->scheduleStart(simTime());
    LC->callInitialize(0);

// create address for the Rendezvous node
    IPvXAddress addr = IPAddress(NetworkAddress::freeAddress());
    cModule* rendezvous = LC->getSubmodule("rendezvous");
    HostBase* rendezvousCtrl = check_and_cast<HostBase*>(
            rendezvous->getSubmodule("ctrl"));
    rendezvousCtrl->setIPAddress(addr);
    LC->callInitialize(1);

// place the logical computer
    long x = coordinator->random_LC_X();
    long y = coordinator->random_LC_Y();
    LC->getDisplayString().setTagArg("p", 0, x);
    LC->getDisplayString().setTagArg("p", 1, y);

// create meta information
    SimpleNodeEntry* entry = new SimpleNodeEntry(rendezvous);
    SimpleInfo* info = new SimpleInfo(rendezvous->getId(),
            rendezvous->getFullName());
    info->setEntry(entry);
//add host to bootstrap oracle
    globalNodeList->addPeer(addr, info);

    TransportAddress *address = new TransportAddress(addr);
    return address;
}

TransportAddress* UnderlayConfigurator::registerEndpoint(cModule* host) {
    IPvXAddress addr = IPAddress(NetworkAddress::freeAddress());
// create meta information
    SimpleNodeEntry* entry = new SimpleNodeEntry(host);
    SimpleInfo* info = new SimpleInfo(host->getId(), host->getFullName());
    info->setEntry(entry);
//add node to bootstrap oracle
    globalNodeList->addPeer(addr, info);

    TransportAddress *address = new TransportAddress(addr);
    return address;
}

TransportAddress* UnderlayConfigurator::createNode(cModule* parent,
        int nodeCreated) {
    Enter_Method_Silent("Create a new node");

    const char* hostType = par("nodeType");
    const char* hostName = par("nodeName");

// create a new node
    cModuleType *moduleType = cModuleType::get(hostType);
// create (possibly compound) module and build its submodules (if any)
    cModule *host = moduleType->create(hostName, parent, nodeCreated + 1,
    nodeCreated);
    std::string nodeName = host->getFullName();
    EV << "[" << simTime() << "s] create host: " << nodeName << endl;

// set up parameters, if any
    host->finalizeParameters();
    host->buildInside();
// create activation message
    host->scheduleStart(simTime());
    host->callInitialize(0);

    IPvXAddress addr = IPAddress(NetworkAddress::freeAddress());
    IPv4InterfaceData* ipData = new IPv4InterfaceData();
    ipData->setIPAddress(addr.get4());
    InterfaceEntry* ie = new InterfaceEntry();
    std::string IPInterface = host->par("IPInterface");
    ie->setName(IPInterface.c_str());
    ie->setIPv4Data(ipData);
    InterfaceTable* ift = InterfaceTableAccess().get(host);
    ift->addInterface(ie);
    host->callInitialize(1);
// create meta information
    SimpleNodeEntry* entry = new SimpleNodeEntry(host);
    SimpleInfo* info = new SimpleInfo(host->getId(), host->getFullName());
    info->setEntry(entry);
//add node to bootstrap oracle
    globalNodeList->addPeer(addr, info);

// time for node failure
    Failure* failure = new Failure(msg::FAILURE);
    std::string addrStr = addr.get4().str();
    failure->setHostAddress(addrStr.c_str());
    failures[addrStr] = failure;
    simtime_t lifespan = ChurnGeneratorAccess().get()->getSessionLength();
    simtime_t deathTime = simTime() + lifespan;
    scheduleAt(deathTime, failure);
    death_schedule[addrStr] = deathTime;
    TransportAddress *address = new TransportAddress(addr);
    return address;
}

void UnderlayConfigurator::removeNode(IPvXAddress& nodeAddr) {
    SimpleInfo* info = dynamic_cast<SimpleInfo*>(globalNodeList->getPeerInfo(
            nodeAddr));
    if (info != nullptr) {
        SimpleNodeEntry* destEntry = info->getEntry();
        cModule* node = destEntry->getUdpIPv4Gate()->getOwnerModule();
        node->callFinish();
        node->deleteModule();
        globalNodeList->killPeer(nodeAddr);
        parameterList->remoteHost(nodeAddr);
        std::string addr = nodeAddr.get4().str();
        Failure* failure = failures[addr];
        cancelAndDelete(failure);
        failures.erase(nodeAddr.get4().str());
    }
}

void UnderlayConfigurator::removeHost(Termination* t) {

    cModule* host = getSimulation()->getModuleByPath(t->getModulePath());

    cout << "Remove host: " << host->getFullName() << endl;

    host->callFinish();
    host->deleteModule();

    delete t;
}

void UnderlayConfigurator::revokeNode(IPvXAddress& nodeAddr,
        bool toRemoveNode) {
    Enter_Method_Silent("Revoke a node");

    if(toRemoveNode) {
        Termination* t = new Termination(msg::REVOKE_HOST);
        SimpleInfo* info = dynamic_cast<SimpleInfo*>(globalNodeList->getPeerInfo(
                nodeAddr));
        SimpleNodeEntry* destEntry = info->getEntry();
        cModule* node = destEntry->getUdpIPv4Gate()->getOwnerModule();
        t->setModulePath(node->getFullPath().c_str());
        // module removal waiting period for message cleanup
        scheduleAt(simTime() + remove_wait, t);
    }

    globalNodeList->killPeer(nodeAddr);
    parameterList->remoteHost(nodeAddr);
    std::string addr = nodeAddr.get4().str();
    Failure* failure = failures[addr];
    if(failure != NULL) {
        cancelAndDelete(failure);
        failure = NULL;
    }
    failures.erase(nodeAddr.get4().str());
}

void UnderlayConfigurator::revokeLogicComputer(IPvXAddress& nodeAddr) {
    Enter_Method_Silent("Revoke a Mesh");

    SimpleInfo* info = dynamic_cast<SimpleInfo*>(globalNodeList->getPeerInfo(
            nodeAddr));
    SimpleNodeEntry* destEntry = info->getEntry();
    cModule* LC = destEntry->getUdpIPv4Gate()->getOwnerModule()->getParentModule();
    Termination* t = new Termination(msg::REVOKE_HOST);
    t->setModulePath(LC->getFullPath().c_str());
    // module removal waiting period for message cleanup
    scheduleAt(simTime() + remove_wait, t);

    globalNodeList->killPeer(nodeAddr);
    parameterList->remoteHost(nodeAddr);
}

void UnderlayConfigurator::revokeClient(IPvXAddress nodeAddr) {
    Enter_Method_Silent("Revoke a client");

    SimpleInfo* info = dynamic_cast<SimpleInfo*>(globalNodeList->getPeerInfo(
            nodeAddr));
    SimpleNodeEntry* destEntry = info->getEntry();
    cModule* client = destEntry->getUdpIPv4Gate()->getOwnerModule();
    Termination* t = new Termination(msg::REVOKE_HOST);
    t->setModulePath(client->getFullPath().c_str());
    // module removal waiting period for message cleanup
    scheduleAt(simTime() + remove_wait, t);

    globalNodeList->killPeer(nodeAddr);
    parameterList->remoteHost(nodeAddr);
}

void UnderlayConfigurator::disposeFailures() {
    try {
        for (std::map<std::string, Failure*>::iterator it = failures.begin();
                it != failures.end(); ++it) {
            Failure* failure = it->second;
            if (failure != NULL) {
                cancelAndDelete(failure);
            }
        }
        failures.clear();
    } catch (exception& e) {
        EV << e.what() << endl;
    }
}
