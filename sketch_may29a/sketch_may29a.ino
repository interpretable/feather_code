#include <EthernetUdp.h>
#include <Ethernet.h>
#include <SPI.h>
#include <EEPROM.h>

const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin
int buttonState = 0;         // variable for reading the pushbutton status
int held = 0;
int ok = 0;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0xA8, 0x61, 0x0A, 0xAE, 0x51, 0x94}; // depends on the ethernet shield 
byte target_mac[] = {0x80, 0xEE, 0x73, 0xCE, 0x83, 0x1F}; //depends on the target  

IPAddress ip(192,168,1,133);
IPAddress remote(192,168,1,2);
IPAddress broadcastIP(192,168,1,255);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);


unsigned int localPort = 8888;      // local port to listen on

// Initialize magic packet 
const int nMagicPacketLength = 102;
byte abyMagicPacket[nMagicPacketLength] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

char request[] = "shutdown "; // string for shutdown

EthernetUDP Udp;

void setup() {
  
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);

  Ethernet.begin(mac,ip,gateway,gateway,subnet);
  ok = Udp.begin(localPort);
  Serial.println(ok);

  // Make the magic packet for WoL
  for (int ix=6; ix<102; ix++)
      abyMagicPacket[ix]=target_mac[ix%6];

  Serial.println("Initialisation terminée"); 
  
}

void loop() {

  
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  delay(100);
  if (buttonState == LOW)
    {
      Serial.println("Vous avez appuyé");
      while (buttonState == LOW) // count how long the button is pushed
      {
        delay(100);
        held++;
        buttonState=digitalRead(2);
      }
      Serial.println(held);
      Serial.println("Statut");
      Serial.println(ok);
      if (held<30) // if short press
      {
        Udp.beginPacket(remote, localPort);
        Udp.write(abyMagicPacket,nMagicPacketLength); // send magic packet for WoL 
        Udp.endPacket();
        Serial.println("finished 1");
      }
      if (held>29) // if long press
      {
        Udp.beginPacket(remote, localPort);
        Udp.write(request,strlen(request)); // send the string that will be recognize by the python script
        Udp.endPacket();
        Serial.println("finished 2");
      }
      held=0;
    }
}
