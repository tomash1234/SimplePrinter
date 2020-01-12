/**
 * Made by Tomas's projects
 * https://github.com/tomash1234/SimplePrinter
 * My youtube channel
 * https://www.youtube.com/channel/UCT7FQjUkKS6F1Bj0QxLrB9A
 * Video with printer
 * https://www.youtube.com/watch?v=sigeapRQPQE
 * Feel free to use and edit this code :]
 */

#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Servo.h>

#define PACKET_MAX_SIZE 500 // number of pixels / 8
#define WIFI_SSID "*name_of_your_wifi*"
#define WIFI_PASSWORD "*your_password*"
#define LOCAL_PORT 8525 //port where the board will be receiving data from computer

#define SERVO_X_PIN D5
#define SERVO_Y_PIN D6
#define SERVO_PEN_PIN D7

#define CANVAS_WIDTH 40 //width of image in pixels
#define CANVAS_HEIGHT 40 //width of image in pixels

#define LENGTH_OF_BEAM_X 2.08f //in cm
#define LENGTH_OF_BEAM_Y 2.6f //in cm

//angles are in degrees
#define SERVO_X_END 90  
#define SERVO_Y_END 90

WiFiUDP Udp;

Servo servoX; 
Servo servoY; 
Servo servoPen; 

float realLifeImageSizeX = 2.4f;//in cm
float realLifeImageSizeY = 2.4f;//in cm

static void connectToWifi(const char* ssid, const char* password){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  WiFi.begin(ssid, password); 
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
    if(i > 10){
      Serial.println("Failed! No connection");  
      break;
    }
  }
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
}

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    servoX.attach(SERVO_X_PIN);  
    servoY.attach(SERVO_Y_PIN);
    servoPen.attach(SERVO_PEN_PIN);
    servoX.write(SERVO_X_END);
    servoY.write(SERVO_Y_END);
    servoPen.write(75);
    
    connectToWifi(WIFI_SSID, WIFI_PASSWORD);
  
    Udp.begin(LOCAL_PORT);
}


void drawPixel(){
    servoPen.write(80);
      delay(200);
    servoPen.write(87);
      delay(200);
    servoPen.write(75);

}

static void print(unsigned char* data, int len){
  Serial.println("Start printing...");
  int pixels = 0;
  int lastY = 0;
  for(int i = 0; i < len; i++){
    for(int j = 0; j < 8; j++){
      uint8_t pix = data[i] >> (7-j) & 1;
      int x = pixels % CANVAS_WIDTH;
      int y = pixels / CANVAS_WIDTH;
     

      float x2 = realLifeImageSizeX - realLifeImageSizeX / CANVAS_WIDTH * x;
      float y2 = realLifeImageSizeY / CANVAS_HEIGHT * y;
      //int angleX = (int)(SERVO_X_END - atan(x2 / LENGTH_OF_BEAM_X) / PI * 180.0f); 
      float angleX = (SERVO_X_END - atan(x2 / LENGTH_OF_BEAM_X) / PI * 180.0f); 
      //int angleY = (int)(SERVO_Y_END - atan(y2/b2) / PI * 180.0f); 
      float angleY = (SERVO_Y_END - atan(y2 / LENGTH_OF_BEAM_Y) / PI * 180.0f); 
      //int angleX = SERVO_X_START + (int)((1.0f *SERVO_X_END - SERVO_X_START)/ CANVAS_WIDTH) * x;
      //int angleY =  SERVO_Y_START + (int)((1.0f *SERVO_Y_END - SERVO_Y_START)/ CANVAS_HEIGHT) * y;
      servoX.write_float(angleX);
      //servoX.write(angleX);
      servoY.write_float(angleY);
      
      if(!pix){
        drawPixel();
      }else{
        delay(50);
      }
      pixels++;
    }
  }
  
}

static void listen(){
  int request = 0;
  if(Udp.parsePacket()){ 
    unsigned char* receivedData = (byte*) malloc(PACKET_MAX_SIZE * sizeof(unsigned char));
    int len = Udp.read(receivedData, PACKET_MAX_SIZE);
    Serial.printf("Received %d\n", len);
    
    if(len >= CANVAS_WIDTH * CANVAS_HEIGHT / 8){
      print(receivedData, len);
    }
    free(receivedData);
  }
}

void loop() {
    listen();
}
