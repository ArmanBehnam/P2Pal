# P2Pal - Peer-to-Peer Communication Application

P2Pal is a decentralized peer-to-peer communication application that implements Destination-Sequenced Distance-Vector (DSDV) routing and NAT traversal capabilities, enabling reliable communication across network boundaries.

## Features

- **Decentralized Communication**: Direct peer-to-peer messaging without central servers
- **DSDV Routing**: Efficient point-to-point message delivery using distance-vector routing
- **NAT Traversal**: Ability to connect peers behind different NAT configurations
- **Private Messaging**: Secure direct messaging between specific nodes
- **Route Rumors**: Automatic route discovery and maintenance
- **Rendezvous Mode**: Server mode to facilitate NAT hole-punching

## Installation

Clone the repository and build using CMake or Qt's build system:

```bash
git clone https://github.com/ArmanBehnam/P2Pal.git
cd P2Pal
mkdir build && cd build
cmake ..
make
```

## Usage

### Basic Usage

Start P2Pal with a specific port:
```
P2Pal -port 12345
```

Connect to another P2Pal instance:
```
P2Pal -port 23456 -connect 127.0.0.1:12345
```

### Rendezvous Server Mode
To create a rendezvous server that helps with NAT traversal:
```
P2Pal -port 45678 -noforward
```

### Command Line Options

- `-port <number>`: Set the listening port (default: random available port)
- `-connect <address>:<port>`: Connect to specified P2Pal instance
- `-noforward`: Enable rendezvous server mode (only forward route information, not chat messages)

## Architecture

P2Pal consists of several key components:

- **Network Layer**: UDP-based communication with NAT traversal capabilities
- **Routing Layer**: DSDV routing implementation for message forwarding
- **Message Protocol**: JSON-based protocol for chat and routing messages
- **GUI**: User interface for messaging and node discovery

### DSDV Routing Implementation

P2Pal implements the Destination-Sequenced Distance-Vector (DSDV) routing protocol which:

1. Maintains a routing table with next-hop information for each destination
2. Uses sequence numbers to prevent routing loops and ensure freshness
3. Propagates route information through route rumors
4. Supports point-to-point message delivery with hop limits

### NAT Traversal

NAT traversal is achieved through:

1. A rendezvous server that helps peers discover each other
2. UDP hole punching techniques to establish direct connections
3. Exchange of public endpoint information via route rumors
4. Direct route preference when multiple paths are available

## Testing

See the following documents for testing procedures:
- [Network Namespace Setup Guide](docs/network-namespace-setup.md)
- [Testing Procedure Explanation](docs/testing-procedure.md)
- [Known Limitations](docs/known-limitations.md)
