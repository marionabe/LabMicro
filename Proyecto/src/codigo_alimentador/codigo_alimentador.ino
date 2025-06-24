/*************************************************************

Sección del codigo para descargar datos desde una pagina web

 *************************************************************/
#include <ESP8266WiFi.h> 
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>
#include <LittleFS.h>
//#include <LiquidCrystal_I2C.h>


char ssid[] = "Casa bambu";
char pass[] = "camilo123";
const char* host = "worldtimeapi.org/"; 
String hora;
String payload;
String info_porciones;
String porc;
String info_en_mem;
//LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  //lcd.init();              // Inicializa el LCD
  //lcd.backlight();         // Enciende la luz de fondo
  //lcd.setCursor(0, 0);     // Columna 0, fila 0
  //lcd.print("Hola mundo!");

/***********************************************************************/ 
  pinMode(12, OUTPUT); 

  Serial.begin(115200);   
     delay(100);    
     // We start by connecting to a WiFi network    
     Serial.println();   
     Serial.println();   
     Serial.print("Connecting to ");   
     Serial.println(ssid);
  


  //conectars e por ble y obtener contraseña     

     WiFi.begin(ssid, pass);      

     while (WiFi.status() != WL_CONNECTED) {
           delay(500);     
           Serial.print(".");   
           }   

      Serial.println("");   
      Serial.println("WiFi connected");     
      Serial.println("IP address: ");   
      Serial.println(WiFi.localIP());       

    //Acá se monta el sistema para guardar y leer archivos de la memoria del micro
    if (!LittleFS.begin()) {
      Serial.println("Error al montar LittleFS");
    } else {
      Serial.println("LittleFS montado correctamente");
    }
      
} 


void loop() {   
  delay(2000);
  serv_porc(1);


  /*

  //hora = get_time();

  info_porciones = get_data();
  info_porciones.trim(); 

//Aca incia el diagrama de flujo
  if (info_porciones == "error"){
    //En este caso no fue posible descargar la info del servidor
    //Se procede a utilizar la informacion que se tenia almacenada en memoria
    info_porciones = leer_porc_mem();
  }else{
    //En este caso sí fue posible descargar la info del servidor
    //Comparar la info descargada con la informacion almacenada en memoria.
    info_en_mem = leer_porc_mem();
    info_en_mem.trim();
    if (info_porciones.equals(info_en_mem)){
      //Info almacenada sí es igual que la info en memoria
    }else{
      //Info en memoria no es igual que la info en memoria
      //Procedo a quedarme con la info mas reciente
      int comp = comparar_infos(info_porciones, info_en_mem);
      //Si la info en memoria es mas reciente (comp == 2), se reemplaza la variable info_porciones con la info almacenada
      //Caso contrario se conserva la info del servidor
      if (comp == 2){
        info_porciones = info_en_mem;
        //TODO: aca se debe llamar a la funcion encargada de subir al info al servidor
      }else{
      //si la info del servidor es mas reciente, entonces se escribe esa info en la memoria del dispositivo
      write_data(info_porciones);
      }
    }
  }

                      Serial.println("Test para modificar un dato en el jason");
                      Serial.println("Info antes de modificar");
                      Serial.println(info_porciones);
    String info_porciones_2 = mod_field(info_porciones, "porcion1", "cantidad", "200");
                      Serial.println("Info luego de la modificacion");
                      Serial.println(info_porciones_2);

String hora = get_time();
JsonDocument doc_hora;
  deserializeJson(doc_hora, hora);
String hora_minutos = doc_hora["time"];
lcd.clear();
  lcd.print(hora_minutos);
  lcd.setCursor(0, 1);     // Columna 0, fila 0
  lcd.print("Test");
*/
}

/*********************************************************************
* Funcion utilizada para obtener la fecha con hora, desde una api
* devuelve un String similar a este:
* {"year":2025,"month":5,"day":23,"hour":18,"minute":50,"seconds":17,"milliSeconds":177,"dateTime":"2025-05-23T18:50:17.1775725"},
*  "date":"05/23/2025","time":"18:50","timeZone":"America/Costa_Rica","dayOfWeek":"Friday","dstActive":false}
*********************************************************************/
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

