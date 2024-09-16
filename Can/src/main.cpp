#include <U8g2lib.h>
#include <Wire.h>
#include <Arduino.h>
#include <ACAN2515.h>

#define cs_2515 5          // réglage broche CS
#define int_2515 36          // réglage broche Interuption
#define f_2515 16000000     // réglage Fréquence du quartz de la carte MCP2515 
#define f_bus_can 250000      // réglage Vitesse du bus CAN

ACAN2515 can2515(cs_2515, SPI, int_2515); // Déclaration de l'objet CAN utilisant le réglages des broches définis avant
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
const ACAN2515Mask masque = standard2515Mask(0b11111111111, 0, 0);

void message_1octet(const CANMessage & inMessage);

class RegimeMoteur {
    public:
        String hex;
        int regime;
        double setapDesBarre;
        CANMessage message;
        RegimeMoteur(CANMessage message) {
            message = message;
            regime = (message.data[0]*256 + message.data[1]) / 8;
            hex = String(message.data[0], HEX) + String(message.data[1], HEX);
            setapDesBarre = regime / 1000;
        }
};

const ACAN2515AcceptanceFilter filtres[] = {
   { standard2515Filter(0x3E1, 0, 0), message_1octet }, 

};

CANMessage messageCANReception;

void setup(void) 
{
  Serial.begin(9600);
  SPI.begin();
  ACAN2515Settings can_vit(f_2515, f_bus_can);
  const uint16_t v_err = can2515.begin(can_vit, [] { can2515.isr(); },masque, filtres, 2 );   
  
  if (v_err == 0) 
    { 
        Serial.println("Recepteur: Config ok");
    } 
	else 
    {
        Serial.println("Recepteur: Config HS"); 
        while (1); 
    }
	
	Serial.println("Attente de message !");
	delay(500);

  u8g2.setI2CAddress(0x78);
  u8g2.begin();
  u8g2.enableUTF8Print(); //nécessaire pour écrire des caractères accentués

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x13_tf);
  u8g2.setCursor(5,32);
  u8g2.print("Gherbi & Lalaouna");
  u8g2.sendBuffer();
  delay(2000);
}


void message_1octet(const CANMessage & inMessage)
{

}

void loop()
{
	
    if (can2515.receive(messageCANReception))  // test si un message est arrivé
    {
    // Un message CAN est arrive 
    
    can2515.receive(messageCANReception);

    RegimeMoteur MoteurInfo(messageCANReception);
    
    Serial.println(MoteurInfo.setapDesBarre);
    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x13_tf);
    u8g2.setCursor(8,32);
    for(int i=0; i<9; i++){
        u8g2.print(i);
        u8g2.setCursor(8+(13.3*(i+1)),32);
    }

    u8g2.drawBox(8, 40,8+13.3*MoteurInfo.setapDesBarre, 10);

    u8g2.sendBuffer();
    }

}
