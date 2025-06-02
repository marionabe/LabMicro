/*************************************************************

Sección del codigo para descargar datos desde una pagina web

 *************************************************************/
#include <ESP8266WiFi.h> 
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
char ssid[] = "Casa bambu";
char pass[] = "camilo123";
const char* host = "worldtimeapi.org/"; 
String hora;
String payload;
String info_porciones;
String porc;

void setup() {
     Serial.begin(115200);   
     delay(100);    
     // We start by connecting to a WiFi network    
     Serial.println();   
     Serial.println();   
     Serial.print("Connecting to ");   
     Serial.println(ssid);      
     WiFi.begin(ssid, pass);      


     while (WiFi.status() != WL_CONNECTED) {
           delay(500);     
           Serial.print(".");   
           }    
      Serial.println("");   
      Serial.println("WiFi connected");     
      Serial.println("IP address: ");   
      Serial.println(WiFi.localIP());       


      
} 


void loop() {   
  hora = get_time();
  //Serial.println("Respuesta JSON de hora:");
  //Serial.println(hora);
  //Serial.println("Fin de respuesta JSON de hora");

  info_porciones = get_data();

  JsonDocument doc_info_porciones;
  deserializeJson(doc_info_porciones, info_porciones);
  int counter =1;
    Serial.println("test para extraer subjson de un json");
    while (counter<1000) {
      Serial.println("Se ingresa al while");
      String S_counter = String(counter);
      String key_porc = "porcion"+S_counter;
      Serial.println("String de info recibida");
      Serial.println(info_porciones);

      Serial.println("llave a la que se está accediendo_1");
      Serial.println(key_porc);
      Serial.println("resultado de comparacion");
      Serial.println(info_porciones.indexOf(key_porc));

      if (info_porciones.indexOf(key_porc)!=-1){
        porc = doc_info_porciones[key_porc].as<String>();
      }else{
        break;
      }
      Serial.println("llave a la que se está accediendo_2");
      Serial.println(key_porc);
      Serial.println("Info de esta porción");
      Serial.println(porc);
      counter++;
    }




  //Esta sección se utiliza para deserializar un jason, se puede usar como ejemplo
  /*
  Serial.println("test_deserialización");
  JsonDocument doc;
  deserializeJson(doc, hora);
  int year = doc["year"];
  int month = doc["month"];
  int day = doc["day"];
  int hour = doc["hour"];
  int minute = doc["minute"];
  int seconds = doc["seconds"];
  Serial.println("imprimir hora deserializada");
  Serial.println("año");
  Serial.println(year);
  Serial.println("mes");
  Serial.println(month);
  Serial.println("dia");
  Serial.println(day);
  Serial.println("hora");
  Serial.println(hour);
  Serial.println("minuto");
  Serial.println(minute);
  Serial.println("segundo");
  Serial.println(seconds);
  Serial.println("Fin de imprimir hora deserializada");
*/




  delay(10000);   

}


//Funcion utilizada para obtener la fecha con hora, desde una api
//devuelve un String similar a este:
//{"year":2025,"month":5,"day":23,"hour":18,"minute":50,"seconds":17,"milliSeconds":177,"dateTime":"2025-05-23T18:50:17.1775725"},
// "date":"05/23/2025","time":"18:50","timeZone":"America/Costa_Rica","dayOfWeek":"Friday","dstActive":false}
String get_time (){
  int intentos = 0;
  int httpCode = 0;
  while (intentos <=100 && httpCode<=0){
    Serial.print("connecting to ");   
    Serial.println("timeapi.io");      
    // Use WiFiClient class to create TCP connections   
    WiFiClientSecure client;   
    client.setInsecure();
    HTTPClient http;
    http.begin(client, "https://timeapi.io/api/time/current/zone?timeZone=America%2FCosta_Rica");

    httpCode = http.GET();
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Respuesta JSON dentro de la funcion:");
      Serial.println(payload);
      Serial.println("Numero de intentos antes de obtener la respuesta");
      Serial.println(intentos);
      return payload;
    } else {
      Serial.printf("Error en la solicitud HTTP: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    intentos ++;
  }
    return "error en obtener la hora";
}


//Esta función se utilizará para descargar datos desde el servidor 
//El plan es que devuelva un string que contenga el json con la info de las porciones
String get_data (){
  //Al momento de realizar esta sección del codigo, no se cuenta con el servidor implementado,
  //Por ello se van a simular los datos obtenidos.
  payload = "{\"porcion1\":{\"cantidad\":3,\"hora_para_servir\":\"08:15\",\"fecha_de_modificacion\":\"2025:05:23:22:31:23\",\"servido_hoy\":\"no\"},\"porcion2\":{\"cantidad\":3,\"hora_para_servir\":\"17:15\",\"fecha_de_modificacion\":\"2025:05:23:22:31:23\",\"servido_hoy\":\"no\"}}";
  return payload;

}





/*
  const int httpPort = 80;   
  if (!client.connect(host, httpPort)) {     
    Serial.println("connection failed");     
    return;   
    }      
    // We now create a URI for the request   
    String url = "/api/timezone/America/Costa_Rica";   
    Serial.print("Requesting URL: ");   
    Serial.println(url);      
    // This will send the request to the server   
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");  
    delay(500);      
    // Read all the lines of the reply from server and print them to Serial   
    while(client.available()){     
      String line = client.readStringUntil('\r');     
      Serial.print(line);   
    }      
    Serial.println();   
    Serial.println("closing connection"); 
  } 
*/


/*************************************************************

Esta sección corresponde al codigo que se utiliza para encender el led desde blynk
 *************************************************************/

/*
char ssid[] = "Casa bambu";
char pass[] = "camilo123";
#define BLYNK_TEMPLATE_ID           "TMPL2ayyPGdAD"
#define BLYNK_TEMPLATE_NAME         "Quickstart Template"
#define BLYNK_AUTH_TOKEN            "BrjdaFX4z0P0tZxtB22VOZX1j1Ofw9AJ"

#define BLYNK_TEMPLATE_ID           "TMPL2ayyPGdAD"
#define BLYNK_TEMPLATE_NAME         "Quickstart Template"
#define BLYNK_AUTH_TOKEN            "wCc2gRSCwFFPcQGpvz8QmVIfF0XDjAxT"

#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>



// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin V1
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable

  if (pinValue==1){
    digitalWrite(5, HIGH);
  }else {
    digitalWrite(5, LOW);
  }

}

void setup()
{
  // Debug console
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);
  pinMode(5, OUTPUT);
}

void loop()
{
  Blynk.run();
}


*/