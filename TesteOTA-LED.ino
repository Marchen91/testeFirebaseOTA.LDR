#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <FirebaseESP8266.h>  


#ifndef STASSID
#define STASSID "BRAGA"
#define STAPSK  "braga2617"
#endif

#define FIREBASE_HOST "exemplofirebase-15a09-default-rtdb.firebaseio.com"              // the project name address from firebase id
#define FIREBASE_AUTH "D5y5X4SUKY7HXgcWgoeY3njFXeEOxqkIGI4TndDX"       // the secret key generated from firebase
#define AnalogLDR A0 // define pino analógico A0
#define LimiarMin 0.8 
#define LimiarMax 1.2 // define constante igual a 1.5
#define ledPin 15 // define pino digital D8

const char* ssid = STASSID;
const char* password = STAPSK;



int Leitura = 0; // variavel inteiro igual a zero
float VoltageLDR; // variável número fracionário
float ResLDR; // variável número fracionário

String fireStatus = "";                                                     // led status received from firebase

float val1 = 0;
float val2 = 0;
String led= "";

FirebaseData firebaseData;


void setup() {

  pinMode(ledPin, OUTPUT); // configura D15 como saída digital
  Serial.begin(115200);
  delay(100); // atraso de 100 milissegundos
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("esp8266");

  // No authentication by default
  ArduinoOTA.setPassword("123456");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); 
}

void loop() {
    ArduinoOTA.handle();

    Leitura = analogRead(AnalogLDR); // leitura da tensão no pino analogico A0
    VoltageLDR = Leitura * (3.3/1024); // cálculo da tensão no LDR
    Serial.print("Leitura sensor LDR = "); // imprime no monitor serial
    Serial.println(VoltageLDR); // imprime a tensão do LDR

    


    //Leitura valores min e max Real time DB
    if (Firebase.getInt(firebaseData, "/min")) {                           // On successful Read operation, function returns 1  
  
      if (firebaseData.dataType() == "float") {                            // print read data if it is integer
  
        val1 = firebaseData.floatData();
      }
  }
  else {
      Serial.println(firebaseData.errorReason());
    }


      if (Firebase.getInt(firebaseData, "/max")) {                           // On successful Read operation, function returns 1  
  
      if (firebaseData.dataType() == "float") {                            // print read data if it is integer
  
        val2 = firebaseData.floatData();
      }
      }
     else {
      Serial.println(firebaseData.errorReason());
    }

        Serial.println(val1); 
        Serial.println(val2); 
        
   

  //Função clássica do LDR- min e max -> Firebase RealTime
    if (VoltageLDR > val1 & VoltageLDR < val2){ // se a tensão LDR maior do que limiar
    digitalWrite(ledPin, HIGH);
    led = "ON";
    Serial.println("LED ON"); } // liga LED com 3.3V
    else // senão a tensão LDR < limiar
    {digitalWrite(ledPin, LOW);
    led = "OFF";
    Serial.println("LED OFF");} // desliga LED com 0V
    
    
    delay(500); // atraso de 500 milisegundos*/

    //Escrita valor Analogico LDR Firebase RealTime DB
     if (Firebase.setInt(firebaseData, "/LDR", VoltageLDR)) {
      Serial.println("Value LDR Uploaded Successfully");
      }
    else {        
    Serial.println(firebaseData.errorReason());
  }   
  


  //Escrita no valor led Firebase RealTime DB
  if (Firebase.setString(firebaseData, "/led", led)) {
      Serial.println("Value LDR Uploaded Successfully");
      }
    else {        
    Serial.println(firebaseData.errorReason());
  }


    
}
