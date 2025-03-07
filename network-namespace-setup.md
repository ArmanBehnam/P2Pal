# Network Namespace Setup Guide

This guide explains how to set up network namespaces to test the NAT traversal functionality of P2Pal.

## Prerequisites
- Linux operating system with network namespace support
- Root privileges
- iptables installed

## Setup Steps

### 1. Create Network Namespaces
Create two separate network namespaces to simulate different NAT environments:

```bash
sudo ip netns add nat1
sudo ip netns add nat2
```

### 2. Create Virtual Interfaces
Set up virtual ethernet pairs to connect the namespaces:

```bash
# Create veth pair for nat1
sudo ip link add veth1-host type veth peer name veth1-ns
sudo ip link set veth1-ns netns nat1

# Create veth pair for nat2
sudo ip link add veth2-host type veth peer name veth2-ns
sudo ip link set veth2-ns netns nat2
```

### 3. Configure IP Addressing
Assign IP addresses to interfaces:

```bash
# Host interfaces
sudo ip addr add 10.0.1.1/24 dev veth1-host
sudo ip addr add 10.0.2.1/24 dev veth2-host
sudo ip link set veth1-host up
sudo ip link set veth2-host up

# nat1 namespace
sudo ip netns exec nat1 ip addr add 10.0.1.2/24 dev veth1-ns
sudo ip netns exec nat1 ip link set veth1-ns up
sudo ip netns exec nat1 ip link set lo up
sudo ip netns exec nat1 ip route add default via 10.0.1.1

# nat2 namespace
sudo ip netns exec nat2 ip addr add 10.0.2.2/24 dev veth2-ns
sudo ip netns exec nat2 ip link set veth2-ns up
sudo ip netns exec nat2 ip link set lo up
sudo ip netns exec nat2 ip route add default via 10.0.2.1
```

### 4. Configure NAT
Set up NAT rules with iptables to simulate different NAT behaviors:

```bash
# Enable IP forwarding on host
sudo sysctl -w net.ipv4.ip_forward=1

# Standard NAT for nat1
sudo iptables -t nat -A POSTROUTING -s 10.0.1.0/24 -j MASQUERADE

# Random port NAT for nat2 to simulate challenging NAT environments
sudo ip netns exec nat2 iptables -t nat -A POSTROUTING -j MASQUERADE --random
```

### 5. Network Validation
Verify connectivity:

```bash
# Test connectivity from nat1
sudo ip netns exec nat1 ping -c 3 10.0.1.1

# Test connectivity from nat2
sudo ip netns exec nat2 ping -c 3 10.0.2.1
```

## Running P2Pal in Network Namespaces

### 1. Start Rendezvous Server
First, start a P2Pal instance in rendezvous mode:

```bash
./P2Pal -port 45678 -noforward
```

### 2. Start P2Pal in NAT Environments
Connect instances from both NAT environments to the rendezvous server:

```bash
# Start P2Pal in nat1
sudo ip netns exec nat1 ./P2Pal -port 11111 -connect <rendezvous-server-ip>:45678

# Start P2Pal in nat2
sudo ip netns exec nat2 ./P2Pal -port 22222 -connect <rendezvous-server-ip>:45678
```

### Cleanup
Remove the namespaces when testing is complete:

```bash
sudo ip netns delete nat1
sudo ip netns delete nat2
sudo ip link delete veth1-host
sudo ip link delete veth2-host
```
