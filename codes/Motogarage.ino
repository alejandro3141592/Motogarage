#include <SPI.h>
#include <MFRC522.h>
#include "BluetoothSerial.h"
#include <Adafruit_Sensor.h>
#define SS_PIN 5
#define RST_PIN 22
BluetoothSerial SerialBT;
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
int pin_iluminacion = 12;
int pin_selenoide = 32;
bool Estado = 0;
int PHOTO_SENSOR = 15;
bool men = false;
bool LuzEncendida = 0;
int intensidad_leds = 255;
unsigned long tiempo_espera = 500;
unsigned long tiempo_inicial;
unsigned long tiempo_actual;
String ID1 = "56 22 29 30";
String ID2 = "E4 E1 19 49";
String ID3 = "D6 31 3B 94";
String ID4 = "44 C2 07 48";
String lectura = "";
String message1 = "";
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
void setup()
{
Serial.begin(115200); // Initiate a serial communication
SPI.begin(); // Initiate SPI bus
mfrc522.PCD_Init(); // Initiate MFRC522
Serial.println("Approximate your card to the reader...");
Serial.println();
pinMode(pin_iluminacion, OUTPUT);
pinMode(pin_selenoide, OUTPUT);
SerialBT.begin("ESP32_SIELCO2");
digitalWrite(pin_iluminacion, LOW);
pinMode(pin_selenoide, LOW);
Estado = 0;
//digitalWrite(pin_selenoide,HIGH);
//delay(1000);
//digitalWrite(pin_selenoide,LOW);
//ledcSetup(ledChannel, freq, resolution);
// attach the channel to the GPIO to be controlled
//ledcAttachPin(pin_iluminacion, ledChannel);
tiempo_inicial = millis();
}
//void LeerTarjeta();
void loop()
{
LeerTarjeta();
LeerSensorFotoelectrico();
//ControlLeds();
RecibirMensaje();
EnviarDatos();
}
void EnviarDatos() {
tiempo_actual = millis();
if (tiempo_actual - tiempo_inicial >= 1000) {
SerialBT.println(Estado);
//SerialBT.print(",");
//SerialBT.println(LuzEncendida);
tiempo_inicial = millis();
}
}
void RecibirMensaje() {
if (SerialBT.available()) {
char incomingChar1 = SerialBT.read();
if (incomingChar1 != '\n') {
message1 += String(incomingChar1);
}
else {
men = true;
}
}
if (men) {
Serial.println(message1);
int num = message1.toInt();
if (num == 0) {
if (message1 == "Activar 1") {
//Serial.println(mess);
ActivarSelenoide();
}
} else {
//if (num==1){
//LuzEncendida=!LuzEncendida;
//}
}
men = false;
message1 = "";
}
}
void LeerSensorFotoelectrico() {
Estado = digitalRead(PHOTO_SENSOR);
digitalWrite(pin_iluminacion, Estado);
}
void ControlLeds() {
if (Estado == 1) {
digitalWrite(pin_iluminacion, LuzEncendida);
} else {
digitalWrite(pin_iluminacion, LOW);
}
}
void LeerTarjeta() {
if ( ! mfrc522.PICC_IsNewCardPresent())
{
return;
}
// Select one of the cards
if ( ! mfrc522.PICC_ReadCardSerial())
{
return;
}
//Show UID on serial monitor
Serial.print("UID tag :");
String content = "";
byte letter;
for (byte i = 0; i < mfrc522.uid.size; i++)
{
Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
Serial.print(mfrc522.uid.uidByte[i], HEX);
content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
content.concat(String(mfrc522.uid.uidByte[i], HEX));
}
Serial.println();
Serial.print("Message : ");
content.toUpperCase();
lectura = content.substring(1);
if (lectura == ID1 || lectura == ID2 || lectura == ID3 || lectura == ID4) //change here the UID of the
card/cards that you want to give access
{
Serial.println("Authorized access");
Serial.println();
ActivarSelenoide();
}
else {
Serial.println(" Access denied");
delay(500);
}
}
void ActivarSelenoide() {
digitalWrite(pin_iluminacion, LOW);
delay(10);
digitalWrite(pin_selenoide, HIGH);
delay(2000);
digitalWrite(pin_selenoide, LOW);
delay(10);
digitalWrite(pin_iluminacion, Estado);
}
