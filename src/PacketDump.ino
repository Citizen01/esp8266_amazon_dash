#include <cassert>
#include <ESP8266WiFi.h>
#include <lwip/netif.h>
#include "PacketParser.hpp"

const char* WIFI_SSID = "CiviWifi";
const char* WIFI_PASS = "wifi2018";

const String liptonDash = "6c:56:97:59:91:07";

PacketParser pp(Serial);

netif_input_fn oldInput = nullptr;
netif_linkoutput_fn oldLinkOutput = nullptr;

void processPacket(char dir, const pbuf* p)
{
    Serial.print(dir);
    Serial.print(' ');
    pp.processEthernet(p, 0);
}

err_t myInput(pbuf* p, netif* inp)
{
    processPacket('>', p);
    return (*oldInput)(p, inp);
}

err_t myLinkOutput(netif* netif, pbuf* p)
{
    processPacket('<', p);
    return (*oldLinkOutput)(netif, p);
}

void replaceNetifFns(netif* netif)
{
    Serial.printf_P(PSTR("Replacing callbacks on netif %c%c%d\n"), netif->name[0], netif->name[1], netif->num);
    oldInput = netif->input;
    netif->input = myInput;
    oldLinkOutput = netif->linkoutput;
    netif->linkoutput = myLinkOutput;
}

void handleArpCallback(const String& src) {

    Serial.print("Catched an ARP request from ");
    Serial.println(src);

    if (liptonDash.equals(src)) {
        Serial.println("Lipton dash pressed !");
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println();

    pp.setArpCallback(handleArpCallback);

    WiFi.persistent(false);
    // wifi_promiscuous_enable(1);
    WiFi.mode(WIFI_STA);

    assert(netif_list != nullptr); // should have one interface
    assert(netif_list->next == nullptr); // should have only one interface
    replaceNetifFns(netif_list);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.println(F("Not connected."));
        delay(1000);
    }
    delay(1000);
    Serial.print(F("WiFi connected IP="));
    Serial.println(WiFi.localIP());
}

void loop()
{
}

