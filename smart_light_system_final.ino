#include <SoftwareSerial.h> //include the software serial library
int sensor; //variable to store sensor values
int data; //variable to stored mapped sensor values
int Light_status=10; //variable to store pump status values
int threshold=55;



SoftwareSerial esp8266(3, 4); //set the software serial pins RX pin = 3, TX pin = 4 
//definition of variables
#define DEBUG true //show messages between ESP8266 and Arduino in serial port, when set to true
#define SSID "Tech" //replace x with your wifi network name
#define PASS "1234567890" //replace x with your wifi network password


String sendAT(String command, const int timeout, boolean debug)
{
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      char c = esp8266.read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}

void setup()
{

  Serial.begin(9600);// begin the serial communication with baud of 9600
  esp8266.begin(9600);// begin the software serial communication with baud rate 9600
  
  sendAT("AT+RST\r\n", 2000, DEBUG); // call sendAT function to send reset AT command
  sendAT("AT\r\n", 1000, DEBUG);
  sendAT("AT+CWMODE=1\r\n", 1000, DEBUG); //call sendAT function to set ESP8266 to station mode
  sendAT("AT+CWJAP=\""SSID"\",\""PASS"\"\r\n", 2000, DEBUG); //AT command to connect wit the wifi network

  while(!esp8266.find("OK")) { //wait for connection
  } 
  sendAT("AT+CIFSR\r\n", 1000, DEBUG); //AT command to print IP address on serial monitor
  sendAT("AT+CIPMUX=0\r\n", 1000, DEBUG); //AT command to set ESP8266 to multiple connections
  pinMode(8,OUTPUT);
  
}

void loop(){
  // put your main code here, to run repeatedly:
  sensor=analogRead(A0); //read raw sensor data and store it in variable sensor
  data=map(sensor,0,1023,0,100); //map the raw sensor data and store the result in data variable
  

  if(data<threshold){ //check if sensor value is less then 76
    digitalWrite(8,HIGH); //switch on the water pump
    Light_status=100; //update pump status variable value to 100
  }
  else{
    digitalWrite(8,LOW); //switch off the water pump
    Light_status=10; //update pump status variable value to 0
  }

  String sensor_value=String(data); //convert interger to string data type
  Serial.print("Light intensity: "); //print soil moisture on serial monitor
  Serial.println(data); //print soil moisture value on serial monitor

  String Light_status1=String(Light_status); //convert interger to string data type
  Serial.print("Light status: "); //print soil moisture on serial monitor
  Serial.println(Light_status); //print soil moisture value on serial monitor

  String threshold1=String(threshold); //convert interger to string data type
  Serial.print("Threshold: "); //print soil moisture on serial monitor
  Serial.println(threshold); //print soil moisture value on serial monitor
  
  updateTS(sensor_value,Light_status1,threshold1); //call the function to update ThingSpeak channel
  delay(3000);
}



void updateTS(String S,String L,String T){
  Serial.println("");
  sendAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n", 1000, DEBUG);    
  delay(2000);
  String cmdlen;
  String cmd="GET /update?key=HLO3TX434U118I5Q&field1="+L+"&field2="+S+"&field3="+T+"\r\n"; // update the temprature and humidity values on thingspeak url,replace xxxxxxx with your write api key
   cmdlen = cmd.length();
  sendAT("AT+CIPSEND="+cmdlen+"\r\n", 2000, DEBUG);
   esp8266.print(cmd);
   Serial.println("");
   sendAT("AT+CIPCLOSE\r\n", 2000, DEBUG);
   Serial.println("");
   delay(1000);
   }




