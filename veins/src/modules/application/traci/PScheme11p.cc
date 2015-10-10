//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "application/traci/PScheme11p.h"
#include <stdlib.h>

using Veins::TraCIMobilityAccess;
using Veins::AnnotationManagerAccess;

const simsignalwrap_t PScheme11p::parkingStateChangedSignal = simsignalwrap_t(TRACI_SIGNAL_PARKING_CHANGE_NAME);

Define_Module(PScheme11p);

void PScheme11p::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        traci = TraCIMobilityAccess().get(getParentModule());
        annotations = AnnotationManagerAccess().getIfExists();
        ASSERT(annotations);

        sentMessage = false;
        lastDroveAt = simTime();
        isParking = false;
    }
}

void PScheme11p::onBeacon(WaveShortMessage* wsm) {
    findHost()->getDisplayString().updateWith("r=16,green");
}

void PScheme11p::onData(WaveShortMessage* wsm) {

}

void PScheme11p::sendMessage(std::string blockedRoadId) {

}

void PScheme11p::handleSelfMsg(cMessage* msg) {
    switch (msg->getKind()) {
        case SEND_BEACON_EVT: {
            WaveShortMessage* wsm = prepareWSM("beacon", beaconLengthBits, type_CCH, beaconPriority, 0, -1);
            wsm->setSpeed(traci->getSpeed());
            wsm->setAngleRad(traci->getAngleRad());
            wsm->setvecX(traci->getSpeed(),traci->getAngleRad());
            wsm->setvecY(traci->getSpeed(),traci->getAngleRad());
            wsm->setId(traci->getExternalId());
            if(simTime()>=20){
                    if ((int)simTime().dbl()%20 == 0){
                        add = senderAddress;
                        wsm->setSenderAddress(senderAddress++);
                    }
                    else{
                        wsm->setSenderAddress(add);
                   }
            }
            sendWSM(wsm);
            scheduleAt(simTime() + par("beaconInterval").doubleValue(), sendBeaconEvt);
            break;
        }
        default: {
            if (msg)
                DBG << "APP: Error: Got Self Message of unknown kind! Name: " << msg->getName() << endl;
            break;
        }
    }
}

void PScheme11p::receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj) {
    Enter_Method_Silent();
    handlePositionUpdate(obj);
}
void PScheme11p::handleParkingUpdate(cObject* obj) {

}
void PScheme11p::handlePositionUpdate(cObject* obj) {
    BaseWaveApplLayer::handlePositionUpdate(obj);
}
void PScheme11p::sendWSM(WaveShortMessage* wsm) {
    if (isParking && !sendWhileParking) return;
    sendDelayedDown(wsm,individualOffset);
}
