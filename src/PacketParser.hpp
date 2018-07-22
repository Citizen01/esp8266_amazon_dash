#ifndef PACKET_PARSER_HPP
#define PACKET_PARSER_HPP

#include <cinttypes>
#include <Print.h>

struct eth_hdr;
struct ip_addr_packed;
struct ip_hdr;
struct pbuf;

typedef void (*ArpCallback) (const String& src);

class PacketParser
{
  public:
    explicit PacketParser(Print& output);
    explicit PacketParser(Print& output, ArpCallback arpcallback);
  
    void processEthernet(const pbuf* p, uint16_t off);
  
    void processArp(const pbuf* p, uint16_t off, const eth_hdr* eth);
  
    void processIpv4(const pbuf* p, uint16_t off, const eth_hdr* eth);
  
    void processTcp(const pbuf* p, uint16_t off, const ip_hdr* ip);
  
    void processUdp(const pbuf* p, uint16_t off, const ip_hdr* ip);

    inline void setArpCallback(ArpCallback arpcallback) { f_arpcallback = arpcallback; }
  
  protected:
    void printEthSrcDst(const eth_hdr* eth);
  
    void printIpv4Addr(ip_addr_packed a);
  
    void printIpv4SrcDst(const ip_hdr* ip);
  
  private:
    Print& m_output;
    ArpCallback f_arpcallback = nullptr;
};

#endif // PACKET_PARSER_HPP
