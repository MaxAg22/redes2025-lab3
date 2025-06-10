#ifndef TRANSPORT_TX
#define TRANSPORT_TX

#include <string.h>
#include <omnetpp.h>
#include "packet_m.h"

using namespace omnetpp;

const double TIME_OUT = 0.5;
const double MIN_SCALAR = 0.1;

class TransportTx : public cSimpleModule {
private:
    double scalar;
    double lastFeedbackpkt;
    int bufferSize;
    cQueue buffer;
    cMessage *endServiceEvent;
    simtime_t serviceTime;

    // ANALYTICS 
    cOutVector bufferSizeVector;
    cOutVector packetDropVector;
    cOutVector delayQueueVector;

    cOutVector scalarVector;

    cOutVector paquetesEnviados;
    int contadorPaquetes;

    cOutVector paquetesPorSegundo;

public:
    TransportTx();
    virtual ~TransportTx();

protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    virtual void handleCongestion();
    virtual void handleTransferRate();
};

Define_Module(TransportTx);

TransportTx::TransportTx() {
    endServiceEvent = NULL;
}

TransportTx::~TransportTx() {
    cancelAndDelete(endServiceEvent);
}

void TransportTx::initialize() {
    double scalar = 0.0;
    double lastFeedbackpkt = 0;

    buffer.setName("buffer");
    endServiceEvent = new cMessage("endService");

    // ANALYTICS 
    bufferSizeVector.setName("bufferSizeVector");
    delayQueueVector.setName("DelayQueueVector");
    packetDropVector.setName("packetDropVector");

    scalarVector.setName("scalarVector");

    paquetesEnviados.setName("paquetesEnviados");
    contadorPaquetes = 0;

    paquetesPorSegundo.setName("paquetesPorSegundo");

}

void TransportTx::finish() {
}

void TransportTx::handleCongestion() {
    scalar = scalar + 0.1;
    scalarVector.record(scalar);
}

void TransportTx::handleTransferRate() {
    if (simTime().dbl() - lastFeedbackpkt >= TIME_OUT && scalar > MIN_SCALAR) {
        scalar = scalar - 0.1;
        scalarVector.record(scalar);
    }
}

void TransportTx::handleMessage(cMessage *msg) {
    if (msg->getKind() == 2) {
        FeedbackPkt *feedbackPkt = (FeedbackPkt *)msg;

        // Record last Feedbackpkt
        lastFeedbackpkt = simTime().dbl();

        if (feedbackPkt->getSlowDown_receiver() || feedbackPkt->getSlowDown_red()) {
            handleCongestion();
        }

        delete(msg);
    } 
    else if (msg == endServiceEvent) {
        // If msg is signaling an endServiceEvent
        if (!buffer.isEmpty()) {
            // Dequeue packet
            cPacket *pkt = (cPacket *)buffer.pop();

            // Record process time in queue
            simtime_t delay = simTime() - pkt->getArrivalTime();
            delayQueueVector.record(delay);

            // Record creation time in pkt
            pkt->setTimestamp(simTime());

            // Handle transfer rate
            handleTransferRate();

            // Send packet
            send(pkt, "toOut$o");
            contadorPaquetes ++;
            paquetesEnviados.record(contadorPaquetes);





            // Start new service
            serviceTime = pkt->getDuration();
            scheduleAt(simTime() + serviceTime + (serviceTime * scalar), endServiceEvent);
        }
    } 
    else if ((int)buffer.getLength() >= par("bufferSize").intValue()) {
        // Drop the packet
        delete msg;
        this->bubble("packet dropped");
        packetDropVector.record(1);
    } 
    else {
        // If msg is a data packet, enqueue the packet
        buffer.insert(msg);
        bufferSizeVector.record(buffer.getLength());

        // If the server is idle
        if (!endServiceEvent->isScheduled()) {
            // Start the service
            scheduleAt(simTime() + 0, endServiceEvent);
        }
    }
}

#endif /* TRANSPORT_TX */
