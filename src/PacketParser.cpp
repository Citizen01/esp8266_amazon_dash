#include "PacketParser.hpp"
#include <lwip/ip.h>
#include <lwip/tcp_impl.h>
#include <lwip/udp.h>
#include <netif/etharp.h>

#define PBUF_PAYLOAD_OFF(p, T, off) \
  reinterpret_cast<const T*>(reinterpret_cast<const uint8_t*>(p->payload) + (off))

PacketParser::PacketParser(Print& output) : m_output(output)
{
}

PacketParser::PacketParser(Print& output, ArpCallback arpcallback) : m_output(output), f_arpcallback(arpcallback)
{
}

void PacketParser::printEthSrcDst(const eth_hdr* eth)
{
  m_output.printf_P(PSTR(" src=%02x:%02x:%02x:%02x:%02x:%02x dst=%02x:%02x:%02x:%02x:%02x:%02x"),
                    eth->src.addr[0], eth->src.addr[1], eth->src.addr[2], eth->src.addr[3], eth->src.addr[4], eth->src.addr[5],
                    eth->dest.addr[0], eth->dest.addr[1], eth->dest.addr[2], eth->dest.addr[3], eth->dest.addr[4], eth->dest.addr[5]);
}

void PacketParser::processEthernet(const pbuf* p, uint16_t off)
{
  if (p->len < off + sizeof(eth_hdr)) {
    m_output.print(F("not-Ethernet"));
    m_output.print(F("\n"));
    return;
  }

  auto eth = PBUF_PAYLOAD_OFF(p, eth_hdr, off);
  off += sizeof(eth_hdr);

  switch (eth->type) {
    case PP_HTONS(ETHTYPE_ARP):
      processArp(p, off, eth);
      break;
    case PP_HTONS(ETHTYPE_IP):
      processIpv4(p, off, eth);
      m_output.print(F("\n"));
      break;
    default:
      m_output.print(F("Ethernet"));
      printEthSrcDst(eth);
      m_output.printf_P(PSTR(" ethtype=%04x"), ntohs(eth->type));
      m_output.print(F("\n"));
      break;
  }
}

void PacketParser::processArp(const pbuf* p, uint16_t off, const eth_hdr* eth)
{  
  if (p->len < off + sizeof(etharp_hdr)) {
    m_output.println(F("ARP-truncated"));
    printEthSrcDst(eth);
    m_output.print(F("\n"));
    return;
  }

  auto arp = PBUF_PAYLOAD_OFF(p, etharp_hdr, off);

  m_output.print(F("ARP"));
  printEthSrcDst(eth);
  m_output.printf_P(PSTR(" opcode=%02" PRIx16), ntohs(arp->opcode));
  m_output.print(F("\n"));

  if (f_arpcallback != nullptr) { // Calling callback
    char tbs[17];
    sprintf(tbs, "%02x:%02x:%02x:%02x:%02x:%02x",
      eth->src.addr[0], eth->src.addr[1], eth->src.addr[2], eth->src.addr[3], eth->src.addr[4], eth->src.addr[5]);
    f_arpcallback(tbs);
  }
}

void PacketParser::printIpv4Addr(ip_addr_packed a)
{
  uint32_t addr = ntohl(a.addr);
  m_output.printf_P(PSTR("%d.%d.%d.%d"), (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, (addr >> 0) & 0xFF);
}

void PacketParser::printIpv4SrcDst(const ip_hdr* ip)
{
  m_output.print(F(" src="));
  printIpv4Addr(ip->src);
  m_output.print(F(" dst="));
  printIpv4Addr(ip->dest);
}

void PacketParser::processIpv4(const pbuf* p, uint16_t off, const eth_hdr* eth)
{
  if (p->len < off + sizeof(ip_hdr)) {
    m_output.print(F("IP-truncated"));
    printEthSrcDst(eth);
    return;
  }

  auto ip = PBUF_PAYLOAD_OFF(p, ip_hdr, off);
  off += IPH_HL(ip) << 2;

  if ((IPH_OFFSET(ip) & PP_HTONS(IP_OFFMASK | IP_MF)) != 0) {
    m_output.print(F("IP-fragmented"));
    printIpv4SrcDst(ip);
    return;
  }

  switch (IPH_PROTO(ip)) {
    case IP_PROTO_TCP:
      processTcp(p, off, ip);
      break;
    case IP_PROTO_UDP:
      processUdp(p, off, ip);
      break;
    default:
      m_output.print(F("IP"));
      printIpv4SrcDst(ip);
      m_output.printf_P(PSTR(" proto=%02" PRIx8), IPH_PROTO(ip));
      break;
  }
}

void PacketParser::processTcp(const pbuf* p, uint16_t off, const ip_hdr* ip)
{
  if (p->len < off + sizeof(tcp_hdr)) {
    m_output.print(F("TCP-truncated"));
    printIpv4SrcDst(ip);
    return;
  }

  auto tcp = PBUF_PAYLOAD_OFF(p, tcp_hdr, off);

  m_output.print(F("TCP src="));
  printIpv4Addr(ip->src);
  m_output.printf_P(PSTR(":%" PRId16 " dst="), ntohs(tcp->src));
  printIpv4Addr(ip->dest);
  m_output.printf_P(PSTR(":%" PRId16), ntohs(tcp->dest));
}

void PacketParser::processUdp(const pbuf* p, uint16_t off, const ip_hdr* ip)
{
  if (p->len < off + sizeof(udp_hdr)) {
    m_output.print(F("UDP-truncated"));
    printIpv4SrcDst(ip);
    return;
  }

  auto udp = PBUF_PAYLOAD_OFF(p, udp_hdr, off);

  m_output.print(F("UDP src="));
  printIpv4Addr(ip->src);
  m_output.printf_P(PSTR(":%" PRId16 " dst="), ntohs(udp->src));
  printIpv4Addr(ip->dest);
  m_output.printf_P(PSTR(":%" PRId16), ntohs(udp->dest));
}

