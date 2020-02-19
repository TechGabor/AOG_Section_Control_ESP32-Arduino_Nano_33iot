// WIFI handling 11. Feb 2020 for ESP32 and Nano 33 IoT -------------------------------------------


//here supporting 2 WiFi networks, so send number of network!!

void WiFi_Start_STA(byte NW) {
  unsigned long timeout;

 // WiFi.mode(WIFI_STA);   //  Workstation
  
  /*if (!WiFi.config(SCSet.myIP, SCSet.gwIP, SCSet.mask, SCSet.myDNS))
    {Serial.println("STA Failed to configure\n"); }
    else { Serial.println("trying to connect to WiFi"); }
  */
  if (NW == 2) {
	  WiFi.begin(SCSet.ssid2, SCSet.password2); 
      timeout = millis() + (SCSet.timeoutRouter2 * 1000); 
      Serial.print("try to connect to WiFi: "); Serial.println(SCSet.ssid2);
  }
  else {
	  WiFi.begin(SCSet.ssid, SCSet.password); 
      timeout = millis() + (SCSet.timeoutRouter * 1000); 
      Serial.print("try to connect to WiFi: "); Serial.println(SCSet.ssid);
  }
  while (WiFi.status() != WL_CONNECTED && millis() < timeout) {
    delay(300);
    Serial.print(".");
    //WIFI LED blink in double time while connecting
    if (!LED_WIFI_ON) {
        if (millis() > (LED_WIFI_time + (LED_WIFI_pause >> 2))) 
          {
           LED_WIFI_time = millis();
           LED_WIFI_ON = true;
           digitalWrite(SCSet.LEDWiFi_PIN, !SCSet.LEDWiFi_ON_Level);
          }
    }
    if (LED_WIFI_ON) {
      if (millis() > (LED_WIFI_time + (LED_WIFI_pulse >> 2))) {
        LED_WIFI_time = millis();
        LED_WIFI_ON = false;
        digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level);
      }
    }
  }  //connected or timeout  
  
  Serial.println(""); //NL  
  if (WiFi.status() == WL_CONNECTED)
  {
      delay(200);  
      Serial.println();
      Serial.print("WiFi Client successfully connected to : ");
      if (NW == 2) { Serial.println(SCSet.ssid2); } else { Serial.println(SCSet.ssid); }
      Serial.print("Connected IP - Address : ");
      IPAddress myIP = WiFi.localIP();
      Serial.println(myIP);
      IPAddress gwIP = WiFi.gatewayIP();
 #if HardwarePlatform == 0     //on ESP32: after connecting get IP from router -> change it to x.x.x.IP Ending (from settings)
      myIP[3] = SCSet.myIPEnding; //set ESP32 IP to x.x.x.myIP_ending
      Serial.print("changing IP to: ");
      Serial.println(myIP);      
      if (!WiFi.config(myIP, gwIP, SCSet.mask, SCSet.myDNS)) { Serial.println("STA Failed to configure"); }
      delay(300);
      Serial.print("Connected IP - Address : ");
      myIP = WiFi.localIP();
      Serial.println(myIP);
#endif
      Serial.print("Gateway IP - Address : ");
      Serial.println(gwIP);
      SCSet.IPToAOG[0] =myIP[0];
      SCSet.IPToAOG[1] = myIP[1];
      SCSet.IPToAOG[2] = myIP[2];
      SCSet.IPToAOG[3] = 255;//set IP to x.x.x.255 according to actual network
      LED_WIFI_ON = true;
      digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level);
      my_WiFi_Mode = 1;// WIFI_STA;
  }
  else
  {
     // WiFi.end();
      Serial.println("WLAN-Client-Connection failed");
      Serial.println();
      LED_WIFI_ON = false;
      digitalWrite(SCSet.LEDWiFi_PIN, !SCSet.LEDWiFi_ON_Level);
  }
  delay(20);
}


