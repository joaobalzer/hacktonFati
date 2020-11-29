#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <PubSubClient.h>

#include <PubSubClient.h>


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>

// -----------------------------------------------------------
// Projeto de IOT para o Hackaton FATI
// -----------------------------------------------------------

// -----------------------------------------------------------
// Configurações do WiFi
// -----------------------------------------------------------

//const char* ssid = "dlink-8D20";
//const char* password = "piclt67341";

const char* ssid = "nome da rede";
const char* password = "senha";

WiFiClient nodemcuClient;
// -----------------------------------------------------------


// -----------------------------------------------------------
// Configurações do MQTT
// -----------------------------------------------------------
#include<PubSubClient.h>
const char* mqtt_Broker="iot.eclipse.org";  // Porta: 1883
const char* mqtt_ClientID="planetapuc2019-pucpr";

PubSubClient client(nodemcuClient);
const char* topicoTemperatura="planetapuc2019/temperatura";
const char* topicoUmidade="planetapuc2019/umidade";
const char* topicoMedidas="planetapuc2019/medidas1";

// -----------------------------------------------------------
// Configurações do Sensor de Temperatura DHT
// -----------------------------------------------------------
#include <DHT.h>

#define DHTPIN D3      // Pino onde o DHT está conectado
#define DHTTYPE DHT22  // Tipo de Sensor: DHT22

DHT dht(DHTPIN,DHTTYPE);
int umidade;
int temperatura;

// ----------------------------------------------------------
// Display
// ----------------------------------------------------------
#include<Adafruit_SSD1306.h>
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);


// ----------------------------------------------------------
// Setup
// ----------------------------------------------------------
void setup() {
  
  Serial.begin(115200);  // Taxa de Comunicação da Serial (NodeMCU -> PC)
  dht.begin();
  configurarDisplay();
  conectarWifi();
  client.setServer(mqtt_Broker,1883);
  
}


// ----------------------------------------------------------
// Loop
// ----------------------------------------------------------
void loop() {
  
  if (!client.connected()) {
    reconectarMQTT();
  }
  medirTemperaturaUmidade();
  
  publicarTemperaturaUmidadeNoTopico();
  
  mostrarTemperaturaUmidade();
}


// ----------------------------------------------------------
// Funções Auxiliares
// ----------------------------------------------------------


// Configuração e Conexão da Rede WiFi
void conectarWifi() {
  
delay(10);
display.setTextSize(2);
display.setCursor(0,0);
display.print("Conectando WiFi");
display.display();

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }
}



// Configurar Display
void configurarDisplay() {
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.clearDisplay();
  
}


// Publica (MQTT) Temperatura e Umidade
void publicarTemperaturaUmidadeNoTopico(){

  char out_str[40]={};
  strcat(out_str,"Temperatura: ");
  strcat(out_str,String(temperatura).c_str());
  strcat(out_str," Umidade: ");
  strcat(out_str,String(umidade).c_str());
  strcat(out_str,"\r");
    
  client.publish(topicoTemperatura,String(temperatura).c_str(),true);
  client.publish(topicoUmidade,String(umidade).c_str(),true);
  client.publish(topicoMedidas,out_str,true);
}


// Reconectar o Cliente MQTT ao Servidor
void reconectarMQTT(){
  
  while (!client.connected()) {
    
  client.connect(mqtt_ClientID);
  }
  
}
  



// Medição de Temperatura e Umidade
void medirTemperaturaUmidade() {
  
  umidade=dht.readHumidity();
  temperatura=dht.readTemperature(false);
  
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print(" °C");  // Simbolo de Graus = Alt+0176
  Serial.print("    Umidade: ");
  Serial.println(umidade);
  delay(2000);
  
}


// Mostra Temperatura e Umidade
void mostrarTemperaturaUmidade() {
  
  mostrarMensagemNoDisplay("Temperatura",(temperatura)," C");
  mostrarMensagemNoDisplay("Umidade",(umidade)," %");
  
}


// Função para Mostrar Temperatura e Umidade
void mostrarMensagemNoDisplay (const char* texto1,int medicao,const char* texto2) {
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print(texto1);
  display.setTextSize(5);
  display.setCursor(20,20);
  display.print(medicao);
  display.setTextSize(2);
  display.print(texto2);
  display.display();
  delay(2000);
  
}
