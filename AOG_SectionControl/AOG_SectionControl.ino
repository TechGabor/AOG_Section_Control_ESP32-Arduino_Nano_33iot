
#define HardwarePlatform  0  //0 = runs on ESP32 1 = run on NANO 33 IoT

struct Storage {
//User config: ***********************************************************************************
	
	uint8_t DataTransViaUSB = 0;          //transfer data via USB  0: OFF 1: ON 
	uint8_t DataTransViaWiFi = 1;         //transfer data via WiFi	0: OFF 1: ON						   
									   
	uint8_t LEDWiFi_PIN = 13;// 2;      // WiFi Status LED 0 = off
	uint8_t LEDWiFi_ON_Level = HIGH;    //HIGH = LED on high, LOW = LED on low

	//WiFi---------------------------------------------------------------------------------------------
	//tractors WiFi
	char ssid[24] = "WLANHammer";// "Fendt_209V";           // WiFi network Client name
	char password[24] = "aveasillrac";                 // WiFi network password//Accesspoint name and password
	char ssid2[24] = "Fendt_209V";           // WiFi network Client name
	char password2[24] = "";                 // WiFi network password//Accesspoint name and password
	char ssid_ap[24] = "SectionControlNet";// name of Access point, if no WiFi found, no password!!
	uint8_t timeoutRouter = 20;       //time (s) to search for existing WiFi, than starting Accesspoint 
	uint8_t timeoutRouter2 = 20;       //time (s) to search for existing WiFi, than starting Accesspoint 
	//static IP
	uint8_t myIP[4] = { 192, 168, 2, 71 };     // Roofcontrol module 
	uint8_t gwIP[4] = { 192, 168, 2, 1 };      // Gateway IP also used if Accesspoint created
	uint8_t mask[4] = { 255, 255, 255, 0 };
	uint8_t myDNS[4] = { 192,168,2,1 };// { 8, 8, 8, 8 };         //optional
	uint8_t IPToAOG[4] = { 192,168,2,255 };
	uint8_t myIPEnding = 71;
	uint16_t PortSCToAOG = 5533;             //this is port of this module: Autosteer = 5577 IMU = 5566 GPS = 5544
	uint16_t PortFromAOG = 8888;            // port to listen for AOG
	uint16_t portDestination = 9999;    // Port of AOG that listens
	uint16_t BaudRate = 38400;                   //Baudrate = speed of serial port or USB or Bluetooth. AOG uses 38400 for UART

//the following lines should be configed by the user to fit the programm to the sprayer/ESP32
//GPIOs of the ESP32 (current setting is for the layout shown as example WIKI)

// if only 1 flowrate is used, use left
//Example1: motor valve is controled only by Switch not by AOG, no Flowmeter, : RateControl..Equiped = false; RateSW..Equiped = true; RateControlPRM = false;
//Example2: PWM valve, with flowmeter all controled by AOG:   RateControl..Equiped = true; RateSW..Equiped = true; RateControlPRM = true;	
	uint8_t RateControlLeftEquiped = 0;		//1 if Rate control is there, else: 0
	uint8_t RateSWLeftEquiped = 1;			//1 if Rate control Pressure switch is there, else: 0
	uint8_t RateSWLeft_PIN = 34;				//Rate +/- switch
	uint8_t RateControlPWM = 0;				//1 if PWM valve, 0 if Motor drive for pressure change		

	uint8_t	FlowDirLeft_PIN = 255;// 23;//19				//Rate-Control Valve/Motor Direktion
	uint8_t	FlowPWMLeft_PIN = 255;// 21;				//Rate-Control Valve PWM/Motor ON/OFF
	uint8_t	FlowEncALeft_PIN = 255;				//Flowmeter left/1

	uint8_t RateControlRightEquiped = 0;	//1 if Rate control is there, else: 0
	uint8_t RateSWRightEquiped = 0;			//1 if Rate control Pressure switch is there, else: 0	
	uint8_t	RateSWRight_PIN = 0;				//Rate +/- switch
	uint8_t	FlowDirRight_PIN = 0;				//0  = unused Rate-Control Valve Direktion
	uint8_t	FlowPWMRight_PIN = 0;				//0  = unused Rate-Control Valve PWM
	uint8_t	FlowEncARight_PIN = 0;				//Flowmeter right/2 
	

