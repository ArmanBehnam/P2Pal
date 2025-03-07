# Programming Assignment 1

## P2Pal - Peer-to-Peer Chat Application

P2Pal is a simple peer-to-peer chat application that implements a gossip-based protocol for reliable message delivery, using both rumor mongering and anti-entropy mechanisms.

## Features

- UDP-based messaging with QUdpSocket
- Message serialization/deserialization using QVariantMap and JSON
- Gossip protocol with rumor mongering and anti-entropy
- Auto peer discovery on local ports
- Support for manual peer addition
- Unique node identification
- Vector clocks for message synchronization
- Sequence numbering for ordered message delivery
- Simple Qt6-based GUI with chat log and text input


## Project Structure

```bash
P2Pal/
├── src/
│   ├── main.cpp         // Entry point, GUI setup
│   ├── p2pal.h          // P2Pal class declaration
│   ├── p2pal.cpp        // P2Pal class implementation
│   ├── network.h        // Network communication logic
│   ├── network.cpp
│   ├── gossip.h         // Gossip protocol logic
│   ├── gossip.cpp
│   ├── message.h        // Message structure and serialization
│   ├── message.cpp
│   ├── peer.h           // Peer management
│   ├── peer.cpp
│   ├── gui.h            // GUI related code
│   └── gui.cpp
├── tests/
│   ├── test_message.cpp // Unit tests for message handling
│   ├── test_gossip.cpp  // Unit tests for gossip protocol
│   ├── test_network.cpp // Unit tests for network functionality
│   └── test_gui.cpp     // Unit test for gui functionality
├── scripts/
│   ├── build.sh         // Build script
│   ├── run_instances.sh // Script to run multiple instances
│   └── test.sh          // Test script
├── docs/
│   └── README.md        // Project overview
├── CMakeLists.txt       // CMake build configuration
└── P2Pal.pro            // Qt project file
```

## Building

### Prerequisites

- Qt 6.2.0 or later
- C++17 compatible compiler (GCC, Clang, MSVC)
- CMake 3.16 or later (if using CMake build system)

### Building with CMake

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build . --config Release
```

## Running

### Single instance

```bash
# From the build directory
./P2Pal
```

specify a port to use:

```bash
./P2Pal --port 5000
```

### Multiple instances for testing

To test communication between multiple instances:

```bash
# Start multiple instances on different ports
./P2Pal --port 5000 &
./P2Pal --port 5001 &
./P2Pal --port 5002 &
./P2Pal --port 5003 &
```

## Usage

1. The application automatically discovers other P2Pal instances running on standard ports on localhost (5000-5009)
2. You can manually add peers by providing their IP address and port
3. Type messages in the text input area and press Enter to send
4. Messages will be propagated to all connected peers through the gossip protocol

## Protocol Details

### Message Types

#### Rumor Message
```
{
    "Type": "Rumor",
    "Origin": "<unique-node-id>",
    "SequenceNumber": <sequence-number>,
    "ChatText": "<message-text>"
}
```

#### Status Message
```
{
    "Type": "Status",
    "Want": {
        "<origin1>": <max-sequence-number>,
        "<origin2>": <max-sequence-number>,
        ...
    }
}
```

### Gossip Protocol Implementation

1. **Rumor Mongering**
   - When a node creates or receives a new message, it propagates it to a randomly selected peer
   - Messages are retransmitted periodically until acknowledgment is received
   - Each message is uniquely identified by its origin and sequence number

2. **Anti-Entropy**
   - Nodes periodically share their status (vector clock) with random peers
   - When a node receives a status message, it compares with its own state
   - If the node has messages the peer doesn't have, it sends them
   - If the peer has messages the node doesn't have, it requests them

### Vector Clocks

Each node maintains a mapping of:
- Origin (node ID) → Highest sequence number seen

This allows efficient synchronization between peers.

## Testing

To verify correct implementation:

1. Launch multiple instances using different ports
2. Send messages from different instances
3. Observe message propagation across all instances
4. Check sequence numbers to verify ordered delivery
5. Kill and restart instances to test resilience

## Troubleshooting

- If instances can't connect, ensure no firewall is blocking UDP traffic
- Check that the selected ports are available and not in use
- Messages may take some time to propagate through the network, especially with many hops

