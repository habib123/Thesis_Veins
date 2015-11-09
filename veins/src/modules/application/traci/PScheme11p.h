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

#ifndef Test11p_H
#define Test11p_H

#include "BaseWaveApplLayer.h"
#include "modules/mobility/traci/TraCIMobility.h"
#include <iostream>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include<functional>
#include <thread>
using namespace std;

using Veins::TraCIMobility;
using Veins::AnnotationManager;
static int senderAddress;






/**
 * Small IVC Demo using 11p
 */
class PScheme11p : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
        virtual void receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj);
    protected:
        double offSet;
        TraCIMobility* traci;
        AnnotationManager* annotations;
        simtime_t lastDroveAt;
        simtime_t addressChangeTime;

        int address;
        bool sentMessage;
        bool isParking;
        bool sendWhileParking;
        static const simsignalwrap_t parkingStateChangedSignal;
    protected:
        //std::function<void(void)> func;
        //std::function<void(void)> func =std::bind(&PScheme11p::do_something, this);
        //std::function<void(void)> f = std::bind(&Foo::doSomething, this);
        virtual void timer_start(unsigned int interval,PScheme11p* p);
        virtual void do_something();
        virtual void myThread(int arg);
        virtual void onBeacon(WaveShortMessage* wsm);
        virtual void onData(WaveShortMessage* wsm);
       // void sendMessage(std::string blockedRoadId);
        virtual void handlePositionUpdate(cObject* obj);
       // virtual void handleParkingUpdate(cObject* obj);
        virtual void sendWSM(WaveShortMessage* wsm);
        virtual void handleSelfMsg(cMessage* msg);
        virtual void changeId(std::string carId);
        virtual int getActualPositionId(string carId);
        virtual int getActualCarId(string carId);
       // void handleMessage(cMessage *msg);
        virtual WaveShortMessage* prepareWSM(std::string name, int lengthBits, t_channel channel, int priority, int rcvId, int serial);
};

//int PScheme11p::senderAddress =  0;

#endif
