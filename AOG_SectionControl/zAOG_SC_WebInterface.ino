// Wifi variables & definitions
#define MAX_PACKAGE_SIZE 2048
char HTML_String[20000];
char HTTP_Header[150];

// Allgemeine Variablen

int Aufruf_Zaehler = 0;

#define ACTION_SET_SSID        1  
#define ACTION_SET_SSID2       2
#define ACTION_SET_OUTPUT_TYPE 3  
#define ACTION_SET_loadDefault 4
#define ACTION_SET_Msg          5
#define ACTION_SET_RESTART     13
#define ACTION_SET_GPIO        14
#define ACTION_SET_DataTransfVia 21
#define ACTION_SET_WiFiLEDon    22

int action;

//-------------------------------------------------------------------------------------------------

void doWebInterface() {

    char my_char;
    int htmlPtr = 0;
    int myIdx;
    int myIndex;
    unsigned long my_timeout;



/*
    WiFiClient client = server.available();   // listen for incoming clients

    if (client) {                             // if you get a client,
        Serial.println("new client");           // print a message out the serial port
        String currentLine = "";                // make a String to hold incoming data from the client
        while (client.connected()) {            // loop while the client's connected
            if (client.available()) {             // if there's bytes to read from the client,
                char c = client.read();             // read a byte, then
                Serial.write(c);                    // print it out the serial monitor
                if (c == '\n') {                    // if the byte is a newline character

                  // if the current line is blank, you got two newline characters in a row.
                  // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();

                        // the content of the HTTP response follows the header:
                        client.print("Click <a href=\"/H\">here</a> turn the LED on<br>");
                        client.print("Click <a href=\"/L\">here</a> turn the LED off<br>");

                        // The HTTP response ends with another blank line:
                        client.println();
                        // break out of the while loop:
                        break;
                    }
                    else {      // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                }
                else if (c != '\r') {    // if you got anything else but a carriage return character,
                    currentLine += c;      // add it to the end of the currentLine
                }

                // Check to see if the client request was "GET /H" or "GET /L":
                if (currentLine.endsWith("GET /H")) {
                    digitalWrite(SCSet.LEDWiFi_PIN, HIGH);               // GET /H turns the LED on
                }
                if (currentLine.endsWith("GET /L")) {
                    digitalWrite(SCSet.LEDWiFi_PIN, LOW);                // GET /L turns the LED off
                }
            }
        }
        // close the connection:
        client.stop();
        Serial.println("client disconnected");
    }
    */


    // Check if a client has connected
    client_page = server.available();

    if (!client_page)  return;

    Serial.println("New Client:");           // print a message out the serial port

    my_timeout = millis() + 250L;

    while (client_page.connected() && (millis() < my_timeout)) {
        delay(10);
        if (millis() > my_timeout)
        {
            Serial.print("Client connection timeout!\n");
            break;// return;
        }
    }
     //   if (client_page.available()) {
            //---------------------------------------------------------------------
            //htmlPtr = 0;
            char c;
            if (client_page) {                        // if you get a client,
              //Serial.print("New Client.\n");                   // print a message out the serial port
                String currentLine = "";                // make a String to hold incoming data from the client
                while (client_page.connected()) {       // loop while the client's connected
                    if (client_page.available()) {        // if there's bytes to read from the client,
                        char c = client_page.read();        // read a byte, then
                        Serial.print(c);                             // print it out the serial monitor
                        if (c == '\n') {                    // if the byte is a newline character

                          // if the current line is blank, you got two newline characters in a row.
                          // that's the end of the client HTTP request, so send a response:
                            if (currentLine.length() == 0) {

                                make_HTML01();  // create Page array
                               //---------------------------------------------------------------------
                               // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                               // and a content-type so the client knows what's coming, then a blank line:
                                strcpy(HTTP_Header, "HTTP/1.1 200 OK\r\n");
                                strcat(HTTP_Header, "Content-Length: ");
                                strcati(HTTP_Header, strlen(HTML_String));
                                strcat(HTTP_Header, "\r\n");
                                strcat(HTTP_Header, "Content-Type: text/html\r\n");
                                strcat(HTTP_Header, "Connection: close\r\n");
                                strcat(HTTP_Header, "\r\n");

                                client_page.print(HTTP_Header);
                                delay(20);
                                send_HTML();

                                // break out of the while loop:
                                break;
                            }
                            else {    // if you got a newline, then clear currentLine:
                                currentLine = "";
                            }
                        }
                        else if (c != '\r')
                        { // if you got anything else but a carriage return character,
                            currentLine += c;      // add it to the end of the currentLine
                            if (currentLine.endsWith("HTTP"))
                            {
                                if (currentLine.startsWith("GET "))
                                {
                                    currentLine.toCharArray(HTML_String, currentLine.length());
                                    Serial.println(); //NL
                                    exhibit("Request : ", HTML_String);
                                    process_Request();
                                }
                            }
                        }//end else
                    } //end client available
                } //end while client.connected
                // close the connection:
                client_page.stop();
                Serial.print("Pagelength : ");
                Serial.print((long)strlen(HTML_String));
                Serial.print("   --> Client Disconnected\n");
            }// end if client 
    //    }
   // }
}

