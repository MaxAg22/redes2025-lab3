#ifndef TRANSPORT_RX
#define TRANSPORT_RX

#include <string.h>
#include <omnetpp.h>
#include "packet_m.h"

using namespace omnetpp;

class TransportRx: public cSimpleModule {
private:
    int bufferSize;
    cQueue buffer;
    cQueue feedbackBuffer;
    cMessage *endServiceEvent;
    cMessage *feedbackEvent;
    simtime_t serviceTime;

    // ANALYTICS 
    cOutVector bufferSizeVector;
    cOutVector packetDropVector;
    cOutVector delayQueueVector;
    cOutVector timeOfLife;


    cOutVector packetPros;
    int packetProsCounter;

public:
    TransportRx();
    virtual ~TransportRx();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    virtual void enqueueFeedback(cMessage *msg);
    virtual void enqueueMessage(cMessage *msg);
};

Define_Module(TransportRx);

TransportRx::TransportRx() {
    endServiceEvent = NULL;
    feedbackEvent = NULL;

}

TransportRx::~TransportRx() {
    cancelAndDelete(endServiceEvent);
    cancelAndDelete(feedbackEvent);
}

void TransportRx::initialize() {
    buffer.setName("buffer");
    feedbackBuffer.setName("feedbackBuffer");
    endServiceEvent = new cMessage("endService");
    feedbackEvent = new cMessage("feedbackEndService");

    // ANALYTICS 
    bufferSizeVector.setName("bufferSizeVector");
    delayQueueVector.setName("DelayQueueVector");
    packetDropVector.setName("packetDropVector");

    packetPros.setName("packetPros");
    packetProsCounter = 0;

    timeOfLife.setName("timeOfLife");
}

void TransportRx::finish() {
}

void TransportRx::enqueueFeedback(cMessage *msg) {
    feedbackBuffer.insert(msg);
    bufferSizeVector.record(buffer.getLength());
            
    // if the server is idle
    if (!feedbackEvent->isScheduled()) {
        // start the service
        scheduleAt(simTime() + 0, feedbackEvent);
    }
}

void TransportRx::enqueueMessage(cMessage *msg) {
    const int bufferSizeMax = par("bufferSize").intValue();
    const int umbral = bufferSizeMax * 0.8;

    if (buffer.getLength() >= bufferSizeMax) {
        // drop the packet
        delete msg;
        this->bubble("packet dropped");
        packetDropVector.record(1);
    } else {

        if(buffer.getLength() >= umbral) {
            FeedbackPkt* feedbackPkt = new FeedbackPkt();
            feedbackPkt->setKind(2);
            feedbackPkt->setByteLength(20);
            feedbackPkt->setSlowDown_receiver(true);
            enqueueFeedback(feedbackPkt);
        }

        // if msg is a data packet, enqueue the packet
        buffer.insert(msg);
        bufferSizeVector.record(buffer.getLength());

        // if the server is idle
        if (!endServiceEvent->isScheduled()) {
            // start the service
            scheduleAt(simTime() + 0, endServiceEvent);
       }
    }
}

void TransportRx::handleMessage(cMessage *msg) {
    // Record stats
    bufferSizeVector.record(buffer.getLength());

    if (msg->getKind() == 2) {
        enqueueFeedback(msg);
    } else {
        if (msg == endServiceEvent) {
            // If msg is signaling an endServiceEvent
            if (!buffer.isEmpty()) {
                // dequeue packet
                cPacket *pkt = (cPacket*) buffer.pop();

                // Record process time in queue
                simtime_t delay = simTime() - pkt->getArrivalTime();
                delayQueueVector.record(delay);

                // send packet
                send(pkt, "toApp");

                // ANALYTICS
                packetProsCounter++;
                packetPros.record(packetProsCounter);

                timeOfLife.record(simTime() - pkt->getTimestamp());

                // start new service
                serviceTime = pkt->getDuration();
                scheduleAt(simTime() + serviceTime, endServiceEvent);
           }
        } else if (msg == feedbackEvent) {
            // If msg is signaling an feedbackEvent
            if (!feedbackBuffer.isEmpty()) {
                // dequeue packet
                FeedbackPkt *feedbackPkt = (FeedbackPkt*) feedbackBuffer.pop();

                // send feedback
                send(feedbackPkt, "toOut$o");

                // start new service
                serviceTime = feedbackPkt->getDuration();
                scheduleAt(simTime() + serviceTime, feedbackEvent);
           }
        } else {
            // If msg is a incoming massage
            enqueueMessage(msg);
        }
    }

}

#endif /* TRANSPORT_RX */
