# esp8266_amazon_dash
ESP8266 project to perform custom actions when pressing an amazon dash button

I bought a bunch of amazon dash buttons when they were on sale at 1.99€ each and wanted to trigger custom stuff when pressing on one of them.

## How to use

You have to configure network settings of your Amazon dash button to use the same LAN network than your ESP8266. 

Right now it only allows for 1 dash button.

## Note for compiling
Make sure you use the `1.4 Prebuilt / 1.4 Higher Bandwidth` variant of `lwIP`.

## What it looks like

My setup:

- Amazon Dash Button - [4.99€ on Amazon.fr](https://www.amazon.fr/b/?ie=UTF8&node=10909388031)
- WeMos D1 Mini - [2.80€ on AliExpress.com](https://aliexpress.com/store/product/ESP8266-ESP12-ESP-12-WeMos-D1-Mini-WIFI-Dev-Kit-Development-Board-NodeMCU-Lua/2130127_32653918483.html)

```bash
Replacing callbacks on netif ew0
< UDP src=0.0.0.0:68 dst=255.255.255.255:67
> UDP src=192.168.43.1:67 dst=192.168.43.35:68
< UDP src=0.0.0.0:68 dst=255.255.255.255:67
> UDP src=192.168.43.1:67 dst=192.168.43.35:68
< ARP src=5c:cf:7f:c0:e1:bf dst=ff:ff:ff:ff:ff:ff opcode=01
Catched an ARP request from 5c:cf:7f:c0:e1:bf

< ARP src=5c:cf:7f:c0:e1:bf dst=ff:ff:ff:ff:ff:ff opcode=01
Catched an ARP request from 5c:cf:7f:c0:e1:bf

< ARP src=5c:cf:7f:c0:e1:bf dst=ff:ff:ff:ff:ff:ff opcode=01
Catched an ARP request from 5c:cf:7f:c0:e1:bf

< IP src=192.168.43.35 dst=224.0.0.1 proto=02
WiFi connected IP=192.168.43.35

*Here I press the dash button*

> UDP src=0.0.0.0:68 dst=255.255.255.255:67
> ARP src=6c:56:97:59:91:07 dst=ff:ff:ff:ff:ff:ff opcode=01
Catched an ARP request from 6c:56:97:59:91:07
Lipton dash pressed !

*Here I press the dash button again*

> UDP src=0.0.0.0:68 dst=255.255.255.255:67
> ARP src=6c:56:97:59:91:07 dst=ff:ff:ff:ff:ff:ff opcode=01
Catched an ARP request from 6c:56:97:59:91:07
Lipton dash pressed !
```