//-------------------------------------------------------------------------------------------------
// Process given values
//-------------------------------------------------------------------------------------------------
void process_Request()
{
    int myIndex;

    if (Find_Start("/?", HTML_String) < 0 && Find_Start("GET / HTTP", HTML_String) < 0)
    {
        //nothing to process
        return;
    }
    action = Pick_Parameter_Zahl("ACTION=", HTML_String);

    if (action != ACTION_SET_RESTART) { EEprom_unblock_restart(); }
    if (action == ACTION_SET_loadDefault) {
        EEprom_read_default();
        delay(5);
    }
    // WiFi access data
    if (action == ACTION_SET_SSID) {

        myIndex = Find_End("SSID_MY=", HTML_String);
        if (myIndex >= 0) {
            for (int i = 0; i < 24; i++) SCSet.ssid[i] = 0x00;
            Pick_Text(SCSet.ssid, &HTML_String[myIndex], 24);
        }
        myIndex = Find_End("Password_MY=", HTML_String);
        if (myIndex >= 0) {
            for (int i = 0; i < 24; i++) SCSet.password[i] = 0x00;
            Pick_Text(SCSet.password, &HTML_String[myIndex], 24);           
        } 
        int tempint = Pick_Parameter_Zahl("timeoutRout=", HTML_String);
        if ((tempint >= 20) && (tempint <= 1000)) { SCSet.timeoutRouter = tempint; }
        EEprom_write_all();
    }
    if (action == ACTION_SET_SSID2) {

        myIndex = Find_End("SSID2_MY=", HTML_String);
        if (myIndex >= 0) {
            for (int i = 0; i < 24; i++) SCSet.ssid2[i] = 0x00;
            Pick_Text(SCSet.ssid2, &HTML_String[myIndex], 24);
        }
        myIndex = Find_End("Password2_MY=", HTML_String);
        if (myIndex >= 0) {
            for (int i = 0; i < 24; i++) SCSet.password2[i] = 0x00;
            Pick_Text(SCSet.password2, &HTML_String[myIndex], 24);
        }
        int tempint = Pick_Parameter_Zahl("timeoutRout2=", HTML_String);
        if ((tempint >= 20) && (tempint <= 1000)) { SCSet.timeoutRouter2 = tempint; }
        EEprom_write_all();
    }
    if (action == ACTION_SET_Msg) {
        bool noMsg = true;
        //int temp = Pick_Parameter_Zahl("AOGNTRIP=", HTML_String);
        if (Pick_Parameter_Zahl("useUSB=", HTML_String) == 1) { SCSet.DataTransViaUSB = 1; noMsg = false; }
            else { SCSet.DataTransViaUSB = 0; }
        if (Pick_Parameter_Zahl("useUDP=", HTML_String) == 1) { SCSet.DataTransViaWiFi = 1; noMsg = false; }
        else { SCSet.DataTransViaWiFi = 0; }
        if (noMsg) { SCSet.DataTransViaUSB = 1; }
        if (SCSet.debugmode) {
            Serial.println();
            Serial.print("Send Data to "); 
            if (SCSet.DataTransViaUSB == 1) { Serial.print("USB "); } 
            if (SCSet.DataTransViaWiFi == 1) {Serial.print(" WiFi ");}
            Serial.println();
            delay(5);
        }
        EEprom_write_all();
    }
  /*  if (action == ACTION_SET_DataTransfVia) {
        //int temp = Pick_Parameter_Zahl("AOGNTRIP=", HTML_String);
        if (Pick_Parameter_Zahl("DataTransfVia=", HTML_String) == 0) SCSet.DataTransVia = 0;
        if (Pick_Parameter_Zahl("DataTransfVia=", HTML_String) == 1) SCSet.DataTransVia = 1;
        if (SCSet.debugmode) {
            Serial.println();
            Serial.print("Set AOGNTRIP to "); Serial.println(SCSet.AOGNtrip);
            Serial.println();
            delay(500);
        }
        EEprom_write_all();
    }*/
	if (action == ACTION_SET_WiFiLEDon) {
		//int temp = Pick_Parameter_Zahl("AOGNTRIP=", HTML_String);
		if (Pick_Parameter_Zahl("WiFiLEDon=", HTML_String) == 0) {
			SCSet.LEDWiFi_ON_Level = 0;
			if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, 0); }
			else { digitalWrite(SCSet.LEDWiFi_PIN, 1); }
		}
		if (Pick_Parameter_Zahl("WiFiLEDon=", HTML_String) == 1) {
			SCSet.LEDWiFi_ON_Level = 1;
			if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, 1); }
			else { digitalWrite(SCSet.LEDWiFi_PIN, 0); }
		}
		EEprom_write_all();
	}
