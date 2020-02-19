void getDataFromAOG()
{
    byte incomingBytes[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 }, incomingByteNum = 0, tempByt = 0;
    int tempInt = 0;
    float tempFlo = 0;
   // bool steerSettingChanged = false;
    isDataFound = false;
    isSettingFound = false;

    //get new AOG Data
    //USB and USB if nothing else selected
    if ((SCSet.DataTransViaUSB == 1) || (SCSet.DataTransViaWiFi == 0)) {
        incomingByteNum = 0;

        while (Serial.available())
        {
            incomingByte = Serial.read();
            if (isDataFound || isSettingFound)
            {
                DataFromAOG[incomingByteNum] = incomingByte;
                incomingByteNum++;
                if (SCSet.debugmodeDataFromAOG) {
                    Serial.print(incomingByte); Serial.print(" ");
                }
                if (incomingByteNum > 10) { break; }//sentence too long
            }
            else {//sentence not started yet
                if ((incomingByte == SCSet.DataFromAOGHeader[0]) || (incomingByte == SCSet.SettingsFromAOGHeader[0]))
                {
                    incomingByte = Serial.read();
                    if (incomingByte == SCSet.DataFromAOGHeader[1]) { isDataFound = true; }
                    if (incomingByte == SCSet.SettingsFromAOGHeader[1]) { isSettingFound = true; }
                    if (SCSet.debugmodeDataFromAOG) { Serial.print("data from AOG via USB: "); }
                }
            }
        }
    }//end USB


    //WiFi UDP 
    if ((SCSet.DataTransViaWiFi == 1))
    {
        //Serial.println("checking for UDP packet");
        isDataFound = false;
        isSettingFound = false;

        byte leng = UDPFromAOG.parsePacket();
        //check packet length and process only fitting ones
        if (leng > 0 && SCSet.debugmodeDataFromAOG) { Serial.println("UDP packet found"); }
        if ((leng >= 6) && (leng <= 12))
        {
            UDPFromAOG.read(incomingBytes, leng);
            if ((incomingBytes[0] == SCSet.DataFromAOGHeader[0]) && (incomingBytes[1] == SCSet.DataFromAOGHeader[1]))
            {
                isDataFound = true;
            }
            if ((incomingBytes[0] == SCSet.SettingsFromAOGHeader[0]) && (incomingBytes[1] == SCSet.SettingsFromAOGHeader[1]))
            {
                isSettingFound = true;
            }
            if (SCSet.debugmodeDataFromAOG) { Serial.print("data from AOG via UDP: "); }
            for (byte n = 0; n < leng; n++) {
                if (SCSet.debugmodeDataFromAOG) { Serial.print(incomingBytes[n]); Serial.print(" "); }
                DataFromAOG[n] = incomingBytes[n];
            }
            if (SCSet.debugmodeDataFromAOG) { Serial.println(); }
        }

    }//end UDP


    //DATA Header 
    if (isDataFound)  //Do we have a match?
    {
        RelayFromAOG[1] = DataFromAOG[2];   // read relay control from AgOpenGPS
        RelayFromAOG[0] = DataFromAOG[3];
        groundSpeed = DataFromAOG[4] >> 2;  //actual speed times 4, single byte
        uTurnRelay = DataFromAOG[5];
        //[6] tree
        //[7] hydraulik lift
        //reset watchdog as we just heard from AgOpenGPS
        watchdogTimer = 0;
        DataFromAOGTime = millis();
     /*   if (SCSet.debugmodeDataFromAOG) {
            Serial.print("got data from AOG:  ");
            for (byte i = 2; i < SCSet.DataFromAOGLength; i++)
            {
                Serial.print(DataFromAOG[i]);
                Serial.print(",");
            }
            Serial.println();
        }*/
    }

    //SETTINGS Header has been found, 6 bytes are the settings
    if (isSettingFound)
    {
        //accumulated volume, 0 it if this is 32700 sent
        float tempf = (float)(DataFromAOG[2] << 8 | DataFromAOG[3]);   //high,low bytes
        if (tempf == 32700) accumulatedCountsDual = 0;

        //flow meter cal factor in counts per Liter
        flowmeterCalFactorLeft = ((float)(DataFromAOG[4] << 8 | DataFromAOG[5]));   //high,low bytes
        flowmeterCalFactorRight = ((float)(DataFromAOG[6] << 8 | DataFromAOG[7]));   //high,low bytes
       /* if (SCSet.debugmodeDataFromAOG) {
            Serial.print("got Setting from AOG:  ");
            for (byte i = 2; i < SCSet.SettingsFromAOGLength; i++)
            {
                Serial.print(DataFromAOG[i]);
                Serial.print(",");
            }
            Serial.println();
        }*/
    }

}

//-------------------------------------------------------------------------------------------------

void AOGDataSend()
{
    //USB, or USB if nothing selected
    if ((SCSet.DataTransViaWiFi == 0) || (SCSet.DataTransViaUSB == 1)) {
        for (byte idx = 0; idx < DataToAOGLength; idx++) {
            Serial.write(RelayToAOG[idx]); 
            if (idx < (DataToAOGLength - 1)) { Serial.print(","); }
        }
        Serial.println();
    }

    if (SCSet.DataTransViaWiFi == 1)
    {
        UDPToAOG.beginPacket(SCSet.IPToAOG, SCSet.portDestination);
        for (byte idx = 0; idx < DataToAOGLength; idx++) {
            UDPToAOG.write(RelayToAOG[idx]);
        }
        UDPToAOG.endPacket();  
        
        // UDPToAOG.writeTo(RelayToAOG, DataToAOGLength, IPToAOG, SCSet.portDestination);
    }
}

