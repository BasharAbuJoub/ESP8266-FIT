/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
*/
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <string.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* host = "esp8266-webupdate";
const char* ssid = "Umniah_4E8F76";
const char* password = "2314231423149";

// Set the LCD I2C address
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  

ESP8266WebServer server(80);
const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";



uint8_t gBigFontShapeTable[] PROGMEM  = {
  //* LT[8] =
  B00111,
  B01111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  //* UB[8] =
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  //* RT[8] =
  B11100,
  B11110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  //* LL[8] =
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B01111,
  B00111,
  //* LB[8] =
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  //* LR[8] =
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11110,
  B11100,
  //* UMB[8] =
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  //* LMB[8] =
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111


};



//************************************************************************
//*  6 numbers for each character
//* 9 means BLANK
uint8_t gBigFontAsciiTable[] PROGMEM  = {

  32, 32, 32, 32, 32, 32,   //  0x20  space
  32, 0,  32, 32, 4,  32,   //  0x21  !
  32, 32, 32, 32, 32, 32,   //  0x22
  32, 32, 32, 32, 32, 32,   //  0x23
  32, 32, 32, 32, 32, 32,   //  0x24
  32, 32, 32, 32, 32, 32,   //  0x25
  32, 32, 32, 32, 32, 32,   //  0x26
  32, 32, 32, 32, 32, 32,   //  0x27
  32, 32, 32, 32, 32, 32,   //  0x28
  32, 32, 32, 32, 32, 32,   //  0x29
  32, 32, 32, 32, 32, 32,   //  0x2A
  32, 32, 32, 32, 32, 32,   //  0x2B
  32, 32, 32, 32, 32, 32,   //  0x2C
  32, 32, 32, 32, 32, 32,   //  0x2D
  32, 32, 32, 32, 4,  32,   //  0x2E  . (period)
  32, 32, 32, 32, 32, 32,   //  0x2F


  0,  1,  2,  3,  4,  5,    //  0x30  0
  1,  2,  32, 4, 5,  4,   //  0x31  1
  6,  6,  2,  3,  7,  7,    //  0x32  2
  6,  6,  2,  7,  7,  5,    //  0x33  3
  3,  4,  2,  32, 32, 5,    //  0x34  4
  255, 6,  6,  7,  7,  5,   //  0x35  5
  //  0,  6,  6,  7,  7,  5,    //  0x35  5
  0,  6,  6,  3,  7,  5,    //  0x36  6
  1,  1,  2,  32, 0,  32,   //  0x37  7
  0,  6,  2,  3,  7,  5,    //  0x38  8
  0,  6,  2,  32, 32, 5,    //  0x39  9
  32, 32, 32, 32, 32, 32,   //  0x3A
  32, 32, 32, 32, 32, 32,   //  0x3B
  32, 32, 32, 32, 32, 32,   //  0x3C
  32, 32, 32, 32, 32, 32,   //  0x3D
  32, 32, 32, 32, 32, 32,   //  0x3E
  1,  6,  2,  254, 7,  32,  //  0x3F  ?

  32, 32, 32, 32, 32, 32,   //  0x40  @
  0,  6,  2,  255, 254, 255, //  0x41  A
  255, 6,  5,  255, 7,  2,  //  0x42  B
  0,  1,  1,  3,  4,  4,    //  0x43  C
  255, 1,  2,  255, 4,  5,  //  0x44  D
  255, 6,  6,  255, 7,  7,  //  0x45  E
  255, 6,  6,  255, 32, 32, //  0x46  F

  0,  1,  1,  3,  4,  2,    //  0x47  G
  255, 4,  255, 255, 254, 255, //  0x48  H
  1,  255, 1,  4,  255, 4,  //  0x49  I
  32, 32, 255, 4,  4,  5,   //  0x4A  J
  255, 4,  5,  255, 254, 2, //  0x4B  K
  255, 32, 32, 255, 4,  4,  //  0x4C  L
  32, 32, 32, 32, 32, 32,   //  0x4D  M place holder
  32, 32, 32, 32, 32, 32,   //  0x4E  N place holder
  0,  1,  2,  3,  4,  5,    //  0x4F  O (same as zero)

  0,  6,  2,  3,  32, 32,   //  0x50  P
  32, 32, 32, 32, 32, 32,   //  0x51  Q
  0,  6,  5,  3,  32, 2,    //  0x52  R
  0,  6,  6,  7,  7,  5,    //  0x35  S (same as 5)
  1,  2,  1,  32, 5,  32,   //  0x54  T
  2,  32, 2,  3,  4,  5,    //  0x55  U
  32, 32, 32, 32, 32, 32,   //  0x56  V place holder
  32, 32, 32, 32, 32, 32,   //  0x57  W place holder
  3,  4,  5,  0,  32, 2,    //  0x58  X
  3,  4,  5,  32, 5,  32,   //  0x59  Y
  1,  6,  5,  0,  7,  4,    //  0x5A  Z
  0

};