/*    if (action == ACTION_SET_GPIO) {
        //Serial.println(Pick_Parameter_Zahl("LED=", HTML_String));
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 2) { SCSet.LEDWiFi_PIN = 2; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 4) { SCSet.LEDWiFi_PIN = 4; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 5) { SCSet.LEDWiFi_PIN = 5; pinMode(SCSet.LEDWiFi_PIN, OUTPUT);  if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); }EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 13) { SCSet.LEDWiFi_PIN = 13; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 14) { SCSet.LEDWiFi_PIN = 14; pinMode(SCSet.LEDWiFi_PIN, OUTPUT);  if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); }EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 15) { SCSet.LEDWiFi_PIN = 15; pinMode(SCSet.LEDWiFi_PIN, OUTPUT);  if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); }EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 16) { SCSet.LEDWiFi_PIN = 16; pinMode(SCSet.LEDWiFi_PIN, OUTPUT);  if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); }EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 17) { SCSet.LEDWiFi_PIN = 17; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 18) { SCSet.LEDWiFi_PIN = 18; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 19) { SCSet.LEDWiFi_PIN = 19; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 20) { SCSet.LEDWiFi_PIN = 20; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 21) { SCSet.LEDWiFi_PIN = 21; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 22) { SCSet.LEDWiFi_PIN = 22; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 23) { SCSet.LEDWiFi_PIN = 23; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 24) { SCSet.LEDWiFi_PIN = 24; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 25) { SCSet.LEDWiFi_PIN = 25; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 26) { SCSet.LEDWiFi_PIN = 26; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 27) { SCSet.LEDWiFi_PIN = 27; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 28) { SCSet.LEDWiFi_PIN = 28; pinMode(SCSet.LEDWiFi_PIN, OUTPUT);  if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); }EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 29) { SCSet.LEDWiFi_PIN = 29; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 30) { SCSet.LEDWiFi_PIN = 30; pinMode(SCSet.LEDWiFi_PIN, OUTPUT);  if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); }EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 31) { SCSet.LEDWiFi_PIN = 31; pinMode(SCSet.LEDWiFi_PIN, OUTPUT);  if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); }EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 32) { SCSet.LEDWiFi_PIN = 32; pinMode(SCSet.LEDWiFi_PIN, OUTPUT); if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); } EEprom_write_all(); }
        if (Pick_Parameter_Zahl("LED=", HTML_String) == 33) { SCSet.LEDWiFi_PIN = 33; pinMode(SCSet.LEDWiFi_PIN, OUTPUT);  if (LED_WIFI_ON) { digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level); }EEprom_write_all(); }
        //Serial.println();
        //Serial.print("set pin for WiFi LED to: "); Serial.println(SCSet.LEDWiFi_PIN); Serial.println();
        //delay(500);
    } */
	if (action == ACTION_SET_RESTART) {
		EEprom_block_restart();//prevents from restarting, when webpage is reloaded. Is set to 0, when other ACTION than restart is called
		delay(1000);
#if HardwarePlatform == 0
        WiFi.disconnect();
        delay(500);
        ESP.restart();
#endif
#if HardwarePlatform == 1
        WiFi.end();
        delay(2000);
        Serial.println("restarting WiFi");
		WiFi_Start_STA(1);
		delay(200);
		if (my_WiFi_Mode == 0) { WiFi_Start_STA(2); }
		delay(200);
		if (my_WiFi_Mode == 0) {// if failed start AP
			WiFi_Start_AP();
			delay(100);
		}
		delay(200);
#endif
	}
}

