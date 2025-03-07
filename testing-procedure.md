# P2Pal Testing Procedure

This document outlines the procedures for testing the P2Pal application, focusing on the DSDV routing and NAT traversal functionality.

## Basic Functionality Testing

### 1. Local Network Testing

1. Launch three P2Pal instances with different port numbers:
   ```
   P2Pal.exe -port 12345
   P2Pal.exe -port 23456 -connect 127.0.0.1:12345
   P2Pal.exe -port 34567 -connect 127.0.0.1:23456
   ```

2. Verify these behaviors:
   - All nodes appear in each instance's node list within 60 seconds
   - Broadcast messages reach all connected nodes
   - Route rumors are propagated correctly (check available nodes list)
   - Node identifiers display correctly as IP:Port combinations

3. Test message routing:
   - Send a message from node 1 to node 3 (should route through node 2)
   - Verify hop count decrementation works correctly

### 2. DSDV Routing Verification

1. Start three nodes in a chain configuration:
   ```
   P2Pal.exe -port 12345                          # Node A
   P2Pal.exe -port 23456 -connect 127.0.0.1:12345 # Node B
   P2Pal.exe -port 34567 -connect 127.0.0.1:23456 # Node C
   ```

2. Wait for route propagation (60 seconds)

3. Verify that Node A can see Node C and vice versa in the node list

4. Disconnect Node B:
   - Close the Node B application
   - Verify that after a timeout period, Node A and Node C remove Node B from their node lists
   - Verify that Node A and Node C can no longer communicate

## NAT Traversal Testing

### 1. Rendezvous Server Setup

1. Start a rendezvous server with the `-noforward` flag:
   ```
   P2Pal.exe -port 45678 -noforward
   ```

2. Verify server behavior:
   - Server receives but does not forward chat messages
   - Server correctly forwards route rumors

### 2. NAT Traversal Testing with Network Namespaces

1. Set up network namespaces as described in the Network Namespace Setup Guide

2. Start nodes in different namespaces:
   ```bash
   # Start node in nat1
   sudo ip netns exec nat1 ./P2Pal -port 11111 -connect <rendezvous-server-ip>:45678

   # Start node in nat2
   sudo ip netns exec nat2 ./P2Pal -port 22222 -connect <rendezvous-server-ip>:45678
   ```

3. Verify hole-punching functionality:
   - Both NATed nodes should discover each other through the rendezvous server
   - After initial discovery, nodes should establish direct communication
   - Verify that direct messaging works without going through the rendezvous server
   - Test private messaging between the NATed nodes

### 3. Sequence Number Verification

1. Monitor that increasing sequence numbers are properly maintained
2. Verify that nodes properly update routes when higher sequence numbers are received
3. Verify direct connection preference when sequence numbers are equal

## Performance Testing

1. Network Scalability:
   - Start 5-10 instances and verify correct route propagation
   - Monitor memory and CPU usage with increased node count

2. Route Recovery:
   - Disconnect intermediate nodes
   - Verify route re-establishment through alternative paths
   - Measure recovery time

3. Network Congestion:
   - Send multiple messages simultaneously
   - Verify all messages are delivered correctly

## System Testing

1. Firewall Testing:
   - Test with Windows Firewall enabled/disabled
   - Document required firewall exceptions

2. Cross-Platform Testing (if applicable):
   - Test on different operating systems
   - Verify compatibility