uint8_t gBigFontAsciiTableWide[] PROGMEM  = {
  //* this table is 10 bytes, 2 rows of 5
  //  ---top------------|
  0,  1,  3,  1,  2,  3,  32, 32, 32, 5,    //  0x4D  M   5-wide
  0,  3,  32, 2,  32, 3,  32, 2,  5,  32,   //  0x4E  N   4-wide
  0,  1,  2,  32, 32, 3,  4,  3,  4,  32,   //  0x51  Q   4-wide
  3,  32, 32, 5,  32, 32, 3,  5,  32, 32,   //  0x56  V   4-wide
  0,  32, 32, 32, 2,  3,  4,  0,  4,  5,    //  0x57  W   5-wide
  0
};




//************************************************************************
static void BigNumber_SendCustomChars(void)
{
  uint8_t customCharDef[10];
  uint8_t ii;
  int   jj;

  for (ii = 0; ii < 8; ii++)
  {
    for (jj = 0; jj < 8; jj++)
    {
      customCharDef[jj] = pgm_read_byte_near(gBigFontShapeTable + (ii * 8) + jj);
    }
    lcd.createChar(ii, customCharDef);
  }
}

//************************************************************************
//* returns the width of the character
static int  DrawBigChar(int xLocation, int yLocation, char theChar)
{
  int   offset;
  int   ii;
  char  theByte;
  boolean specialCase;
  int   specialIndex;
  int   charWidth;

  if (theChar == 0x20)
  {
    return (2);
  }
  else if (theChar < 0x20)
  {
    return (0);
  }

  if (theChar >= 'A')
  {
    theChar = theChar & 0x5F; //* force to upper case
  }
  specialCase = true;
  switch (theChar)
  {
    case 'M': charWidth = 5;  specialIndex  = 0;  break;
    case 'N': charWidth = 4;  specialIndex  = 1;  break;
    case 'Q': charWidth = 4;  specialIndex  = 2;  break;
    case 'V': charWidth = 4;  specialIndex  = 3;  break;
    case 'W': charWidth = 5;  specialIndex  = 4;  break;


    default:
      charWidth = 3;
      specialCase = false;
      offset    = 6 * (theChar - 0x20);
      lcd.setCursor(xLocation, yLocation);
      for (ii = 0; ii < 3; ii++)
      {
        theByte = pgm_read_byte_near(gBigFontAsciiTable + offset + ii);
        lcd.write(theByte);
      }

      lcd.setCursor(xLocation, yLocation + 1);
      offset  +=  3;
      for (ii = 0; ii < 3; ii++)
      {
        theByte = pgm_read_byte_near(gBigFontAsciiTable + offset + ii);
        lcd.write(theByte);
      }
      break;
  }
  if (specialCase)
  {
    //*
    offset    = 10 * specialIndex;
    lcd.setCursor(xLocation, yLocation);
    for (ii = 0; ii < charWidth; ii++)
    {
      theByte = pgm_read_byte_near(gBigFontAsciiTableWide + offset + ii);
      lcd.write(theByte);
    }

    lcd.setCursor(xLocation, yLocation + 1);
    offset  +=  5;
    for (ii = 0; ii < charWidth; ii++)
    {
      theByte = pgm_read_byte_near(gBigFontAsciiTableWide + offset + ii);
      lcd.write(theByte);
    }
  }

  return (charWidth + 1);
}


void handleRoot() {

  server.send(200, "text/plain", "hello from esp8266!");

}



void handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);

}


//---------------------------------------------------------------------- Lines variables ...
String line3 = "";
String line4 = "";
int counter3 = 0;
int counter4 = 0;

unsigned long prevM = 0;
unsigned long prevReq = 0;
long reqSpeed = 1000*60;
long scrollSpeed = 750; //Scrolling speed ..
//------------------------------------