	uint8_t SectNum = 16;	// number of sections equiped max 16 (in AOG)
	uint8_t SectRelaysEquiped = 1;				//relays for SC output are equiped (0=no output, only documentation)
	uint8_t SectRelaysON = 1;					//relays spray on 1 or 0 (high or low)
	uint8_t Relay_PIN[16] = { 15,2,0,4,16,17,5,18,19,255,255,255,255,255,21,23 };		//GPIOs of ESP32 OUT to sections of sprayer HIGH/3.3V = ON

	uint8_t SectSWEquiped = 1;					//1 if section input switches are equiped, else: 0	
	uint8_t SectSWONorAUTO = 1;				//1 if section = auto/on on 3.3V 
	uint8_t SectSW_PIN[16] = { 13,12,14,27,26,255,255,255,25,255,255,255,255,255,255,33 };  //section switches to GPIOs of ESP32 GND = section off, open/+3.3V section auto/on

	uint8_t	SectMainSWType = 1;						// 0 = not equiped 1 = (ON)-OFF-(ON) toggle switch or push buttons 2 = connected to hitch level sensor 3 = inverted hitch level sensor
	uint16_t	HitchLevelVal = 2000;					// Value for hitch level: switch AOG section control to Auto if lower than...
	uint8_t	SectMainSW_PIN = 32;					//ESP32 to AOG Main auto toggle switch open=nothing/AOG button GND=OFF +3,3=AOG Auto on	OR connected to hitch level sensor	
	uint8_t	SectAutoManSW_PIN = 39;					// Main Auto/Manual switch 39:!!no internal pullup!!

	uint8_t DataToAOGHeader[2] = { 127,249 }, DataFromAOGHeader[2] = { 127, 250 }, SettingsFromAOGHeader[2] = { 127,248 }, n = 0xF8;
	uint8_t DataToAOGLength = 10; //items per UDP packet
	uint8_t DataFromAOGLength = 10; //items comming from AOG 
	uint8_t SettingsFromAOGLength = 10;

	bool debugmode = false;
	bool debugmodeRelay = false;
	bool debugmodeSwitches = false;
	bool debugmodeDataFromAOG = false;
	bool debugmodeDataToAOG = false;

 // END of user config ****************************************************************************
}; Storage SCSet;


bool EEPROM_clear = true;


// WiFi 
byte my_WiFi_Mode = 0;  // WIFI_STA = 1 = Workstation  WIFI_AP = 2  = Accesspoint

// WiFi LED blink times: searching WIFI: blinking 4x faster; connected: blinking as times set; data available: light on; no data for 2 seconds: blinking
unsigned int LED_WIFI_time = 0;
unsigned int LED_WIFI_pulse = 1400;   //light on in ms 
unsigned int LED_WIFI_pause = 700;   //light off in ms
boolean LED_WIFI_ON = false;



//ToDo: - Rate / flowmeter doesn't work, interrupt mode ect not defined
//      - line 260 PWM + Interrupt mode ect...
//		- Right side Rate caluclation / output / Motordrive
//      - Manual mode for Rate Control
//		- what to do if no WIFI??


byte RelayToAOG[] =    { 0,0,0,0,0,0,0,0,0,0 }; 
byte RelayToAOGOld[] = { 0,0,0,0,0,0,0,0,0,0 };
byte DataFromAOG[255]; byte incomingByte = 0, incomingByteNum = 0, DataToAOGLength = 10;

//libraries -------------------------------
#if HardwarePlatform == 1
#include <WiFiNINA.h>
#include <FlashAsEEPROM.h>
#endif
#if HardwarePlatform == 0
#include "EEPROM.h"
#endif
#include <WiFiUdp.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <WiFi.h>/*
#include <WiFiType.h>
#include <WiFiSTA.h>
#include <WiFiScan.h>
#include <WiFiMulti.h>
#include <WiFiGeneric.h>
#include <WiFiAP.h>
#include <ETH.h>*/
WiFiServer server(80);
WiFiClient client_page;
WiFiUDP UDPFromAOG = WiFiUDP();
WiFiUDP UDPToAOG = WiFiUDP();

