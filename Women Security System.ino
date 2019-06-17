 #include <SoftwareSerial.h>
 #include <TinyGPS.h>
TinyGPS gps;

SoftwareSerial mySerial(7, 8);//(rx,tx)//GSM
SoftwareSerial ss(4, 3); //GPS

int button;

void setup()
{
  mySerial.begin(4800);   // Setting the baud rate of GSM Module
  ss.begin(4800);
  Serial.begin(4800);    // Setting the baud rate of Serial Monitor (Arduino)
  delay(100);
  pinMode(6,INPUT);
}


void loop()
{
 button=digitalRead(6);
 if(button == 0)
 {
  Serial.println("Button Pressed");
  sendlocation();
  SendMessage();
 }
}


 void SendMessage()
{
  Serial.println(".............................");
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+918885544987\"\r"); // Replace x with mobile number
  delay(1000);
  mySerial.println("I'm in need ");// The SMS text you want to send
  delay(100);
  mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}


 void RecieveMessage()
{
  mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
                                              //AT+CNMI=1,1,0,0,0 Set the new message indicators.(CNMI=command name in text)

                                            //AT+CNMI=<mode>,<mt>,<bm>,<ds>,<bfr>

//                                              <mode>=1 discard unsolicited result codes indication when TA – TE link is reserved.(TE=terminal equipment)
//
//                                                        <mt>=1 SMS-DELIVERs are delivered to the SIM and routed using unsolicited code.
//
//                                                                    <bm>=0 no CBM indications are routed to the TE.
//
//                                                                      <ds>=0 no SMS-STATUS-REPORTs are routed.

//                                                                            <bfr>=0 TA buffer of unsolicited result codes defined within this command is flushed to the TE.
                                         
  delay(1000);
 }

 void sendlocation()
 {
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
  }
  
  gps.stats(&chars, &sentences, &failed);
  Serial.print(" CHARS=");
  Serial.print(chars);
  Serial.print(" SENTENCES=");
  Serial.print(sentences);
  Serial.print(" CSUM ERR=");
  Serial.println(failed);
  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");
}