//-------------------------------------------------------------------------------------------------
// HTML Seite 01 aufbauen
//-------------------------------------------------------------------------------------------------
void make_HTML01() {

    strcpy(HTML_String, "<!DOCTYPE html>");
    strcat(HTML_String, "<html>");
    strcat(HTML_String, "<head>");
    strcat(HTML_String, "<title>Section Control Config Page</title>");
    strcat(HTML_String, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0;\" />\r\n");
    //strcat( HTML_String, "<meta http-equiv=\"refresh\" content=\"10\">");
    strcat(HTML_String, "<style>divbox {background-color: lightgrey;width: 200px;border: 5px solid red;padding:10px;margin: 10px;}</style>");
    strcat(HTML_String, "</head>");
    strcat(HTML_String, "<body bgcolor=\"#ccff66\">");//ff9900
    strcat(HTML_String, "<font color=\"#000000\" face=\"VERDANA,ARIAL,HELVETICA\">");
    strcat(HTML_String, "<h1>Section control setup</h1>");
    strcat(HTML_String, "Section control software for AgOpenGPS <br>");
    strcat(HTML_String, "supports data via USB / WiFi UDP <br>");
    strcat(HTML_String, "more settings like IPs, UPD ports... in setup zone of INO code<br>");
    strcat(HTML_String, "(Rev. 3.0 by MTZ8302 Webinterface by WEDER)<br><br><hr>");


    //---------------------------------------------------------------------------------------------  
    //load values of INO setup zone
    strcat(HTML_String, "<h2>Load default values of INO setup zone</h2>");
    strcat(HTML_String, "<form>");
    strcat(HTML_String, "<table>");
    set_colgroup(270, 250, 150, 0, 0);

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"2\">Only load default values, does NOT save them</td>");
    strcat(HTML_String, "<td><button style= \"width:150px\" name=\"ACTION\" value=\"");
    strcati(HTML_String, ACTION_SET_loadDefault);
    strcat(HTML_String, "\">Load default values</button></td>");
    strcat(HTML_String, "</tr>");
    strcat(HTML_String, "</table>");
    strcat(HTML_String, "</form>");
    strcat(HTML_String, "<br><hr>");


    //-----------------------------------------------------------------------------------------
    // WiFi Client Access Data
    strcat( HTML_String, "<h2>#1 WiFi Network Client Access Data</h2>");
    strcat(HTML_String, "<form>");
    strcat(HTML_String, "If access fails, the WiFi network #2 will be tryed<br><b>");
    strcat(HTML_String,"<br><br><table>");
    set_colgroup(250, 300, 150, 0, 0);

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td><b>#1 Network SSID:</b></td>");
    strcat(HTML_String, "<td>");
    strcat(HTML_String, "<input type=\"text\" style= \"width:200px\" name=\"SSID_MY\" maxlength=\"22\" Value =\"");
    strcat(HTML_String, SCSet.ssid);
    strcat(HTML_String, "\"></td>");

    strcat(HTML_String, "<td><button style= \"width:120px\" name=\"ACTION\" value=\"");
    strcati(HTML_String, ACTION_SET_SSID);
    strcat(HTML_String, "\">Apply and Save</button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td><b>#1 Password:</b></td>");
    strcat(HTML_String, "<td>");
    strcat(HTML_String, "<input type=\"text\" style= \"width:200px\" name=\"Password_MY\" maxlength=\"22\" Value =\"");
    strcat(HTML_String, SCSet.password);
    strcat(HTML_String, "\"></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr><td colspan=\"3\">&nbsp;</td></tr>");
    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"3\"><b></b>time, trying to connect to network #1</td></tr>");
    strcat(HTML_String, "<td colspan=\"3\">after time has passed, network #2 is tryed</td></tr>");
    strcat(HTML_String, "<tr><td><b>#1 Timeout (s):</td><td><input type = \"number\"  name = \"timeoutRout\" min = \"20\" max = \"1000\" step = \"1\" style= \"width:200px\" value = \"");// placeholder = \"");
    strcati(HTML_String, SCSet.timeoutRouter);
    strcat(HTML_String, "\"></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");
    strcat(HTML_String, "<tr><td colspan=\"2\"><b>Restart WiFi client for changes to take effect</b></td>");
    strcat(HTML_String, "<td><button style= \"width:120px\" name=\"ACTION\" value=\"");
    strcati(HTML_String, ACTION_SET_RESTART);
    strcat(HTML_String, "\">Restart</button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "</table>");
    strcat(HTML_String, "</form>");
    strcat(HTML_String, "<br><hr>");

    //---------------------------------------------------------------------------------------------         
    // WiFi Client Access Data
    strcat( HTML_String, "<h2>#2 WiFi Network Client Access Data</h2>");
    strcat(HTML_String, "<form>");
    strcat(HTML_String, "</b>If access to both networks fails, an accesspoint will be created:<br>SSID: <b>");
    strcat(HTML_String, SCSet.ssid_ap);
    strcat(HTML_String, "</b>     with no password<br><br><table>");
    set_colgroup(250, 300, 150, 0, 0);

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td><b>#2 Network SSID:</b></td>");
    strcat(HTML_String, "<td>");
    strcat(HTML_String, "<input type=\"text\" style= \"width:200px\" name=\"SSID2_MY\" maxlength=\"22\" Value =\"");
    strcat(HTML_String, SCSet.ssid2);
    strcat(HTML_String, "\"></td>");

    strcat(HTML_String, "<td><button style= \"width:120px\" name=\"ACTION\" value=\"");
    strcati(HTML_String, ACTION_SET_SSID2);
    strcat(HTML_String, "\">Apply and Save</button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td><b>#2 Password:</b></td>");
    strcat(HTML_String, "<td>");
    strcat(HTML_String, "<input type=\"text\" style= \"width:200px\" name=\"Password2_MY\" maxlength=\"22\" Value =\"");
    strcat(HTML_String, SCSet.password2);
    strcat(HTML_String, "\"></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");
    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"3\">time, trying to connect to network #2</td></tr>");
    strcat(HTML_String, "<td colspan=\"3\">after time has passed access point is opened</td></tr>");
    strcat(HTML_String, "<tr><td><b>#2 Timeout (s):</td><td><input type = \"number\"  name = \"timeoutRout2\" min = \"20\" max = \"1000\" step = \"1\" style= \"width:200px\" value = \"");// placeholder = \"");
    strcati(HTML_String, SCSet.timeoutRouter2);
    strcat(HTML_String, "\"></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");
    strcat(HTML_String, "<tr><td colspan=\"2\"><b>Restart WiFi client for changes to take effect</b></td>");
    strcat(HTML_String, "<td><button style= \"width:120px\" name=\"ACTION\" value=\"");
    strcati(HTML_String, ACTION_SET_RESTART);
    strcat(HTML_String, "\">Restart</button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "</table>");
    strcat(HTML_String, "</form>");
    strcat(HTML_String, "<br><hr>");

    //---------------------------------------------------------------------------------------------  
 
    // Checkboxes Messages
    strcat(HTML_String, "<h2>communication AgOpenGPS - Section control via</h2>");
    strcat(HTML_String, "<form>");
    strcat(HTML_String, "<table>");
    set_colgroup(300, 250, 150, 0, 0);

	strcat(HTML_String, "<tr>");
	strcat(HTML_String, "<td></td><td><input type=\"checkbox\" name=\"useUSB\" id = \"Part\" value = \"1\" ");
	if (SCSet.DataTransViaUSB == 1) strcat(HTML_String, "checked ");
	strcat(HTML_String, "> ");
	strcat(HTML_String, "<label for =\"Part\">USB</label>");
	strcat(HTML_String, "</td>");    
    strcat(HTML_String, "<td><button style= \"width:120px\" name=\"ACTION\" value=\"");
    strcati(HTML_String, ACTION_SET_Msg);
    strcat(HTML_String, "\">Apply and Save</button></td>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td><input type=\"checkbox\" name=\"useUDP\" id = \"Part\" value = \"1\" ");
    if (SCSet.DataTransViaWiFi == 1) strcat(HTML_String, "checked ");
    strcat(HTML_String, "> ");
    strcat(HTML_String, "<label for =\"Part\"> WiFi UDP</label>");
    strcat(HTML_String, "</td></tr>");

    strcat(HTML_String, "</table>");
    strcat(HTML_String, "</form>");
    strcat(HTML_String, "<br><hr>");
    
 /*   //---------------------------------------------------------------------------------------------  
    //---------------------------------------------------------------------------------------------  
    // WiFi LED light on high/low 
    strcat(HTML_String, "<h2>WiFi LED light on</h2>");
    strcat(HTML_String, "<form>");
    strcat(HTML_String, "<table>");
    set_colgroup(150, 270, 150, 0, 0);

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td><input type = \"radio\" name=\"WiFiLEDon\" id=\"JZ\" value=\"0\"");
    if (SCSet.LEDWiFi_ON_Level == 0)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"JZ\">LOW</label></td>");
    strcat(HTML_String, "<td><button style= \"width:120px\" name=\"ACTION\" value=\"");
    strcati(HTML_String, ACTION_SET_WiFiLEDon);
    strcat(HTML_String, "\">Apply and Save</button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td><input type = \"radio\" name=\"WiFiLEDon\" id=\"JZ\" value=\"1\"");
    if (SCSet.LEDWiFi_ON_Level == 1)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"JZ\">HIGH</label></td></tr>");

    strcat(HTML_String, "</table>");
    strcat(HTML_String, "</form>");
    strcat(HTML_String, "<br><hr>");

    //---------------------------------------------------------------------------------------------  
    // WiFi LED PINs selection
    strcat(HTML_String, "<h2>WiFi LED pin setting</h2>");
    strcat(HTML_String, "<br>");

    strcat(HTML_String, "<b>select # of GPIO pin at ESP32</b><br>");
    strcat(HTML_String, "<form>");
    strcat(HTML_String, "<table>");
    set_colgroup(100, 100, 100, 100, 200);
    strcat(HTML_String, "<tr><td>");
    strcat(HTML_String, "<button style= \"width:120px\" name=\"ACTION\" value=\"");
    strcati(HTML_String, ACTION_SET_GPIO);
    strcat(HTML_String, "\">Apply and Save</button></td></tr>");
    strcat(HTML_String, "<tr><td><b>pin for WiFi indication LED</b></td>");
    strcat(HTML_String, "</tr>");
    for (int i = 2; i < 34; i++) {
        //skip not usabel GPIOs
        if (i == 3) { i++; } //3: 6-11: not use! USB 12: ESP wouldn't boot
        if (i == 6) { i = 13; }

        strcat(HTML_String, "<tr>");
        strcat(HTML_String, "<td><input type = \"radio\" name=\"LED\" id=\"GPIOLED\" value=\"");
        strcati(HTML_String, i);
        strcat(HTML_String, "\"");
        if (SCSet.LEDWiFi_PIN == i) { strcat(HTML_String, " CHECKED"); }
        strcat(HTML_String, "><label for=\"JZ");
        strcati(HTML_String, i);
        strcat(HTML_String, "\">");
        strcati(HTML_String, i);
        strcat(HTML_String, "</label></td>");

        strcat(HTML_String, "</tr>");
    }
    strcat(HTML_String, "</table>");
    strcat(HTML_String, "</form>");
*/}
//--------------------------------------------------------------------------
void send_not_found() {

	Serial.print("\nSend Not Found\n");

	client_page.print("HTTP/1.1 404 Not Found\r\n\r\n");
	delay(20);
	//client_page.stop();
}

