// [treesource] This defines the behavior of Nodes.

#include <iostream>
#include "node.hpp"
#include "packet.hpp"

void Node::receive_packet(const Packet &pkt)
{
  std::cout << "Node " << nid << " received packet " << pkt.id << std::endl;
}

void Node::send_packet(const Packet &pkt, int next_node)
{
  std::cout << "Node " << nid << " sending packet " << pkt.id
            << " to node " << next_node << std::endl;
}
