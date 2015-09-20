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

#include "application/traci/TestRSU11p.h"

using Veins::AnnotationManagerAccess;

Define_Module(TestRSU11p);

void TestRSU11p::initialize(int stage) {
	BaseWaveApplLayer::initialize(stage);
	if (stage == 0) {
	    mobi = dynamic_cast<BaseMobility*> (getParentModule()->getSubmodule("mobility"));
	    ASSERT(mobi);
	    annotations = AnnotationManagerAccess().getIfExists();
	    ASSERT(annotations);
	    RSUSpeedLimit = par("RSUSpeedLimit").longValue();
	}

}

void TestRSU11p::onBeacon(WaveShortMessage* wsm) {
    findHost()->getDisplayString().updateWith("r=16,green");
    int ok = wsm ->getSenderAddress();
    simtime_t t =  wsm ->getTimestamp();
    Coord cr = wsm ->getSenderPos();
    double x = cr.x;
    EV<<ok;
    EV << "Received WSM from" << wsm->getSenderAddress() << " at " << wsm->getArrivalTime() <<"at position"<< wsm ->getSenderPos() << endl;
    EV << "Received message `" << wsm->getPsid() << "', sending it out again\n";

    ofstream outfile;
    outfile.open("afile.txt",ios::app);
    outfile <<"Received WSM from" << wsm->getSenderAddress() << " at " << wsm->getArrivalTime() <<"at position"<<"("<<cr.x<<","<<cr.y<<","<<cr.z<<")"<< endl;
    outfile.close();
}

void TestRSU11p::onData(WaveShortMessage* wsm) {

}

void TestRSU11p::sendWSM(WaveShortMessage* wsm) {
	sendDelayedDown(wsm,individualOffset);
}