//--------------------------------------------------------------------------
void send_HTML() {
	char my_char;
	int  my_len = strlen(HTML_String);
	int  my_ptr = 0;
	int  my_send = 0;

	//--------------------------------------------------------------------------
	// in Portionen senden
	while ((my_len - my_send) > 0) {
		my_send = my_ptr + MAX_PACKAGE_SIZE;
		if (my_send > my_len) {
			client_page.print(&HTML_String[my_ptr]);
			delay(10);

			//Serial.println(&HTML_String[my_ptr]);

			my_send = my_len;
		}
		else {
			my_char = HTML_String[my_send];
			// Auf Anfang eines Tags positionieren
			while (my_char != '<') my_char = HTML_String[--my_send];
			HTML_String[my_send] = 0;
			client_page.print(&HTML_String[my_ptr]);
			delay(10);

			//Serial.println(&HTML_String[my_ptr]);

			HTML_String[my_send] = my_char;
			my_ptr = my_send;
		}
	}
	//client_page.stop();
}

//----------------------------------------------------------------------------------------------
void set_colgroup(int w1, int w2, int w3, int w4, int w5) {
	strcat(HTML_String, "<colgroup>");
	set_colgroup1(w1);
	set_colgroup1(w2);
	set_colgroup1(w3);
	set_colgroup1(w4);
	set_colgroup1(w5);
	strcat(HTML_String, "</colgroup>");

}
//------------------------------------------------------------------------------------------
void set_colgroup1(int ww) {
	if (ww == 0) return;
	strcat(HTML_String, "<col width=\"");
	strcati(HTML_String, ww);
	strcat(HTML_String, "\">");
}


