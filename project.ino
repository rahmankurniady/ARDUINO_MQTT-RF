
#include <TrueRandom.h>
#include <EEPROM.h>
#include <RCSwitch.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

RCSwitch mySwitch = RCSwitch();

//========================================================
// Update these with values suitable for your network.

IPAddress server(156, 67, 214, 77);
IPAddress ip(192, 168, 3, 253); //INPUT NILAI IP STATIS
long lastReconnectAttempt = 0;

char Conection_name[20] = "testMQTT";
char Username_conn[20] = "rahman";
char Password_conn[20] = "rahman"; 

String ID_Device = "RA100";
char Subscribe[10] = "Command";
char Publish[10] = "RFINPUT";
// =========================================================.


const int W5500_RESET_PIN = 11; 

byte MAC_ADDR[6];
char macstr[18];
const int ledData = 5;
const int ledError = 6; 

String incoming = "";
void callback(char* topic, byte* payload, unsigned int length) {
    payload[length] = '\0';
     String strTopic = String((char*)topic);
     String strSubs = String((char*)Subscribe);
     
    if (strTopic == strSubs) {
      Serial.println((char *)payload);
    }
}

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

boolean reconnect() {
 if (client.connect(Conection_name, Username_conn, Password_conn)) {
    client.subscribe(Subscribe);
  }
  return client.connected();
}
void load_parameter(){
  Serial.println("load Parameter");
     
     // Store MAC address in EEPROM
  if (EEPROM.read(7) == '#') {
    for (int i = 1; i < 6; i++) {
      MAC_ADDR[i] = EEPROM.read(i+1);
    }
  } else {
    for (int i = 1; i < 6; i++) {
      MAC_ADDR[i] = TrueRandom.randomByte();
      EEPROM.write(i+1, MAC_ADDR[i]);
    }
    EEPROM.write(7, '#');
  }     
    
  Serial.print("Conection_name  : "); Serial.println(Conection_name);
  Serial.print("Username_conn   : ");Serial.println(Username_conn);
  Serial.print("Password_conn   : ");Serial.println(Password_conn);
  Serial.println("--------------------------------------------------");
  Serial.print("ID_Device       : ");Serial.println(ID_Device);
  Serial.print("Subscribe       : ");Serial.println(Subscribe);
  Serial.print("Publish         : ");Serial.println(Publish);
  Serial.println("--------------------------------------------------");
  Serial.print("IP STATIC       : ");Serial.println(ip);
  Serial.print("MAC RANDOM      : ");Serial.println(mac2String(MAC_ADDR));
  Serial.print("server          : ");Serial.println(server);
  Serial.println("--------------------------------------------------");
   Serial.println("");
  
}

String mac2String(byte ar[]){
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%2X", ar[i]);
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

void setup() {
  Serial.begin(9600);
  //mySwitch.enableReceive(0);   default
  mySwitch.enableReceive(1);
  
    Serial.println("start");
    pinMode(9, OUTPUT);  
  digitalWrite(9, LOW);
  delayMicroseconds(500);
  digitalWrite(9, HIGH);
  delayMicroseconds(1000);

  pinMode(ledData,OUTPUT);
  pinMode(ledError,OUTPUT);

    digitalWrite(ledError, HIGH);
  
  Serial.print("RF MQTT V2.3\r\n");
  load_parameter();
  
//   if (Ethernet.begin(MAC_ADDR,ip) == 0) {
//    Serial.println("Failed to configure Ethernet using DHCP");
//    // no point in carrying on, so do nothing forevermore:
//    for (;;)
//      ;
//  }
//  else{
//  Serial.println("OK");
//      digitalWrite(ledError, LOW);
//  }


  Ethernet.begin(MAC_ADDR,ip);
  while(!Ethernet.begin(MAC_ADDR))
  {
    Serial.println("Failed to configure Ethernet using DHCP");
  }

  Serial.println("OK");
  digitalWrite(ledError, LOW);



  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
 
 
 if (client.connect(Conection_name, Username_conn, Password_conn)) {
    client.subscribe(Subscribe);
  }
  else {
    Serial.println("Server not Connected");
  }
  
  Serial.print("START\r\n");
}

void loop() {
  //SERIAL loop
    while (Serial.available() > 0) {

                incoming = Serial.readString();
                // say what you got:
                Serial.println(incoming);    
                
                Serial.flush(); 
                //SET#CON#TEXT#         //Connection Name
                //SET#USR#USERNAME#     //Username Connection
                //SET#PSW#PASSWORD#     //Password Connection
                //SET#IDD#IDDevice#     //ID Device
                //SET#SUB#Subscribe#    //Subscribe MQTT
                //SET#PUB#Publish#      //Publish MQTT
                //SET#SRV#192.168.3.1#  //IP Server            

    }
  
//Cek STATUS CONNECTION
if(Ethernet.maintain()==0){
  //nothing happen
}
else if(Ethernet.maintain()==1){
  //renew failed
}
else if(Ethernet.maintain()==2){
  //renew success
}
else if(Ethernet.maintain()==3){
  //rebind fail
}
else if(Ethernet.maintain()==4){
  //rebind success
}

 if (client.connect(Conection_name, Username_conn, Password_conn)) {
    // Client connected
    digitalWrite(ledError, LOW);
     client.loop();
      if (mySwitch.available()) {
        
        int value = mySwitch.getReceivedValue();
        int length = mySwitch.getReceivedBitlength();
        if (value == 0) {
          Serial.print("Unknown encoding");
        } 
        else {
          String data = String(mySwitch.getReceivedValue(), HEX);
          char charBuf[20];
          data.toCharArray(charBuf, 20);

          if(data.length()<6){
//            Serial.print("0");
//          Serial.println(data);
            if(data.length()==5){
              //Add dummy 0 one char
              data=String("0")+data;
            }
            else if(data.length()==4){
              //Add dummy 0 two char
              data=String("00")+data;
            }
            else if(data.length()==3){
              //Add dummy 0 three char
              data=String("000")+data;
            }
            else if(data.length()==2){
              //Add dummy 0 four char
              data=String("0000")+data;
            }
          }
          
          //RFADDRESS#DAT#IDdevice#tombol
          //12345#DAT#RA001#01
          String DatatoSendStr;
          char DatatoSend[100];
          String RF = String(data.substring(0,5));
          RF.toUpperCase();
               
          DatatoSendStr = RF + String("#DAT#") + ID_Device + String("#0") + String(data.substring(5,6));
          DatatoSendStr.toCharArray(DatatoSend, 100);
          
          Serial.println(DatatoSend);
          client.publish(Publish,DatatoSend);
          delay(200);
          digitalWrite(ledData,HIGH);
          delay(250);
          digitalWrite(ledData,LOW);
        }

    mySwitch.resetAvailable();
      }
  }
  else {
    Serial.println("Server not Connected");
     digitalWrite(ledError, HIGH);

  }   
}
