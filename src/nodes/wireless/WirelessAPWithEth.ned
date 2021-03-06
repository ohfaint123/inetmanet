//
// Copyright (C) 2006 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//




package inet.nodes.wireless;

import inet.base.NotificationBoard;
import inet.mobility.NullMobility;
import inet.linklayer.ethernet.EtherMAC;
import inet.linklayer.etherswitch.MACRelayUnit;
import inet.linklayer.ieee80211.Ieee80211NicAP;


//
// Models a 802.11 Access Point with Ethernet ports.
// It can be used in conjunction with WirelessHost, or any other host model
// containing WirelessNicSTA.
//
// @see WirelessAP, WirelessAPSimplified, WirelessAPWithEth, WirelessAPWithEthSimplified
// @see WirelessHost, WirelessHostSimplified
// @see MobileHost, MFMobileHost
//
module WirelessAPWithEth
{
    parameters:
        @node();
        string relayUnitType = default("MACRelayUnitNP"); // type of the MACRelayUnit (MACRelayUnitNP, MACRelayUnitPP, etc)
    gates:
        input radioIn @directIn;
        inout ethg[];
    submodules:
        relayUnit: <relayUnitType> like MACRelayUnit {
            parameters:
                @display("p=169,124");
            gates:
                lowerLayerIn[sizeof(ethg)+1];
                lowerLayerOut[sizeof(ethg)+1];
        }
        wlan: Ieee80211NicAP { // see also Ieee80211NicAPSimplified
            parameters:
                @display("p=89,225;q=queue");
        }
        eth[sizeof(ethg)]: EtherMAC {
            parameters:
                promiscuous = true;
                txQueueLimit = 1000; // increase if needed
                queueModule = "";
                @display("p=174,225,row;q=queue");
        }
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=78,60");
        }
        mobility: NullMobility {
            parameters:
                @display("p=71,121");
        }
    connections allowunconnected:
        radioIn --> { @display("m=s"); } --> wlan.radioIn;
        wlan.uppergateIn <-- relayUnit.lowerLayerOut[0];
        wlan.uppergateOut --> relayUnit.lowerLayerIn[0];
        for i=0..sizeof(ethg)-1 {
            eth[i].upperLayerIn <-- relayUnit.lowerLayerOut[i+1];
            eth[i].upperLayerOut --> relayUnit.lowerLayerIn[i+1];
            eth[i].phys <--> ethg[i];
        }
}

