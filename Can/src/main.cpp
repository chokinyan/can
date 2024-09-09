#include <Arduino.h>
#include <ACAN2515.h>

// Utilisation du MCP2515 relié par bus SPI
// Voir dans doc les pins à utiliser en fonction du type de cible (ESP32, arduino...)

#define cs_2515 5          // réglage broche CS
#define int_2515 36          // réglage broche Interuption 
#define f_2515 16000000     // réglage Fréquence du quartz de la carte MCP2515 
#define f_bus_can 250000      // réglage Vitesse du bus CAN


ACAN2515 can2515(cs_2515, SPI, int_2515); // Déclaration de l'objet CAN utilisant le réglages des broches définis avant


// Declaration d'un masque (param 1) qui utilise les 11 bits d'identification de message
// et qui n'utilise pas les 2 premiers octets de data (param 2 et 3) optionnels
const ACAN2515Mask masque = standard2515Mask(0b11111111111, 0, 0);

// Declaration procedure de traitement  filtre
void message_1octet(const CANMessage & inMessage);


//réglage identifiant trame voulue
const ACAN2515AcceptanceFilter filtres[] = {
   { standard2515Filter(0x3E1, 0, 0), message_1octet }, 

};

CANMessage messageCANReception;


void setup()
{
	Serial.begin(9600);
	Serial.println("Init CAN ok");
	delay(500);
	SPI.begin();
	ACAN2515Settings can_vit(f_2515, f_bus_can);
    
	// Demarrage CAN (le 2eme param s'appelle "une closure" il permet de relier
	// le code à la fonction qui traite l'intéruption dans la lib ACAN2515)
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
}


//Traitement message par fonction
void message_1octet(const CANMessage & inMessage)
{

}


void loop()
{
	
    if (can2515.receive(messageCANReception))  // test si un message est arrivé
    {
    // Un message CAN est arrive 
    
    can2515.receive(messageCANReception) ;
  
    Serial.println("Messages obtenus: ") ;
    Serial.println(messageCANReception.data[0]);
   
    } 
    
    
    delay(100);

}
	