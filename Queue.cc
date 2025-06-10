#ifndef QUEUE
#define QUEUE

#include <string.h>
#include <omnetpp.h>
#include "packet_m.h"

using namespace omnetpp;

class Queue: public cSimpleModule {
private:
    int bufferSize;

    cQueue buffer;
    cMessage *endServiceEvent;
    simtime_t serviceTime;

    cOutVector bufferSizeVector;
    cOutVector packetDropVector;
    cOutVector delayQueueVector;

    void handleEndServiceEvent();
    void handleIncomingMessage(cMessage *msg);
    void sendFeedbackIfNeeded();

public:
    Queue();
    virtual ~Queue();
protected:
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Queue);

Queue::Queue() {
    endServiceEvent = nullptr;
}

Queue::~Queue() {
    cancelAndDelete(endServiceEvent);
}

void Queue::initialize() {
    buffer.setName("buffer");
    endServiceEvent = new cMessage("endServiceEvent");

    bufferSizeVector.setName("bufferSize");
    packetDropVector.setName("packetDrops");
    delayQueueVector.setName("queueDelay");

    bufferSize = par("bufferSize").intValue();
}

void Queue::handleMessage(cMessage *msg) {
    if (msg == endServiceEvent) {
        handleEndServiceEvent();
    }
    else {
        handleIncomingMessage(msg);
    }
}

void Queue::handleEndServiceEvent() {
    if (buffer.isEmpty())
        return;

    cPacket *pkt = (cPacket *)buffer.pop();

    simtime_t delay = simTime() - pkt->getArrivalTime();
    delayQueueVector.record(delay);

    send(pkt, "out");

    serviceTime = pkt->getDuration();
    scheduleAt(simTime() + serviceTime, endServiceEvent);
}

void Queue::handleIncomingMessage(cMessage *msg) {
    bufferSizeVector.record(buffer.getLength());

    if (buffer.getLength() >= bufferSize) {
        packetDropVector.record(1);
        bubble("packet dropped");
        delete msg;
        return;
    }

    sendFeedbackIfNeeded();

    buffer.insert(msg);
    bufferSizeVector.record(buffer.getLength());

    if (!endServiceEvent->isScheduled()) {
        scheduleAt(simTime(), endServiceEvent);
    }
}

void Queue::sendFeedbackIfNeeded() {
    // Umbral del 80% de bufferSize
    int umbral = bufferSize * 0.8;
    if (buffer.getLength() >= umbral) {
        FeedbackPkt *feedbackPkt = new FeedbackPkt();
        feedbackPkt->setKind(2);
        feedbackPkt->setByteLength(20);
        feedbackPkt->setSlowDown_receiver(true);
        buffer.insertBefore(buffer.front(), feedbackPkt);
    }
}

void Queue::finish() {
}

#endif /* QUEUE */