//values to decide position of section switch
#define SWOFF 350 // analog in is lower than .. for off
#define SWON  2900 // analog in is lower than .. for auto/on

//analog value of the toggle switches
int MainSWVal = 2000;
int MainSWValOld = 2000;
int RateSWLeftVal = 2000;
int RateSWRightVal = 2000;

//state of the switches HIGH = ON/Auto LOW = OFF								 
boolean SectSWVal[16] = { HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH };
boolean AutoSWVal = HIGH;
byte debugmodeSwitchesBak = 3;  //0 = false 1 = true 3 = not used

//loop time variables in microseconds
const unsigned long LOOP_TIME = 1000;// 400; //in msec; 1000 = 1 Hz
const unsigned long SectSWDelayTime = 1500;//1400; //time the arduino waits after manual Switch is used before acception command from AOG in msec
unsigned long lastTime = LOOP_TIME;
unsigned long currentTime = LOOP_TIME;
unsigned long DataFromAOGTime = LOOP_TIME;
unsigned long SectAutoSWTime = LOOP_TIME;
unsigned long SectAutoSWlastTime = LOOP_TIME;
unsigned long SectMainSWlastTime = LOOP_TIME;
unsigned long RateSWlastTimeLeft = LOOP_TIME;
unsigned long RateSWDelayTime = 250; //time to pass before a new toggle of switch is accepted = if hold down, time between steps
unsigned long RateSWlastTimeRight = LOOP_TIME;
unsigned long SectSWcurrentTime = LOOP_TIME;
byte watchdogTimer = 0;

/* no rate control in AOG
//Kalman variables Left
float rateKLeft = 0;
float PcLeft = 0.0;
float GLeft = 0.0;
float PLeft = 1.0;
float XpLeft = 0.0;
float ZpLeft = 0.0;
float XeRateLeft = 0; //the filtered flowrate
const float varRateLeft = 100; // variance, smaller, more filtering
const float varProcessLeft = 10;

//Kalman variables Right
float rateKRight = 0;
float PcRight = 0.0;
float GRight = 0.0;
float PRight = 1.0;
float XpRight = 0.0;
float ZpRight = 0.0;
float XeRateRight = 0; //the filtered flowrate
const float varRateRight = 100; // variance, smaller, more filtering
const float varProcessRight = 10;
*/
//program flow
bool isDataFound = false, isSettingFound = false;

//bit 0 on byte[0] is section 1
byte RelayFromAOG[] = { 0, 0 }; //Sections ON / OFF Bitwise !!! LOWByte, HighByte
byte RelayOUT[] = { 0,0 }; //Sections ON / OFF Bitwise !!! LOWByte, HighByte
byte RelayOUTOld[] = { 0,0 };
byte SectSWOffToAOG[] = { 0,0 };
//byte SectSWOffToAOGOld[] = { 0,0 };
byte SectMainToAOG = 0;
//byte SectMainFromAOG = 0;
byte uTurnRelay = 0;
//  byte flowRateSW1 = 0;
boolean SectMainSWpressed = false;
boolean SectSWpressed = false;
boolean SectSWpressedLoop = false;
boolean SectAuto = true;
boolean SectAutoOld = true;
boolean SectAutoSWpressed = false;
//boolean RateAuto = false;
boolean RateManUpLeft = false;
boolean RateManUpRight = false;
boolean RateManDownLeft = false;
boolean RateManDownRight = false;

boolean SectMainOn = false;
boolean BitVal = false;

boolean NewDataToAOG = false;//wird nie true gesetzt!!

float groundSpeed = 0; //speed from AgOpenGPS is multiplied by 4
float rateSetPointLeft = 0.0;
float countsThisLoopLeft = 0;
float rateErrorLeft = 0; //for PID
int rateAppliedLPMLeft = 0;
float rateSetPointRight = 0.0;
float countsThisLoopRight = 0;
float rateErrorRight = 0; //for PID
int rateAppliedLPMRight = 0;

unsigned long accumulatedCountsLeft = 0;
float flowmeterCalFactorLeft = 50;
unsigned long accumulatedCountsRight = 0;
float flowmeterCalFactorRight = 50;
unsigned long accumulatedCountsDual = 0;

float pulseAverageLeft = 0;
float pulseAverageRight = 0;

