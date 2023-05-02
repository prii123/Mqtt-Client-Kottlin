#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <ESP32Time.h>
#include <WiFi.h>
#include <string.h>


/*
VARIABLES PARA USAR MQTT
*/
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const char* mqtt_server = "192.168.20.55";
const char* mqtt_user = "usuario1";
const char* mqtt_password = "123";
// esto es para usar un servidor en linea
const char* mqtt_server_hiv = "d6007c0d7c6040d4b7e7207bfbbf8dfe.s2.eu.hivemq.cloud";
const char* mqtt_port_hiv = 8883;
const char* mqtt_user_hiv = "prii123";
const char* mqtt_password_hiv = "04373847Vallejos";

  //SERVO
Servo servo;
int pinServo = 2;
int pos = 0;

// SENSOR DISTANCIA
int DISTANCIA = 0;
int pinLed = 14;
int pinEco = 12;
int pinGatillo = 13;

// medida del contenedor
int medida_contendedor = 30;



/* 
CONFIGURACION WIFI
*/
#define WIFI_SSID "Claro_6A8BAB"
#define WIFI_PASSWORD "M2Q3K5M7A3E3"



/* 
CONFIGURACION DE LA HORA
variables para configurar el tiempo
configuracion del servidor NTP
*/
const char* ntpServer = "pool.ntp.org";  // servidor ntp
const long gmtOffset_sec = -5 * 3600;    // Desplazamiento GMT
const int daylightOffset_sec = 0;        // Compensacion de luz diurna
// agregar el objeto de tiempo
ESP32Time rtc;
String HORA = "";

//--------------------------------------------------------------------------------------  VOID SETUP -------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // CONFIGURACION DEL WIFI
  config_wifi();

  // configuracion del broker
  client.setServer(mqtt_server , 1883);
  client.setCallback(callback);

  // CONDIGURACION DEL SERVO
  servo.attach(pinServo, 500, 2500);
  pinMode(pinLed, OUTPUT);

  //CONFIGURACION DE LA HORA
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

/* 
--------------------------------------------------------------------------------------  ULTRASONIDO -------------------------------------------------------------------------------------
FUNCION CALCULADOR DE DISTANCIA
*/
long readUltrasonicDistance(int triggerPin, int echoPin) {
  //Iniciamos el pin del emisor de reuido en salida
  pinMode(triggerPin, OUTPUT);
  //Apagamos el emisor de sonido
  digitalWrite(triggerPin, LOW);
  //Retrasamos la emision de sonido por 2 milesismas de segundo
  delayMicroseconds(2);
  // Comenzamos a emitir sonido
  digitalWrite(triggerPin, HIGH);
  //Retrasamos la emision de sonido por 2 milesismas de segundo
  delayMicroseconds(10);
  //Apagamos el emisor de sonido
  digitalWrite(triggerPin, LOW);
  //Comenzamos a escuchar el sonido
  pinMode(echoPin, INPUT);
  // Calculamos el tiempo que tardo en regresar el sonido
  return pulseIn(echoPin, HIGH);
}

/* 
--------------------------------------------------------------------------------------  TIEMPO -------------------------------------------------------------------------------------
FUNCION QUE DETERMINA LA HORA EN QUE SE ACTIVARAN EL SERVO
Y DETERMINA SI EN EL SERVIDOR ACTIVAORN EL SERVO MANUALMENTE
*/
bool leerTiempoParaMoverMotor(String horaEntrada) {
  //Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));
  if (horaEntrada == "07:00:00") {
    return true;
  } else if (horaEntrada == "11:00:00") {
    return true;
  } else if (horaEntrada == "15:00:00") {
    return true;
  } else if (horaEntrada == "19:00:04") {
    return true;
  } else {
    return false;
  }
}

/*
--------------------------------------------------------------------------------------  WIFI -------------------------------------------------------------------------------------
configuracion del wifi
*/
void config_wifi() {

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
}