//-------------------------------------------------------------------------------------------------
// start WiFi Access Point = only if no existing WiFi

//ESP32
#if HardwarePlatform == 0 
void WiFi_Start_AP() {
  WiFi.mode(WIFI_AP);   // Accesspoint
  WiFi.softAP(SCSet.ssid_ap, "");
  while (!SYSTEM_EVENT_AP_START) // wait until AP has started
   {
    delay(100);
    Serial.print(".");
   }   
  delay(100);//right IP adress only with this delay 
  WiFi.softAPConfig(SCSet.gwIP, SCSet.gwIP, SCSet.mask);  // set fix IP for AP  

  IPAddress getmyIP = WiFi.softAPIP();
  delay(300);

  //AP_time = millis();
  Serial.print("Accesspoint started - Name : ");
  Serial.println(SCSet.ssid_ap);
  Serial.print( " IP address: ");
  Serial.println(getmyIP);
  LED_WIFI_ON = true;
  digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level);
  my_WiFi_Mode = WIFI_AP;
}
#endif

// AccessPoint on Nano 33 IoT

#if HardwarePlatform == 1
void WiFi_Start_AP() {

    my_WiFi_Mode = WiFi.beginAP(SCSet.ssid_ap, "");
    delay(500);
    if (my_WiFi_Mode != WL_AP_LISTENING) {
        Serial.println("Creating access point failed");
        delay(500);
        WiFi.end();
        delay(500);
        my_WiFi_Mode = WiFi.beginAP(SCSet.ssid_ap, "");
        if (my_WiFi_Mode != WL_AP_LISTENING) {
            Serial.println("Creating access point failed");
        }
    }
    delay(300);//right IP adress only with this delay 
    WiFi.config(SCSet.gwIP, SCSet.gwIP, SCSet.mask);  // set fix IP for AP  
    delay(300); 
    IPAddress getmyIP = WiFi.localIP();
    delay(300);

    //AP_time = millis();
    Serial.print("Accesspoint started - Name : ");
    Serial.println(SCSet.ssid_ap);
    Serial.print(" IP address: ");
    Serial.println(getmyIP);
    LED_WIFI_ON = true;
    digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level);
    my_WiFi_Mode = WiFi.status();
}

#endif

//-------------------------------------------------------------------------------------------------

void UDP_Start()
{   //init UPD
    if (UDPToAOG.begin(SCSet.PortSCToAOG))
    {
        Serial.print("UDP sendig to IP: ");
        for (byte n = 0; n < 4; n++) {
            Serial.print(SCSet.IPToAOG[n]);
            Serial.print(".");
        }
        Serial.print(" from port: ");
        Serial.print(SCSet.PortSCToAOG);
        Serial.print(" to port: ");
        Serial.println(SCSet.portDestination);
    }
    delay(300);
    if (UDPFromAOG.begin(SCSet.PortFromAOG))
    {
        Serial.print("UDP listening for AOG data on IP: ");
        Serial.println(WiFi.localIP());
        Serial.print(" on port: ");
        Serial.println(SCSet.PortFromAOG);
        //getAOGSteerData();
    }
}





/* if (UDPFromAOG.begin(SCSet.PortFromAOG))
    {
        Serial.print("Section Control UDP Listening to port: ");
        Serial.println(SCSet.PortFromAOG);
        Serial.println();
    }
    delay(50);
    if (UDPToAOG.begin(SCSet.PortSCToAOG))
    {
        Serial.print("UDP writing to IP: ");
        Serial.print(SCSet.IPToAOG[0]); Serial.print("."); Serial.print(SCSet.IPToAOG[1]);  Serial.print("."); Serial.print(SCSet.IPToAOG[2]);  Serial.print("."); Serial.println(SCSet.IPToAOG[3]);
        Serial.print("UDP writing to port: ");
        Serial.println(SCSet.portDestination);
        Serial.print("UDP writing from port: ");
        Serial.println(SCSet.PortSCToAOG);
    }
}*/