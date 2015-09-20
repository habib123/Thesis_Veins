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

#include "application/traci/AmrRSU11p.h"

using Veins::AnnotationManagerAccess;

Define_Module(AmrRSU11p);

void AmrRSU11p::initialize(int stage) {
	BaseWaveApplLayer::initialize(stage);
	if (stage == 0) {
	    mobi = dynamic_cast<BaseMobility*> (getParentModule()->getSubmodule("mobility"));
	    ASSERT(mobi);
	    annotations = AnnotationManagerAccess().getIfExists();
	    ASSERT(annotations);
	    RSUSpeedLimit = par("RSUSpeedLimit").longValue();
	}

}
void AmrRSU11p::handleSelfMsg(cMessage* msg) {
    switch (msg->getKind()) {
        case SEND_BEACON_EVT: {
            sendSpeedMessage(RSUSpeedLimit);
            //sendWSM(prepareWSM("beacon", beaconLengthBits, type_CCH, beaconPriority, 0, -1));
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

void AmrRSU11p::sendSpeedMessage(int speed) {
    //sentMessage = true;
    t_channel channel = dataOnSch ? type_SCH : type_CCH;
    WaveShortMessage* wsm = prepareWSM("data", dataLengthBits, channel, dataPriority, -1,2);
    //WaveShortMessage* wsm = prepareWSM("beacon", beaconLengthBits, type_CCH, beaconPriority, 0, -1);

    std::ostringstream convert;   // stream used for the conversion
    convert << speed;      // insert the textual representation of 'Number' in the characters in the stream

    wsm->setWsmData(convert.str().c_str());
    sendWSM(wsm);
}

void AmrRSU11p::onBeacon(WaveShortMessage* wsm) {

}

void AmrRSU11p::onData(WaveShortMessage* wsm) {
	findHost()->getDisplayString().updateWith("r=16,green");

	annotations->scheduleErase(1, annotations->drawLine(wsm->getSenderPos(), mobi->getCurrentPosition(), "blue"));

	//if (!sentMessage) sendMessage(wsm->getWsmData());
}

void AmrRSU11p::sendWSM(WaveShortMessage* wsm) {
	sendDelayedDown(wsm,individualOffset);
}
