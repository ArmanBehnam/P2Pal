#include "gossip.h"
#include <QDateTime>
#include <QDebug>

Gossip::Gossip(QObject *parent) : QObject(parent), currentSeqNo(0)
{
    // Generate a random node ID if none is set
    if (myNodeId.isEmpty()) {
        myNodeId = QString("Node%1").arg(QRandomGenerator::global()->bounded(1000, 9999));
    }

    // Set up route rumor timer (60 seconds = 60000 ms)
    routeRumorTimer = new QTimer(this);
    connect(routeRumorTimer, &QTimer::timeout, this, &Gossip::sendRouteRumor);
    routeRumorTimer->start(60000);  // Send route rumors every 60 seconds
    
    // Send initial route rumor
    QTimer::singleShot(1000, this, &Gossip::sendRouteRumor);
}

Gossip::~Gossip()
{
    if (routeRumorTimer) {
        routeRumorTimer->stop();
        delete routeRumorTimer;
    }
}

void Gossip::setNodeId(const QString& nodeId)
{
    myNodeId = nodeId;
}

QString Gossip::getNodeId() const
{
    return myNodeId;
}

void Gossip::addNeighbor(const QString& ipAddress, quint16 port)
{
    neighbors.insert(qMakePair(ipAddress, port));
    
    // Send our route rumor to the new neighbor
    QTimer::singleShot(500, this, &Gossip::sendRouteRumor);
}

void Gossip::removeNeighbor(const QString& ipAddress, quint16 port)
{
    neighbors.remove(qMakePair(ipAddress, port));
}

void Gossip::handleIncomingMessage(const QVariantMap& message, const QString& senderIP, quint16 senderPort)
{
    // Add NAT traversal information
    QVariantMap enhancedMsg = message;
    enhancedMsg["LastIP"] = senderIP;
    enhancedMsg["LastPort"] = senderPort;
    
    // Process the enhanced message
    processMessage(enhancedMsg, senderIP, senderPort);
}

void Gossip::processMessage(const QVariantMap& message, const QString& senderIP, quint16 senderPort)
{
    // Check if it's a private message
    if (message.contains("Dest")) {
        forwardPrivateMessage(message);
        return;
    }
    
    // Process as a rumor message
    QString origin = message["Origin"].toString();
    quint32 seqNo = message["SeqNo"].toUInt();
    
    // Update routing information
    updateRoutingTable(origin, senderIP, senderPort, seqNo);
    
    // Check if we've seen this message before
    if (receivedMessages.contains(origin) && receivedMessages[origin].contains(seqNo)) {
        return; // Already processed this message
    }
    
    // Mark this message as received
    if (!receivedMessages.contains(origin)) {
        receivedMessages[origin] = QSet<quint32>();
    }
    receivedMessages[origin].insert(seqNo);
    
    // Check if this is a chat message or just a route rumor
    if (message.contains("ChatText")) {
        // This is a chat message - emit signal to display in GUI
        emit messageReceived(origin, message["ChatText"].toString());
    }
    
    // Add NAT traversal information
    // If message contains LastIP and LastPort, store this information
    if (message.contains("LastIP") && message.contains("LastPort")) {
        QString lastIP = message["LastIP"].toString();
        quint16 lastPort = message["LastPort"].toUInt();
        
        // Store this as a potential direct route
        if (!origin.isEmpty() && origin != myNodeId) {
            updateRoutingTable(origin, lastIP, lastPort, seqNo, true);  // Mark as direct
        }
    }
    
    // Forward the message to propagate it (DSDV style)
    processRouteRumor(message, senderIP, senderPort);
}

void Gossip::processRouteRumor(const QVariantMap& message, const QString& senderIP, quint16 senderPort)
{
    // Forward the rumor to a random neighbor (except the sender)
    QList<QPair<QString, quint16>> eligibleNeighbors;
    for (const auto& neighbor : neighbors) {
        if (neighbor.first != senderIP || neighbor.second != senderPort) {
            eligibleNeighbors.append(neighbor);
        }
    }
    
    if (!eligibleNeighbors.isEmpty()) {
        int randomIndex = QRandomGenerator::global()->bounded(eligibleNeighbors.size());
        const auto& randomNeighbor = eligibleNeighbors.at(randomIndex);
        emit sendMessage(randomNeighbor.first, randomNeighbor.second, message);
    }
}

