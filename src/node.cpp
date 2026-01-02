#include "node.hpp"
#include <iostream>

void Node::receive_packet(const Packet &pkt)
{
  std::cout << "Node " << id << " received packet " << pkt.id << std::endl;
}

void Node::send_packet(const Packet &pkt, int next_node)
{
  std::cout << "Node " << id << " sending packet " << pkt.id
            << " to node " << next_node << std::endl;
}