void setup(void){
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println(WiFi.localIP());
  if(WiFi.waitForConnectResult() == WL_CONNECTED){
    MDNS.begin(host);
    server.on("/", handleRoot);
    server.on("/display", []() {
  
      String Line = server.arg(0);
      drawWord( Line.toInt(), server.arg(1));
      server.send(200, "text/plain", "Done");
  
    });
    server.on("/speed", []() {
  
      String line = server.arg(0);
      scrollSpeed = line.toInt();
      Serial.println(scrollSpeed);
      Serial.println(line.toInt());
      server.send(200, "text/plain", "Done");
  
    });
    server.on("/on", []() {
      lcd.backlight();
      server.send(200, "text/plain", "Done");
    });

    server.on("/off", []() {
      lcd.noBacklight();
      server.send(200, "text/plain", "Done");
    });

     server.on("/displaysmall", []() {
      String line = server.arg(0);
      lcd.setCursor(0, line.toInt());
      lcd.print("                    ");
      lcd.setCursor(0, line.toInt());
      lcd.print( server.arg(1).substring(0, 20));
      server.send(200, "text/plain", "Done");
  
  
  
      //ADDED BY BESHO
      if (line.toInt() == 2) {
        line3 = server.arg(1);
        counter3 = 0;
        Serial.println(line3);
      } else if (line.toInt() == 3) {
        line4 = server.arg(1);
        counter4 = 0;
      }
  
      //END ADDED BY BESHO
  
    });
    server.on("/upload", HTTP_GET, [](){
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/html", serverIndex);
    });
    server.on("/update", HTTP_POST, [](){
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
      ESP.restart();
    },[](){
      HTTPUpload& upload = server.upload();
      if(upload.status == UPLOAD_FILE_START){
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)){//start with max available size
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_WRITE){
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_END){
        if(Update.end(true)){ //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      yield();
    });
    server.onNotFound(handleNotFound);
    server.begin();

    IPAddress myIP = WiFi.softAPIP();
    lcd.begin(20, 4);


    BigNumber_SendCustomChars();
    lcd.backlight();
    drawWord( 0, " F I T ");
    lcd.setCursor(0, 2);
    lcd.print("  >Welcome to FIT<  ");
    
    
    MDNS.addService("http", "tcp", 80);
  
    Serial.printf("Ready! Open http://%s.local in your browser\n", host);
  } else {
    Serial.println("WiFi Failed");
  }
  
}



void clearLine(int line)
{

  lcd.setCursor(0, line);
  lcd.print("                    ");
  lcd.setCursor(0, line + 1);
  lcd.print("                    ");
}


void drawWord (int line, String msg) {

  int i = msg.length();
  int x = 0;

  clearLine(line);

  lcd.setCursor(0, line);

  for ( int j = 0 ; j < i ; j++)
  {
    x += DrawBigChar(x, line, msg.charAt(j));
  }

}






void loop()   /*----( LOOP: RUNS CONSTANTLY )----*/
{
  server.handleClient();

      unsigned long currentM = millis();

  //  /----------------------------------------------------- Scrolling Effect
  if (currentM - prevM >= scrollSpeed) {

      prevM = currentM;

    //  Line 3 Scroller ...
    if (line3.length() > 20 ) {

      if (counter3 >= line3.length() - 15) {
        counter3 = 0;
      }



      String line = line3.substring(counter3, counter3 + 20);
      lcd.setCursor(0, 2);
      lcd.print("                    ");
      lcd.setCursor(0, 2);
      lcd.print( line);
      server.send(200, "text/plain", "Done");
      counter3++;

    }else{
      lcd.setCursor(0, 2);
      lcd.print( line3);
    }


    //Line4 Scroller

    if (line4.length() > 20 ) {
      unsigned long currentM = millis();
      if (counter4 >= line4.length() - 15) {
        counter4 = 0;
      }



      String line = line4.substring(counter4, counter4 + 20);
      lcd.setCursor(0, 3);
      lcd.print("                    ");
      lcd.setCursor(0, 3);
      lcd.print( line);
      server.send(200, "text/plain", "Done");
      counter4++;




    }else{
      lcd.setCursor(0, 4);
      lcd.print( line4);
    }


  }

  
  if(currentM - prevReq >= reqSpeed){

  prevReq = currentM;
  Serial.println("SENDING REQUEST");
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    HTTPClient http;  //Declare an object of class HTTPClient
    
    http.begin("http://192.168.1.59:3000/api/overlay/192.168.1.217");  //Specify request destination

    int httpCode = http.GET();                                                                  //Send the request
 
    if (httpCode > 0) { //Check the returning code
 
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload

      StaticJsonBuffer<2000> jsonBuffer;
      String json = payload;
      JsonObject& root = jsonBuffer.parseObject(json);
      if (!root.success()) {

           drawWord( 0, " F I T ");
    lcd.setCursor(0, 2);
    lcd.print("  >Welcome to FIT<  ");
        Serial.println("Parsing failed");
      
       
      }
      String l1 = root["line1"];
      String l2 = root["line2"];
      line3 = l1;
      line4 = l2;
      String bigLine = root["title"];
      drawWord( 0, bigLine);
      lcd.setCursor(0, 2);
      lcd.print("                    ");
      lcd.setCursor(0, 3);
      lcd.print("                    ");
 
    }
 
    http.end();   //Close connection
 
  }
    
  }


}
