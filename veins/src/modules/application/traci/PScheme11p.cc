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


void PScheme11p::timer_start(unsigned int interval,PScheme11p* p)
{
    std::cout << "timer_time" << simTime()<<traci->getExternalId()<<std::endl;
    //interval = simTime()+20-simTime();
    std::thread([interval,p]() {
        while (true)
        {
            p->do_something();
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
    }).detach();
}

void PScheme11p::do_something()
{
    std::cout << "it is time" << simTime()<<std::endl;
    changeId(traci->getExternalId());
  /*  if (simTime()>=20){// check simulation time at certain time at 20ms
            if ((int)simTime().dbl()%20 == 0){ // mode by 20 means every 20 ms
               changeId(traci->getExternalId()); // change node id by this method
               std::cout << "I am doing somethingffsfd" << simTime()<<std::endl;
            }
    }*/


}


void PScheme11p::myThread(int arg){
    std::cout << "I am doing something" <<simTime()<< std::endl;
  }

void PScheme11p::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        traci = TraCIMobilityAccess().get(getParentModule());
        annotations = AnnotationManagerAccess().getIfExists();
        ASSERT(annotations);
        sentMessage = false;
        isParking = false;
        senderAddress = par("senderAddress").longValue();
       // timer_start(5000,this);
    }
}

void PScheme11p::onBeacon(WaveShortMessage* wsm) {
    findHost()->getDisplayString().updateWith("r=16,green");
}

void PScheme11p::onData(WaveShortMessage* wsm) {

}

void PScheme11p::handleSelfMsg(cMessage* msg) { // it an event/self-msg that has be called by scheduleAt with certain time
    if (simTime()>=20){// check simulation time at certain time at 20ms
            if ((int)simTime().dbl()%20 == 0){ // mode by 20 means every 20 ms
                changeId(traci->getExternalId()); // change node id by this method
            }
    }

    switch (msg->getKind()) {
        case SEND_BEACON_EVT: {
            WaveShortMessage* wsm = prepareWSM("beacon", beaconLengthBits, type_CCH, beaconPriority, 0, -1);
            wsm->setSpeed(traci->getSpeed());
            wsm->setAngleRad(traci->getAngleRad());
            wsm->setvecX(traci->getSpeed(),traci->getAngleRad());
            wsm->setvecY(traci->getSpeed(),traci->getAngleRad());
            wsm->setCarId(traci->getExternalId());
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

int PScheme11p::getActualCarId(string carId){
    int position = getActualPositionId(carId);
      string strid = carId.substr(position);
      int intId = atoi(strid.c_str());
      return intId;
}

int PScheme11p::getActualPositionId(string carId) {
    int position = carId.find ('.')+1;
    return position;
}
void PScheme11p::changeId(std::string carId){

     int changeId = senderAddress++;       // number to be converted to a string
     string nodeId;          // string which will contain the string nodeId
     ostringstream convert;   // stream used for the conversion
     convert << changeId;      // insert the textual representation of 'changeId' in the characters in the stream
     nodeId = convert.str();
     string id = carId.replace(getActualPositionId(carId),carId.length(),nodeId);
     traci->setExternalId(id);
}

void PScheme11p::handlePositionUpdate(cObject* obj) {
    BaseWaveApplLayer::handlePositionUpdate(obj);
}
void PScheme11p::sendWSM(WaveShortMessage* wsm) {
    if (isParking && !sendWhileParking) return;
    sendDelayedDown(wsm,individualOffset);
}

WaveShortMessage*  PScheme11p::prepareWSM(std::string name, int lengthBits, t_channel channel, int priority, int rcvId, int serial) {
    WaveShortMessage* wsm = new WaveShortMessage(name.c_str());
    wsm->addBitLength(headerLength);
    wsm->addBitLength(lengthBits);
    myId = getActualCarId(traci->getExternalId());

    switch (channel) {
        case type_SCH: wsm->setChannelNumber(Channels::SCH1); break; //will be rewritten at Mac1609_4 to actual Service Channel. This is just so no controlInfo is needed
        case type_CCH: wsm->setChannelNumber(Channels::CCH); break;
    }
    wsm->setPsid(0);
    wsm->setPriority(priority);
    wsm->setWsmVersion(1);
    wsm->setTimestamp(simTime());
    wsm->setSenderAddress(myId);
    wsm->setRecipientAddress(rcvId);
    wsm->setSenderPos(curPosition);
    wsm->setSerial(serial);

    if (name == "beacon") {
        DBG << "Creating Beacon with Priority " << priority << " at Applayer at " << wsm->getTimestamp() << std::endl;
    }
    if (name == "data") {
        DBG << "Creating Data with Priority " << priority << " at Applayer at " << wsm->getTimestamp() << std::endl;
    }

    return wsm;
}