/*
--------------------------------------------------------------------------------------  CALLBACK MQTT -------------------------------------------------------------------------------------
*/
void callback(char* topic, byte* message, unsigned int length){
    Serial.print("Mensaje recibido en topic: ");
    Serial.print(topic);
    Serial.print(", Mensaje: ");
    String messageTemp;
    for(int i = 0; i<length; i++){
      Serial.print((char)message[i]);
      messageTemp += (char)message[i];
    }
    Serial.println();

    //*************************************** primera salida (usando topic: esp32/salida1) **************************
    if(String(topic) == "esp32/servo1"){
      Serial.print("Cambio de salida: ");
      if(messageTemp == "on"){
        Serial.println("on");
        digitalWrite(pinLed, HIGH);
        //startMotor()
        //digitalWrite(ledPin1, HIGH)
      } else if(messageTemp == "off"){
        Serial.println("off");
        //digitalWrite(ledPin1, LOW)
        digitalWrite(pinLed, LOW);
      }
    }

    //*************************************** primera salida (usando topic: esp32/salida1) **************************
    if(String(topic) == "esp32/output2"){
      Serial.print("Cambio de salida: ");
      if(messageTemp == "on"){
        Serial.println("on");
        //digitalWrite(ledPin1, HIGH)
      } else if(messageTemp == "off"){
        Serial.println("off");
        //digitalWrite(ledPin1, LOW)
      }
    }
    //*************************************** Activar servomotor manual (usando topic: esp32/salida1) **************************
    if(String(topic) == "esp32/motorCompuerta"){
      Serial.print("Cambio de salida: ");
      if(messageTemp == "true"){
        Serial.println("on");
        startMotor();
        //digitalWrite(ledPin1, HIGH)
      } else if(messageTemp == "false"){
        Serial.println("off");
        //digitalWrite(ledPin1, LOW)
      }
    }

}

/*
--------------------------------------------------------------------------------------  RECONECTANDO MQTT -------------------------------------------------------------------------------------
*/
void reconnect(){
  while(!client.connected()){
    Serial.print("Intentando conexion MQTT... ");
    if(client.connect("ESP32Client", mqtt_user, mqtt_password)){
      Serial.println("conectado");
      client.subscribe("esp32/servo1");  //************ Topic: 'esp32/output1' *****************
      client.subscribe("esp32/output2");  //************ Topic: 'esp32/output2' *****************
      client.subscribe("esp32/motorCompuerta");  //************ Topic: 'esp32/motorCompuerta' *****************
    }else{
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println(" Intentelo de nuevo en 5s");
      delay(5000);
    } 

  }

}

/*
--------------------------------------------------------------------------------------  ACTIVAR MOTOR -------------------------------------------------------------------------------------
*/

void startMotor(){
  // activacion de motor manual
  digitalWrite(pinLed, HIGH);
    for (pos = 0; pos <= 90; pos += 1) {
      servo.write(pos);
      // esperamos en milisegundos
      delay(15);
    }
    digitalWrite(pinLed, LOW);
  
}



void loop() {
  // reconectando MQTT en caso que no haya conexion
  if(!client.connected()){
    reconnect();
  }
  client.loop();

  int dist = 0.01723 * readUltrasonicDistance(pinGatillo, pinEco);
  HORA = rtc.getTime("%H:%M:%S");
  String horaStr = HORA;

  if (dist > medida_contendedor) {
        // se enciendo el led cuando la distancia es superior a 30 cm lo que significa que la tapa del recipiente esta abierta
        dist = medida_contendedor;
        digitalWrite(pinLed, HIGH);
      } else if(dist < medida_contendedor) {
        // apago el led
        digitalWrite(pinLed, LOW);
      }

  // este condicional es para enviar la informacion al Broker MQTT
  long now = millis();
  if(now - lastMsg > 1000){  // 100 ms tiempo de muestreo
    lastMsg = now;
    

    float medida_contenedor_porcentual = (dist *100) /medida_contendedor;

    
    //envia la distancia al broker
    char distanciaPorcentualString[8];
    dtostrf(medida_contenedor_porcentual, 1, 2, distanciaPorcentualString);
    Serial.print("%: ");
    Serial.println(distanciaPorcentualString);
    client.publish("esp32/distancia_porcentual", distanciaPorcentualString);

     //envia la distancia al broker
    char distanciaMedidaString[8];
    dtostrf(dist, 1, 2, distanciaMedidaString);
    Serial.print("Distancia: ");
    Serial.println(distanciaMedidaString);
    client.publish("esp32/distancia_centimetros", distanciaMedidaString);

    //envia la hora local al broker
    //const char* horaChar = horaStr.c_str();
    //client.publish("esp32/horaLocal", horaStr);
  }  

  // obtenemos la hora local y se la pasamos como argumento a la funcion que hace la validacion y retorna un boolean
   
   digitalWrite(pinLed, HIGH);
  if (leerTiempoParaMoverMotor(HORA)) {
    for (pos = 0; pos <= 90; pos += 1) {
      servo.write(pos);
      // esperamos en milisegundos
      delay(15);
    }
    digitalWrite(pinLed, LOW);
  }






  delay(10);
}