//---------------------------------------------------------------------
void strcatf(char* tx, float f, byte leng, byte dezim) {
	char tmp[8];

//	dtostrf(f, leng, dezim, tmp);//f,6,2,tmp
	strcat(tx, tmp);
}
//---------------------------------------------------------------------
//void strcatl(char* tx, long l) {
  //char tmp[sizeof l];
  //memcpy(tmp, l, sizeof l);
  //strcat (tx, tmp);
//}

//---------------------------------------------------------------------
void strcati(char* tx, int i) {
	char tmp[8];

	itoa(i, tmp, 10);
	strcat(tx, tmp);
}

//---------------------------------------------------------------------
void strcati2(char* tx, int i) {
	char tmp[8];

	itoa(i, tmp, 10);
	if (strlen(tmp) < 2) strcat(tx, "0");
	strcat(tx, tmp);
}

//---------------------------------------------------------------------
int Pick_Parameter_Zahl(const char* par, char* str) {
	int myIdx = Find_End(par, str);

	if (myIdx >= 0) return  Pick_Dec(str, myIdx);
	else return -1;
}
//---------------------------------------------------------------------
int Find_End(const char* such, const char* str) {
	int tmp = Find_Start(such, str);
	if (tmp >= 0)tmp += strlen(such);
	return tmp;
}