//the ISR counter
volatile unsigned long pulseCountLeft = 0, pulseDurationLeft;
volatile unsigned long pulseCountRight = 0, pulseDurationRight;
bool state = false;

void setup()
{
	delay(300);//wait for power to stabilize
	delay(300);//wait for IO chips to get ready
	//set up communication
	Serial.begin(SCSet.BaudRate);
	delay(300);
	delay(1000);//debug only
	Serial.println();
	//Serial.flush();   // flush out buffer
	//get EEPROM data
	restoreEEprom();
	delay(300);
	RelayToAOG[0] = SCSet.DataToAOGHeader[0];
	RelayToAOGOld[0] = SCSet.DataToAOGHeader[0];
	RelayToAOG[1] = SCSet.DataToAOGHeader[1];
	RelayToAOGOld[1] = SCSet.DataToAOGHeader[1];
	
	//set GPIOs
	assignGPIOs();

	//start WiFi
	WiFi_Start_STA(1);
	delay(200);
	//try 2. wifi network if no connection
	if (my_WiFi_Mode == 0) {WiFi_Start_STA(2); }
	delay(200);
	if (my_WiFi_Mode == 0) {// if failed start AP
		WiFi_Start_AP();
		delay(100);
	}
	delay(100);

	UDP_Start();
	delay(100);
	//start server for settings homepage
	server.begin();
	delay(100);
}  //end setup

void loop() {
	getDataFromAOG();

	//read switches/inputs
	if ((SCSet.SectSWEquiped) || (SCSet.SectMainSWType != 0))
	{
		SectSWRead(); //checks if section switch is toggled and sets RelayOUT
		
		//reset debugSwitches: has been change to true, if debugmodeDataToAOG = true for 1 loop
		if ((SCSet.debugmodeSwitches) && (debugmodeSwitchesBak == 0)) { SCSet.debugmodeSwitches = false; }

		//RelayToAOG 5+6 set in ReadSwitches_buildBytes HiByte, LowByte if bit set -> AOG section forced ON
		RelayToAOG[7] = SectSWOffToAOG[1]; //HiByte if bit set -> AOG section forced off
		RelayToAOG[8] = SectSWOffToAOG[0]; //LowByte if bit set -> AOG section forced off
		RelayToAOG[9] = SectMainToAOG; // Bits: AOG section control AUTO, Section control OFF, Rate L+R ...

		for (byte idx = 2; idx <= 9; idx++) {
			if (RelayToAOG[idx] != RelayToAOGOld[idx]) {
				NewDataToAOG = true;
				RelayToAOGOld[idx] = RelayToAOG[idx];
			}
		}
	}
	//no switches: set Relais as AOG commands
	else { RelayOUT[0] = RelayFromAOG[0]; RelayOUT[1] = RelayFromAOG[1]; }

	SetRelays();

	//Rate switches and motor drive
	if ((SCSet.RateSWLeftEquiped == 1) || (SCSet.RateSWRightEquiped == 1)) { RateSWRead(); }
	if (SCSet.RateControlLeftEquiped == 0) { motorDrive(); } //if Manual do everytime, not only in timed loop

	doWebInterface();

	//timed loop: runs with 1Hz or if new data from switches to send data to AOG
	currentTime = millis();
	if ((currentTime > LOOP_TIME + lastTime) || (NewDataToAOG)) {
		lastTime = currentTime;
		AOGDataSend();
		if (NewDataToAOG) {
			delay(10); //debounce switches
			NewDataToAOG = false;
			AOGDataSend(); //send 2. time for safety
		}
		if (SCSet.debugmode) {
		Serial.print("timed loop (sendig data to AOG) running, timing (ms): "); Serial.println(LOOP_TIME); }
		if (SCSet.debugmodeDataToAOG) {
			Serial.print("Data to AOG ");
			for (byte n = 0; n < SCSet.DataToAOGLength; n++) {
				Serial.print(RelayToAOG[n]); Serial.print(" ");
			}
			Serial.println();
			//printing swich inpu 1x:
			if (!SCSet.debugmodeSwitches) {
				debugmodeSwitchesBak = 0;
				SCSet.debugmodeSwitches = true;
			}
			else debugmodeSwitchesBak = 1;
		}

	}


}