bool Gossip::isBetterRoute(const RouteEntry& oldRoute, const RouteEntry& newRoute)
{
    return (newRoute.seqNo > oldRoute.seqNo) || 
           (newRoute.seqNo == oldRoute.seqNo && newRoute.isDirect && !oldRoute.isDirect);
}

void Gossip::updateRoutingTable(const QString& origin, const QString& senderIP, quint16 senderPort, quint32 seqNo, bool isDirect)
{
    // Skip if this is our own ID
    if (origin == myNodeId) {
        return;
    }
    
    RouteEntry newRoute(senderIP, senderPort, seqNo, isDirect);
    
    // Check if we should update the route
    if (!routingTable.contains(origin) || isBetterRoute(routingTable[origin], newRoute)) {
        routingTable[origin] = newRoute;
        
        // Emit signal to update GUI with available nodes
        QStringList nodes;
        for (auto it = routingTable.constBegin(); it != routingTable.constEnd(); ++it) {
            if (it.key() != myNodeId) {  // Don't include ourselves
                nodes.append(it.key());
            }
        }
        emit availableNodesChanged(nodes);
    }
}

void Gossip::sendRouteRumor()
{
    QVariantMap routeRumor;
    routeRumor["Origin"] = myNodeId;
    routeRumor["SeqNo"] = ++currentSeqNo;
    
    // Add this to our own received messages to avoid loops
    if (!receivedMessages.contains(myNodeId)) {
        receivedMessages[myNodeId] = QSet<quint32>();
    }
    receivedMessages[myNodeId].insert(currentSeqNo);
    
    // Send to all neighbors
    for (const auto& neighbor : neighbors) {
        emit sendMessage(neighbor.first, neighbor.second, routeRumor);
    }
}

void Gossip::sendChatMessage(const QString& text)
{
    QVariantMap message;
    message["Origin"] = myNodeId;
    message["SeqNo"] = ++currentSeqNo;
    message["ChatText"] = text;
    message["Timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    // Add this to our own received messages to avoid loops
    if (!receivedMessages.contains(myNodeId)) {
        receivedMessages[myNodeId] = QSet<quint32>();
    }
    receivedMessages[myNodeId].insert(currentSeqNo);
    
    // Send to all neighbors
    for (const auto& neighbor : neighbors) {
        emit sendMessage(neighbor.first, neighbor.second, message);
    }
    
    // Also display our own message in the GUI
    emit messageReceived(myNodeId, text);
}

void Gossip::sendPrivateMessage(const QVariantMap& message)
{
    QString destination = message["Dest"].toString();
    
    // Check if we have a route to the destination
    if (routingTable.contains(destination)) {
        const RouteEntry& route = routingTable[destination];
        emit sendMessage(route.ipAddress, route.port, message);
    } else {
        qDebug() << "No route to destination:" << destination;
    }
}

void Gossip::forwardPrivateMessage(const QVariantMap& message)
{
    QString destination = message["Dest"].toString();
    
    // If we are the destination, display the message
    if (destination == myNodeId) {
        emit messageReceived(message["Origin"].toString(), message["ChatText"].toString());
        return;
    }
    
    // Check if we have a route to the destination
    if (routingTable.contains(destination)) {
        // Decrement hop limit
        QVariantMap forwardMsg = message;
        quint32 hopLimit = forwardMsg["HopLimit"].toUInt();
        
        if (hopLimit > 0) {
            forwardMsg["HopLimit"] = hopLimit - 1;
            
            const RouteEntry& route = routingTable[destination];
            emit sendMessage(route.ipAddress, route.port, forwardMsg);
        }
    }
}