//---------------------------------------------------------------------
int Find_Start(const char* such, const char* str) {
	int tmp = -1;
	int ww = strlen(str) - strlen(such);
	int ll = strlen(such);

	for (int i = 0; i <= ww && tmp == -1; i++) {
		if (strncmp(such, &str[i], ll) == 0) tmp = i;
	}
	return tmp;
}
//---------------------------------------------------------------------
int Pick_Dec(const char* tx, int idx) {
	int tmp = 0;

	for (int p = idx; p < idx + 5 && (tx[p] >= '0' && tx[p] <= '9'); p++) {
		tmp = 10 * tmp + tx[p] - '0';
	}
	return tmp;
}
//----------------------------------------------------------------------------
int Pick_N_Zahl(const char* tx, char separator, byte n) {//never used?

	int ll = strlen(tx);
	int tmp = -1;
	byte anz = 1;
	byte i = 0;
	while (i < ll && anz < n) {
		if (tx[i] == separator)anz++;
		i++;
	}
	if (i < ll) return Pick_Dec(tx, i);
	else return -1;
}

//---------------------------------------------------------------------
int Pick_Hex(const char* tx, int idx) {
	int tmp = 0;

	for (int p = idx; p < idx + 5 && ((tx[p] >= '0' && tx[p] <= '9') || (tx[p] >= 'A' && tx[p] <= 'F')); p++) {
		if (tx[p] <= '9')tmp = 16 * tmp + tx[p] - '0';
		else tmp = 16 * tmp + tx[p] - 55;
	}

	return tmp;
}

