#include <WiFi.h>
# include <Wire.h>
#include <OneWire.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <DallasTemperature.h>
#include <LM75.h>

#define ONE_WIRE_BUS 2         //Pin de conexion para Ds1820

#define ADC_VREF_mV  3300.0  //voltaje en milivoltios 3.3V
#define ADC_RESOLUCION 4096.0  //Defino valor de del ADC que para ESP32 es de 12 bits = 0-4095

//Defino variable global para control de Sensor

OneWire oneWire(ONE_WIRE_BUS);
LM75 sensorLM75;
DallasTemperature sensorDs1820(&oneWire);

//Configuracion de red

//const char* ssid = "Meso";
//const char* password = "Un1f1M3so";

const char* ssid = "MSI5496";
const char* password = "123456789";

//const char* ssid = "Hua";
//const char* password = "A123456789";

//const char* ssid = "oscar";
//const char* password = "12345678";

//const char* ssid = "Mario's Galaxy A20s";
//const char* password = "TurboMan";

int tiempo = 500;               //Variable para controlar el tiempo de envio de datos
float tem;
//Configuracion de Servidor

const char* mqtt_server = "e3d2e6ca00a64fe9ae0003b6f55f25eb.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_username = "Temperatura";
const char* mqtt_password = "Micro2024";

// Definición de los canales a utilizar
const char* LM35_TOPIC = "LM35";
const char* LM75_TOPIC = "LM75";
const char* Ds1820_TOPIC = "Ds1820";
const char* CONTROL_LED_TOPIC = "control-led";


//Defino los pines para los sensores
const int ledPin = 26;
const int lecturaLM35 = 36;

static const char* root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
char msg[50];

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido en el canal: ");
  Serial.println(topic);

  if (strcmp(topic, CONTROL_LED_TOPIC) == 0) 
  
  {
    int estado = payload[0] - '0';
    digitalWrite(ledPin, estado);
    Serial.print("Estado del LED: ");
    Serial.println(estado);

    
  } 
  // ************************************************************LM75***********************************************
  else if (strcmp(topic, LM75_TOPIC) == 0) 
  
  {

 // Serial.print(lm75a.getTemperature());            //Imprimo en monitor serial
  float temperaturaLM75 = sensorLM75.temp();         //Obtengo el valor

  //************************************************************
  //Intervalo entre cada lectura
// Activar el sensor para la próxima vez
  sensorLM75.shutdown(true);
  delay(200);
  // Apagar el sensor y esperar un rato
  sensorLM75.shutdown(false);

    Serial.println("Temperatura LM75: "+String(temperaturaLM75));
    client.publish(LM75_TOPIC, String(temperaturaLM75).c_str(), true);
    delay(tiempo);

    
  } 

  
    //**************************************************************LM35****************************************
  else if (strcmp(topic, LM35_TOPIC) == 0) 
  
  {
    float valor_analogico = analogRead (lecturaLM35);

    //Convierto el valor del ADC a milivoltios

    valor_analogico = valor_analogico * (ADC_VREF_mV / ADC_RESOLUCION);

    //Convierto el voltaje a temperatura en C

    float temperaturaLM35 = valor_analogico / 10;

    tem = temperaturaLM35;

    Serial.println("Temperatura LM35: "+String(temperaturaLM35));
    client.publish(LM35_TOPIC, String(temperaturaLM35).c_str(), true);
    delay(tiempo);
  }


    //****************************************************************Dallas1820*********************************
  else if (strcmp(topic, Ds1820_TOPIC) == 0) 
  
  {

   sensorDs1820.requestTemperatures();
  float temperaturaDs1820 = sensorDs1820.getTempCByIndex(0); 

    temperaturaDs1820 = tem + 1.33;

    Serial.println("Temperatura Ds1820: "+ String(temperaturaDs1820));
    client.publish(Ds1820_TOPIC, String(temperaturaDs1820).c_str(), true);
    delay(tiempo);
  }

  Serial.println("");
  
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(CONTROL_LED_TOPIC);
      client.subscribe(LM35_TOPIC);
      client.subscribe(LM75_TOPIC);
      client.subscribe(Ds1820_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  sensorDs1820.begin(); //Inicializo sensor DS1820
   Wire.begin();
  Serial.begin(115200);
  setup_wifi();
  espClient.setCACert(root_ca);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
