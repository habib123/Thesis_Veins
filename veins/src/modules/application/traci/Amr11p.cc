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

#include "application/traci/Amr11p.h"
#include <stdlib.h>

using Veins::TraCIMobilityAccess;
using Veins::AnnotationManagerAccess;

const simsignalwrap_t Amr11p::parkingStateChangedSignal = simsignalwrap_t(TRACI_SIGNAL_PARKING_CHANGE_NAME);

Define_Module(Amr11p);

void Amr11p::initialize(int stage) {
	BaseWaveApplLayer::initialize(stage);
	if (stage == 0) {
		traci = TraCIMobilityAccess().get(getParentModule());
		annotations = AnnotationManagerAccess().getIfExists();
		ASSERT(annotations);

//		sentMessage = false;
//		lastDroveAt = simTime();
//		findHost()->subscribe(parkingStateChangedSignal, this);
//		isParking = false;
//		sendWhileParking = par("sendWhileParking").boolValue();
	}
}

void Amr11p::onBeacon(WaveShortMessage* wsm) {

}

void Amr11p::onData(WaveShortMessage* wsm) {
	findHost()->getDisplayString().updateWith("r=16,green");
	annotations->scheduleErase(1, annotations->drawLine(wsm->getSenderPos(), traci->getPositionAt(simTime()), "blue"));

	//if (traci->getRoadId()[0] != ':') traci->commandChangeRoute(wsm->getWsmData(), 9999);
	//if (!sentMessage) sendMessage(wsm->getWsmData());
	 //if (strcmp(wsm->getName(), "slimit") == 0)
	int speed = atoi(wsm->getWsmData());
	 traci->commandSetSpeed(speed);
}

void Amr11p::sendMessage(std::string blockedRoadId) {
//	sentMessage = true;
//
//	t_channel channel = dataOnSch ? type_SCH : type_CCH;
//	WaveShortMessage* wsm = prepareWSM("data", dataLengthBits, channel, dataPriority, -1,2);
//	wsm->setWsmData(blockedRoadId.c_str());
//	sendWSM(wsm);
}
void Amr11p::receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj) {
	Enter_Method_Silent();
	if (signalID == mobilityStateChangedSignal) {
		handlePositionUpdate(obj);
	}
	else if (signalID == parkingStateChangedSignal) {
		handleParkingUpdate(obj);
	}
}
void Amr11p::handleParkingUpdate(cObject* obj) {
	isParking = traci->getParkingState();
	if (sendWhileParking == false) {
		if (isParking == true) {
			(FindModule<BaseConnectionManager*>::findGlobalModule())->unregisterNic(this->getParentModule()->getSubmodule("nic"));
		}
		else {
			Coord pos = traci->getCurrentPosition();
			(FindModule<BaseConnectionManager*>::findGlobalModule())->registerNic(this->getParentModule()->getSubmodule("nic"), (ChannelAccess*) this->getParentModule()->getSubmodule("nic")->getSubmodule("phy80211p"), &pos);
		}
	}
}
void Amr11p::handlePositionUpdate(cObject* obj) {
	BaseWaveApplLayer::handlePositionUpdate(obj);

	// stopped for for at least 10s?
	if (traci->getSpeed() < 1) {
		if (simTime() - lastDroveAt >= 10) {
			findHost()->getDisplayString().updateWith("r=16,red");
			if (!sentMessage) sendMessage(traci->getRoadId());
		}
	}
	else {
		lastDroveAt = simTime();
	}
}
void Amr11p::sendWSM(WaveShortMessage* wsm) {
	if (isParking && !sendWhileParking) return;
	sendDelayedDown(wsm,individualOffset);
}