//---------------------------------------------------------------------
void Pick_Text(char* tx_ziel, char* tx_quelle, int max_ziel) {

	int p_ziel = 0;
	int p_quelle = 0;
	int len_quelle = strlen(tx_quelle);

	while (p_ziel < max_ziel && p_quelle < len_quelle && tx_quelle[p_quelle] && tx_quelle[p_quelle] != ' ' && tx_quelle[p_quelle] != '&') {
		if (tx_quelle[p_quelle] == '%') {
			tx_ziel[p_ziel] = (HexChar_to_NumChar(tx_quelle[p_quelle + 1]) << 4) + HexChar_to_NumChar(tx_quelle[p_quelle + 2]);
			p_quelle += 2;
		}
		else if (tx_quelle[p_quelle] == '+') {
			tx_ziel[p_ziel] = ' ';
		}
		else {
			tx_ziel[p_ziel] = tx_quelle[p_quelle];
		}
		p_ziel++;
		p_quelle++;
	}

	tx_ziel[p_ziel] = 0;
}
//---------------------------------------------------------------------
char HexChar_to_NumChar(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'F') return c - 55;
	return 0;
}
//---------------------------------------------------------------------
void exhibit(const char* tx, int v) {
	Serial.print(tx);
	Serial.print(v, 1);
}
//---------------------------------------------------------------------
void exhibit(const char* tx, unsigned int v) {
	Serial.print(tx);
	Serial.print((int)v, 1);
}
//---------------------------------------------------------------------
void exhibit(const char* tx, unsigned long v) {
	Serial.print(tx);
	Serial.print((long)v, 1);
}
//---------------------------------------------------------------------
void exhibit(const char* tx, const char* v) {
	Serial.print(tx);
	Serial.print(v);
}
