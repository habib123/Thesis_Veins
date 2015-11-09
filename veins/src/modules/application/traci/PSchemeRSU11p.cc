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

#include "application/traci/PSchemeRSU11p.h"

using Veins::AnnotationManagerAccess;

Define_Module(PSchemeRSU11p);

void PSchemeRSU11p::initialize(int stage) {
	BaseWaveApplLayer::initialize(stage);
	if (stage == 0) {
	    mobi = dynamic_cast<BaseMobility*> (getParentModule()->getSubmodule("mobility"));
	    ASSERT(mobi);
	    annotations = AnnotationManagerAccess().getIfExists();
	    ASSERT(annotations);
	}
}

void PSchemeRSU11p::onBeacon(WaveShortMessage* wsm) {
    findHost()->getDisplayString().updateWith("r=16,green");

    simtime_t t =  wsm ->getTimestamp();
    Coord cr = wsm ->getSenderPos();

    ofstream outfile;
    outfile.open("beaconRecord.txt",ios::app);
    outfile <<" ID "<<wsm->getCarId()<<" SenderAdd " << wsm->getSenderAddress()<<" Speed "<< wsm->getSpeed()<<" vecX "<<wsm->getvecX()<<" vecY "<< wsm->getvecY()<<" AngleRad "<< wsm->getAngleRad()<< " ArrivalTime " << wsm->getArrivalTime() <<" Position "<<"("<<cr.x<<","<<cr.y<<","<<cr.z<<")"<< endl;
    outfile.close();

}

void PSchemeRSU11p::onData(WaveShortMessage* wsm) {

}

void PSchemeRSU11p::sendWSM(WaveShortMessage* wsm) {
	sendDelayedDown(wsm,individualOffset);
}
