# Known Limitations

This document outlines the known limitations of the P2Pal application, particularly focusing on the DSDV routing implementation and NAT traversal functionality.

## DSDV Routing Limitations

### Route Convergence Time
- Route information propagates based on the rumor timer (60 seconds)
- Initial network setup requires at least one rumor cycle for full connectivity
- No adaptive timing based on network size or topology

### Node Failure Detection
- No explicit failure detection mechanism
- Failed/disconnected nodes are only detected when routing fails
- No route timeout mechanism implemented

### Routing Table Size
- No maximum limit for routing table size
- Large networks may cause performance degradation
- No pruning mechanism for unused or stale routes

### Loop Prevention
- Basic loop prevention through sequence numbers
- No explicit Time-To-Live (TTL) for route rumors
- Potential for temporary routing loops during convergence

## NAT Traversal Limitations

### NAT Types
- Works with cone NATs and some restricted NATs
- May not work with symmetric NATs where ports change for each destination
- No explicit detection of NAT type

### Rendezvous Server Dependency
- Initial connection requires a publicly accessible rendezvous server
- Single point of failure if the rendezvous server becomes unavailable
- No alternative discovery mechanism if rendezvous server is unreachable

### UDP Hole Punching
- Relies on UDP hole punching, which may not work on all networks
- Firewall configurations may block required ports
- Corporate networks with strict security policies may prevent connections

### Connection Timeout
- No explicit timeout for failed connection attempts
- May continue attempting to reach unreachable nodes

## Security Limitations

### Message Authentication
- No authentication of message sources
- Nodes can spoof their identity
- No protection against replay attacks

### Message Encryption
- No encryption for message content
- Messages can be read by intermediate nodes
- Sensitive information should not be transmitted

### Denial of Service Protection
- No rate limiting for message sending
- Vulnerable to message flooding
- No bandwidth management

## General Limitations

### Scalability
- Not designed for large-scale networks (>100 nodes)
- Performance degrades with increasing network size
- No hierarchical routing to improve scalability

### Persistent Storage
- No persistence of routing tables between application restarts
- Network must rebuild routing information after restart

### Resource Usage
- Memory usage increases linearly with routing table size
- No configurable limits for resource usage

### Platform Compatibility
- Developed and tested primarily on Windows
- May have unexpected behavior on other operating systems
- Limited configuration options for different environments

### Testing Coverage
- NAT traversal primarily tested in simulated environments
- Limited testing across different firewall configurations
- May encounter unexpected behavior in complex network topologies