/*********************************************************************
* Esta función se utilizará para descargar datos desde el servidor 
* El plan es que devuelva un string que contenga el json con la info de las porciones
*********************************************************************/
String get_data (){
 int intentos=0;
  int httpCode = 0;
    while (intentos <=100 && httpCode<=0){
    Serial.print("connecting to ");   
    Serial.println("google");      
    // Use WiFiClient class to create TCP connections   
    WiFiClientSecure client;   
    client.setInsecure();
    HTTPClient http;
    http.begin(client, "https://script.google.com/macros/s/AKfycbynbVyksEvWWlkKShwpAHmn0V-gMnAZ6BLMh4YIQBxNwTtjKbq2_GB08awOlkIB3KsJlg/exec");
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    httpCode = http.GET();
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("payload recibido de la direccion inicial");
      Serial.println(payload);
      Serial.println("Numero de intentos antes de obtener la respuesta");
      Serial.println(intentos);
    } else {
      Serial.printf("Error en la solicitud HTTP: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    intentos ++;
  }
  return payload;
}


/*********************************************************************
* Esta funcion se utiliza para leer la informacion de las porciones desde la memoria del dispositivo
* La funcion no recibe ningun parametro
* La funcion devuelve un Json en forma de String que contiene toda la informacion de las porciones
*********************************************************************/
String leer_porc_mem(){
  String payload_2;
  File file = LittleFS.open("/datos.txt", "r");
  if (file) {
    while (file.available()) {
      char c = file.read();
      payload_2 += c;
    }
  file.close();
  }
  return payload_2; 
}


/*********************************************************************
* Esta función se utiliza para comparar dos Json en forma de String
* La funcion recibe dos parametros, que corresponden a los string con los Json
* La funcion devuelve 1 o 2, indicando si el String 1 es mas reciente o 2 para el caso contrario.
**********************************************************************/
int comparar_infos(String json_1, String json_2){
  int numero_1 = 0;
  int numero_2 = 0;
  int reciente=1; //se incia suponiendo que el mas reciente es el documento 1

  //Se extraen las fechas de la primera porcion en cada json. La fecha de modificacion deberia ser igual en todas las porciones dentro del mismo json.
  JsonDocument doc_1;
  deserializeJson(doc_1, json_1);
  String fecha1 = doc_1["porcion1"]["fecha_de_modificacion"];
  JsonDocument doc_2;
  deserializeJson(doc_2, json_2);
  String fecha2 = doc_2["porcion1"]["fecha_de_modificacion"];

  //Se recorre digito por digito las fechas hasta encontrar alguna diferencia en los digitos. En este caso se escoge el mayor y de 
  //esta forma se puede decidir cual dato es mas reciente con base en la fecha de modificacion
  for (int i=0; i < fecha2.length(); i++){
    char num_1 = fecha1.charAt(i);
    char num_2 = fecha2.charAt(i);
    if (isDigit(num_1) && isDigit(num_2)){
      numero_1 = num_1 - '0';
      numero_2 = num_2 - '0';
        if (numero_1 < numero_2){
          reciente = 2;
          return reciente;
        }else if (numero_1 > numero_2){
          return reciente;
        }
    }
  }
  return reciente;
}

/*********************************************************************
* Esta funcion se utiliza para escribir la info en memoria
* La funcion recibe un unico parametro que es el string con la info. La data siempre se guarda en el mismo lugar
**********************************************************************/
void write_data (String data_str){
  if (!LittleFS.begin()) {
  Serial.println("Error al montar LittleFS");
  return;
  }

  File file = LittleFS.open("/datos.txt", "w");
  if (file) {
    file.println(data_str); // Guardar valor
    file.close();
    Serial.println("Variable guardada correctamente.");
  }
}


/*********************************************************************
* Esta funcion se utiliza para modificar algun espacio en el String con la info de las porciones
* Recibe 4 parametros:
*     - El String con la info de las porciones
*     - El nombre de la porcion que se desea cambiar (porcion1, porcion2, ..., porcion n)
*     - El valor del campo que se desea cambiar (cantidad de porciones, ya_servido, ...)
*     - El nuevo valor para el espacio en cuestion
* La funcion devuelve el un String similar al original, pero con las modificaciones aplicadas
**********************************************************************/
String mod_field(String json, String id_porc, String id_field, String new_value){
  JsonDocument json_porc;
  deserializeJson(json_porc, json);
  json_porc[id_porc][id_field] = new_value;
  String jsonString;
  serializeJson(json_porc, jsonString);
  return jsonString;
}


/*********************************************************************
* Esta función se utiliza con el ADC y el sensor infrarojo utilizado
* La función devuelve el valor cuantizado desde el pin ADC
* La variable umbral se configura para que la funcion devuelva 1 si se sobrepasa o 
* 0 si es inferior.
* Para este proyecto, devuelve 1 si se detecta mucha proximidad
**********************************************************************/
int dtc_prox(){
  int umbral=250;
  int valorADC = analogRead(A0);

  if (valorADC>umbral){
    return 1;
  }
  return 0;
}


/*********************************************************************
* Esta función se utiliza para servir una porcion de alimento
**********************************************************************/
void serv_porc(int cantidad){
  digitalWrite(12, HIGH);
  double contador=0;
  if (dtc_prox()==0){
              Serial.print("entrando a primer while");

    while(dtc_prox()==0){
      
      delay(5);
      contador++;
    }
  }
  contador=0;
              Serial.print("entrando a segundo while");

  while (dtc_prox()==1){
              Serial.print("valor de proximidad en segundo while:        ");
              Serial.println(dtc_prox());
    
      delay(5);
      contador++;
  }
  digitalWrite(12, LOW);
  Serial.println("Saliendo de funcion servir porción");
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
    */


    




/*

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
    */




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



