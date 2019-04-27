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

#ifndef _OVERLAY_MANAGER_H_
#define _OVERLAY_MANAGER_H_

#include <cstdint>
#include <omnetpp.h>
#include "../common/HostBase.h"
#include "../common/Util.h"
#include "../ctrl/ChordCtrl.h"

#include "GlobalNodeListAccess.h"
#include "GlobalParametersAccess.h"

#include "../objects/IPAddress.h"
#include "../objects/IPvXAddress.h"
#include "../objects/SimpleNodeEntry.h"
#include "../objects/SimpleInfo.h"
#include "../objects/TransportAddress.h"
#include "../others/InterfaceTableAccess.h"

#include "../transport/NetworkAddress.h"

using namespace std;
using namespace omnetpp;

/**
 * Base class for configurators of different underlay models
 *
 * @author Stephan Krause, Bernhard Heep
 */
class OverlayManager: public cSimpleModule {
public:
    OverlayManager();
    virtual ~OverlayManager();
//    void removeNode(IPvXAddress& nodeAddr);
//    void addNode();
protected:
    /**
     * OMNeT init methods
     */
    virtual void initialize();
    /**
     * Node mobility simulation
     *
     * @param msg timer-message
     */
    virtual void handleMessage(cMessage* msg);

private:
    // chord node creation counter
    int chord_counter;

    GlobalNodeList* globalNodeList;
    GlobalParameters* parameterList;

    void initChordOverlay(cMessage* msg);
};

#endif
