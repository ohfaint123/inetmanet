//
// Copyright (C) 2004 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#ifndef __IP_H__
#define __IP_H__

#include "RoutingTableAccess.h"
#include "RoutingTable.h"
#include "ICMPAccess.h"
#include "IPControlInfo_m.h"
#include "IPDatagram.h"
#include "IPFragBuf.h"
#include "ProtocolMap.h"


// ICMP type 2, code 4: fragmentation needed, but don't-fragment bit set
const int ICMP_FRAGMENTATION_ERROR_CODE = 4;


/**
 * Implements the IP protocol.
 */
class IP : public cSimpleModule
{
  protected:
    RoutingTableAccess routingTableAccess;
    ICMPAccess icmpAccess;

    // config
    bool IPForward;
    int defaultTimeToLive;
    int defaultMCTimeToLive;
    simtime_t fragmentTimeoutTime;

    // working vars
    long curFragmentId; // counter, used to assign unique fragmentIds to datagrams
    IPFragBuf fragbuf;  // fragmentation reassembly buffer
    simtime_t lastCheckTime; // when fragbuf was last checked for state fragments
    ProtocolMapping mapping; // where to send packets after decapsulation

    // statistics
    int numMulticast;
    int numLocalDeliver;
    int numDropped;
    int numUnroutable;
    int numForwarded;

  protected:
    IPDatagram *encapsulate(cMessage *transportPacket);

    /**
     * Handle IPDatagram messages arriving from lower layer.
     * Decrements TTL, then invokes routePacket().
     */
    virtual void handlePacketFromNetwork(IPDatagram *dgram);

    /**
     * Handle messages (typically packets to be send in IP) from transport or ICMP.
     * Invokes encapsulate(), then routePacket().
     */
    virtual void handleMessageFromHL(cMessage *msg);

    /**
     * Performs routing. Based on the routing decision, it dispatches to
     * localDeliver() for local packets, to fragmentAndSend() for forwarded packets,
     * to handleMulticastPacket() for multicast packets, or drops the packet if
     * it's unroutable or forwarding is off.
     */
    virtual void routePacket(IPDatagram *datagram);

    /**
     * Forwards packets to all multicast destinations, using fragmentAndSend().
     */
    virtual void handleMulticastPacket(IPDatagram *dgram);

    /**
     * Perform reassembly of fragmented datagrams, then send them up to the
     * higher layers using sendToHL().
     */
    virtual void localDeliver(IPDatagram *dgram);

    /**
     * Fragment packet if needed, then send it to the selected interface using
     * sendDatagramToOutput().
     */
    virtual void fragmentAndSend(IPDatagram *dgram);

    /**
     * Last TTL check, then send datagram on the given interface.
     */
    virtual void sendDatagramToOutput(IPDatagram *datagram, int outputPort);

  public:
    Module_Class_Members(IP, cSimpleModule, 0);

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
