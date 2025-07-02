/*************************************************************
* Includes
 *************************************************************/
  #include <ESP8266WiFi.h> 
  #include <ArduinoJson.h>
  #include <ESP8266HTTPClient.h>
  #include <FS.h>
  #include <LittleFS.h>
  #include <Wire.h>
  #include <LiquidCrystal_I2C.h>

/*********************************************************************
* Variables necesarias en el código
*********************************************************************/

    char ssid[] = "";
    char pass[] = "";
  int tiempo_de_ejecucion=0;
  const char* host = "worldtimeapi.org/";
  int lastEncoderCLK = HIGH;
  bool buttonPressed = false; 
  String hora;
  String payload;
  String info_porciones;
  String porc;
  String info_en_mem;
  String link_servidor = "https://script.google.com/macros/s/AKfycbyHo4JFEqel1CJpa5QsrEvf8s83MW9FR0vvOT2prvk9yaQxGmoUjDG_mkMV7eoYtKFCqA/exec";
  String link_hora = "https://script.google.com/macros/s/AKfycbwWrAhggNMcUxqarGNGU6fT8n0ZASkxxVHTmy_47nVgiitOYpQtESRW-cK-Gm1HOjLYIw/exec";
  #define ENCODER_CLK 16
  #define ENCODER_DT  14
  #define ENCODER_SW  13
  enum State {
    MAIN_MENU,
    VER_PORCIONES,
    CREAR_PORCION_HORA,
    CREAR_PORCION_MINUTO,
    CREAR_PORCION_CANTIDAD,
    PORCION_OPCIONES,
    MODIFICAR_HORA,
    MODIFICAR_MINUTO,
    MODIFICAR_CANTIDAD,
    Dispensar
  };
  State menuState = MAIN_MENU;
  int menuIndex = 0;
  int scrollIndex = 0;
  int selectedPorcion = -1;
  struct Porcion {
    int cantidad = 0;
    int horaMinutos = 0;    // minutos totales desde medianoche
    bool servido = false;
    int cantidadReal = 0;   // No implementado
    int horaServido = -1;   // No implementado
    String fecha_de_modificacion = "";
  };
  Porcion porciones[10];
  int numPorciones = 0;
  int editHora = 0;
  int editMinuto = 0;


LiquidCrystal_I2C lcd(0x27, 16, 2);

//Función ISR para click del SW
void IRAM_ATTR clickISR() {
  delay(100);
  delay(0);
  handleSelect();
  mostrarMenu();
}


void setup() {
  Serial.begin(115200);   
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(12, OUTPUT); 
  attachInterrupt(digitalPinToInterrupt(ENCODER_SW), clickISR, FALLING);
  lcd.init();
  lcd.backlight();
  mostrarMenu();

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
  
  //Acá se monta el sistema para guardar y leer archivos de la memoria del micro
  if (!LittleFS.begin()) {
    Serial.println("Error al montar LittleFS");
  } else {
    Serial.println("LittleFS montado correctamente");
  }

  tiempo_de_ejecucion = 3000;
} 


void loop() {   
  delay(0);
  // Lectura de giro del encoder
  int currentCLK = digitalRead(ENCODER_CLK);
  if (currentCLK != lastEncoderCLK && currentCLK == LOW) {
    if (digitalRead(ENCODER_DT) != currentCLK) {
      menuIndex++;  // Sentido horario
    } else {
      menuIndex--;  // Sentido antihorario
    }
    mostrarMenu();
  }
  lastEncoderCLK = currentCLK;

  if (millis() - tiempo_de_ejecucion>=30000 && menuState == MAIN_MENU){  //retardo 3000 ms
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Actualizando  ");
    lcd.setCursor(0,1);
    lcd.print("     Datos     ");
    Serial.println("llamando a json_a_strc");
    json_a_strc(leer_porc_mem());

    tiempo_de_ejecucion = millis();
    
    //La variable info_porciones contiene el String principal con la info
    info_porciones = get_data();
    info_porciones.trim(); 

    // Aca incia el diagrama de flujo
    // Corresponde a "Descargar info del servidor"
    if (info_porciones == "error"){
      // En este caso no fue posible descargar la info del servidor
      // Se procede a utilizar la informacion que se tenia almacenada en memoria
      info_porciones = leer_porc_mem();
    }else{
      // En este caso sí fue posible descargar la info del servidor
      // Comparar la info descargada con la informacion almacenada en memoria.
      info_en_mem = leer_porc_mem();
      info_en_mem.trim();
      if (info_porciones.equals(info_en_mem)){
        //Info almacenada sí es igual que la info en memoria
      }else{
        // Info en memoria no es igual que la info en memoria
        // Procedo a quedarme con la info mas reciente
        int comp = comparar_infos(info_porciones, info_en_mem);
        // Si la info en memoria es mas reciente (comp == 2), se reemplaza la variable info_porciones con la info almacenada 
        // en memoria. Además se sube la info en memoria al servidor.
        // Caso contrario se conserva la info del servidor
        if (comp == 2){
          info_porciones = info_en_mem;
          upload_data(info_porciones);
        }else{
          // si la info del servidor es mas reciente, entonces se escribe esa info en la memoria del dispositivo
          write_data(info_porciones);
        }
      }
    }

    //Este es el segundo estado verde
    //Corresponde a "Descargar la hora actual"
    hora = get_time();
    JsonDocument hora_dese;
    deserializeJson(hora_dese, hora);
  
    //Deserializar la info de las porciones
    JsonDocument info_porc_dese;
    deserializeJson(info_porc_dese, info_porciones);
    JsonObject object = info_porc_dese.as<JsonObject>();

    //Comprobar si hay que servir comida en este minuto
    for(int i=1; i<=object.size();i++){
      String porcion ="porcion"+String(i);
      // Servir alguna porcion si se ha encontrado
      if (String(info_porc_dese[porcion]["hora_para_servir"]) == String(hora_dese["hourMin"])){
        if(String(info_porc_dese[porcion]["servido_hoy"]) == "no"){
          Serial.println("Es necesario servir esta porción");
          serv_porc(String(info_porc_dese[porcion]["cantidad"]).toInt());
          info_porc_dese[porcion]["servido_hoy"] = "si";
          //Es necesario volver a armar el json y subirlo al servidor para actualizar la informacion
          Serial.print("Subiendo información al servidor: ");
          serializeJson(info_porc_dese, info_porciones);
          upload_data(info_porciones);
          Serial.println("ok");
        }else{
          Serial.println("No es necesario servir esta porción: Ya se ha servido hoy.");
        }
      }else{
        Serial.println("No es necesario servir esta porcion.");
      }
    }
  mostrarMenu();
  }
}


void json_a_strc(String json){
  JsonDocument doc;
  deserializeJson(doc, json);
  JsonObject object = doc.as<JsonObject>();
  numPorciones=0;
  for(int i=1; i<=object.size();i++){
    Porcion nueva;
    nueva.cantidad = String(doc["porcion"+ String(i)]["cantidad"]).toInt();
    String hora_para_servir = doc["porcion"+ String(i)]["hora_para_servir"];
    nueva.horaMinutos = String(hora_para_servir.substring(0,hora_para_servir.indexOf(":"))).toInt()*60 + String(hora_para_servir.substring(hora_para_servir.indexOf(":") + 1)).toInt();
    nueva.servido = (String(doc["porcion"+ String(i)]["servido_hoy"]) == "si") ? true : false;
    nueva.fecha_de_modificacion = String(doc["porcion"+ String(i)]["fecha_de_modificacion"]);
    numPorciones++;
    porciones[i-1] = nueva;
  }
}


void strc_a_json(){
  JsonDocument doc0;
  deserializeJson(doc0, String(hora));
  String time = String(doc0["now"]);

  String json_completo; 
  JsonDocument doc1;
  JsonDocument doc2;
  for(int i = 1; i<= numPorciones; i++){
    doc2["cantidad"] = String(porciones[i-1].cantidad);
    doc2["hora_para_servir"] = String(porciones[i-1].horaMinutos / 60) + ":" + String(porciones[i-1].horaMinutos % 60);
    doc2["fecha_de_modificacion"] = time;
    doc2["servido_hoy"] = porciones[i-1].servido ? "si" : "no";
    doc1["porcion"+String(i)] = doc2;
  }
  serializeJson(doc1, json_completo);
  write_data(json_completo);             
}


void mostrarMenu() {
  lcd.clear();
  switch (menuState) {
    case MAIN_MENU:
      if (menuIndex < 0) menuIndex = 1;
      if (menuIndex > 1) menuIndex = 0;
      lcd.setCursor(0, 0);
      lcd.print(menuIndex == 0 ? "> Ver porciones" : "  Ver porciones");
      lcd.setCursor(0, 1);
      lcd.print(menuIndex == 1 ? "> Dispensar" : "  Dispensar");
      break;

    case VER_PORCIONES: {
      const int totalItems = numPorciones + 2;  // porciones + "Crear nueva" + "Regresar"
      // 1) Clamp de menuIndex en [0..totalItems-1]
      if (menuIndex < 0)          menuIndex = totalItems - 1;
      else if (menuIndex >= totalItems) menuIndex = 0;
      // 2) Ajustar scrollIndex para mantener menuIndex visible
      if (menuIndex < scrollIndex)       scrollIndex = menuIndex;
      else if (menuIndex > scrollIndex+1) scrollIndex = menuIndex - 1;
      // 3) Dibujar dos filas basadas en scrollIndex
      for (int row = 0; row < 2; row++) {
        int idx = (scrollIndex + row) % totalItems;
        lcd.setCursor(0, row);
        // Selector
        if (idx == menuIndex)     lcd.print("> ");
        else                      lcd.print("  ");
        // Contenido
        if (idx < numPorciones) {
          // Porciones existentes
          lcd.print("P");
          lcd.print(idx + 1);
          lcd.print(": ");
          lcd.print(porciones[idx].cantidad);
          lcd.print(" @ ");
          imprimirHora(porciones[idx].horaMinutos);
        } else if (idx == numPorciones) {
          // Opción crear nueva
          lcd.print("Crear nueva");
        } else {
          // idx == numPorciones+1 → Regresar
        lcd.print("Regresar");
        }
      }
    break;
    }

    case PORCION_OPCIONES:
      if (menuIndex < 0) menuIndex = 2;
      if (menuIndex > 2) menuIndex = 0;
      if (menuIndex <= 1) {
        lcd.setCursor(0, 0);
        lcd.print(menuIndex == 0 ? "> Modificar" : "  Modificar");
        lcd.setCursor(0, 1);
        lcd.print(menuIndex == 1 ? "> Eliminar" : "  Eliminar");
      } else {
        lcd.setCursor(0, 0);
        lcd.print("> Regresar");
        lcd.setCursor(0, 1);
        lcd.print("                "); // limpia fila 1
      }
      break;

    case Dispensar:
      if (numPorciones == 0) {
        lcd.setCursor(0, 0);
        lcd.print("No hay registros");
        lcd.setCursor(0, 1);
        lcd.print("Regresar");
        if (menuIndex < 0) menuIndex = 1;
        if (menuIndex > 1) menuIndex = 0;
        if (menuIndex == 1) {
          lcd.setCursor(0, 1);
          lcd.print("> Regresar");
          lcd.setCursor(0, 0);
          lcd.print(" No hay registros");
        } else {
          lcd.setCursor(0, 1);
          lcd.print("  Regresar");
        }
      } else {
        if (menuIndex < 0) menuIndex = numPorciones;
        if (menuIndex > numPorciones) menuIndex = 0;
        for (int i = 0; i < 2; i++) {
          int idx = (scrollIndex + i) % (numPorciones + 1);
          lcd.setCursor(0, i);
          if (idx == menuIndex) lcd.print("> ");
          else lcd.print("  ");
          if (idx < numPorciones && porciones[idx].servido) {
            lcd.print("P");
            lcd.print(idx + 1);
            lcd.print(": ");
            lcd.print(porciones[idx].cantidadReal);
            lcd.print(" @ ");
            imprimirHora(porciones[idx].horaServido);
          } else if (idx == numPorciones) {
            lcd.print("Regresar");
          } else {
            lcd.print("                "); // limpia espacio si nada que mostrar
          }
        }
      }
      break;

    case CREAR_PORCION_HORA:
      if (menuIndex < 0) menuIndex = 23;
      if (menuIndex > 23) menuIndex = 0;
      lcd.setCursor(0, 0);
      lcd.print("Hora: ");
      if (menuIndex < 10) lcd.print("0");
      lcd.print(menuIndex);
      lcd.setCursor(0, 1);
      lcd.print("OK para minutos");
      break;

    case CREAR_PORCION_MINUTO:
      if (menuIndex < 0) menuIndex = 59;
      if (menuIndex > 59) menuIndex = 0;
      lcd.setCursor(0, 0);
      lcd.print("Minutos: ");
      if (menuIndex < 10) lcd.print("0");
      lcd.print(menuIndex);
      lcd.setCursor(0, 1);
      lcd.print("OK para cantidad");
      break;

    case CREAR_PORCION_CANTIDAD:
      if (menuIndex < 0) menuIndex = 0;
      if (menuIndex > 99) menuIndex = 99;
      lcd.setCursor(0, 0);
      lcd.print("Cantidad: ");
      lcd.print(menuIndex);
      lcd.setCursor(0, 1);
      lcd.print("OK para guardar");
      break;

    case MODIFICAR_HORA:
      if (menuIndex < 0) menuIndex = 23;
      if (menuIndex > 23) menuIndex = 0;
      lcd.setCursor(0, 0);
      lcd.print("Hora: ");
      if (menuIndex < 10) lcd.print("0");
      lcd.print(menuIndex);
      lcd.setCursor(0, 1);
      lcd.print("OK para minutos");
      break;

    case MODIFICAR_MINUTO:
      if (menuIndex < 0) menuIndex = 59;
      if (menuIndex > 59) menuIndex = 0;
      lcd.setCursor(0, 0);
      lcd.print("Minutos: ");
      if (menuIndex < 10) lcd.print("0");
      lcd.print(menuIndex);
      lcd.setCursor(0, 1);
      lcd.print("OK para cantidad");
      break;

    case MODIFICAR_CANTIDAD:
      if (menuIndex < 0) menuIndex = 0;
      if (menuIndex > 99) menuIndex = 99;
      lcd.setCursor(0, 0);
      lcd.print("Cantidad: ");
      lcd.print(menuIndex);
      lcd.setCursor(0, 1);
      lcd.print("OK para guardar");
      break;
  }
}


void handleSelect() {
  switch (menuState) {
    case MAIN_MENU:
      if (menuIndex == 0) {
        menuState = VER_PORCIONES;
        menuIndex = 0;
        scrollIndex = 0;
      } else {
        //menuState = REGISTRO;
        serv_porc(1);
        menuIndex = 0;
        scrollIndex = 0;
      }
      break;

    case VER_PORCIONES:
      if (numPorciones == 0) {
        // (opción única: crear nueva o regresar, como ya lo tenías)
        if (menuIndex == 0) {
          menuState = CREAR_PORCION_HORA;
          menuIndex = 0;
        } else {
          menuState = MAIN_MENU;
          menuIndex = 0;
        }
      } else {
        // ahora con 3 tipos de opciones
        if (menuIndex < numPorciones) {
          // seleccionó una porción existente
          selectedPorcion = menuIndex;
          menuState = PORCION_OPCIONES;
          menuIndex = 0;
        } else if (menuIndex == numPorciones) {
          // “Crear nueva”
          menuState = CREAR_PORCION_HORA;
          menuIndex = 0;
        } else {
        // “Regresar”
        menuState = MAIN_MENU;
        menuIndex = 0;
        }
      }
      scrollIndex = 0;   // opcional: reinicia scroll al entrar al submenú
      break;

    case PORCION_OPCIONES:
      if (menuIndex == 0) {
        menuState = MODIFICAR_HORA;
        editHora = porciones[selectedPorcion].horaMinutos / 60;
        editMinuto = porciones[selectedPorcion].horaMinutos % 60;
        menuIndex = editHora;
      } else if (menuIndex == 1) {
        // Eliminar porción
        for (int i = selectedPorcion; i < numPorciones - 1; i++) {
          porciones[i] = porciones[i + 1];
        }
        numPorciones--;
        Serial.println("Llamando a strc_a_json desde case Porcion_opciones");
        strc_a_json();
        menuState = VER_PORCIONES;
        menuIndex = 0;
      } else {
        menuState = VER_PORCIONES;
        menuIndex = 0;
      }
      break;

    case Dispensar:
      if (menuIndex == 1 || numPorciones == 0) {
        menuState = MAIN_MENU;
        menuIndex = 0;
      }
      break;

    case CREAR_PORCION_HORA:
      editHora = menuIndex;
      menuIndex = 0;
      menuState = CREAR_PORCION_MINUTO;
      break;

    case CREAR_PORCION_MINUTO:
      editMinuto = menuIndex;
      menuState = CREAR_PORCION_CANTIDAD;
      menuIndex = 0;
      break;

    case CREAR_PORCION_CANTIDAD:
      {
        Porcion p;
        p.horaMinutos = editHora * 60 + editMinuto;
        p.cantidad = menuIndex;
        p.servido = false;
        p.cantidadReal = p.cantidad;
        p.horaServido = -1;
        if (numPorciones < 10) {
          porciones[numPorciones++] = p;
        }
        Serial.println("llamando a strc_a_json desde case crear_porcion_cantidad");
        strc_a_json();
        menuState = VER_PORCIONES;
        menuIndex = numPorciones - 1;
      }
      break;

    case MODIFICAR_HORA:
      editHora = menuIndex;
      menuIndex = 0;
      menuState = MODIFICAR_MINUTO;
      break;

    case MODIFICAR_MINUTO:
      editMinuto = menuIndex;
      menuState = MODIFICAR_CANTIDAD;
      menuIndex = porciones[selectedPorcion].cantidad;
      break;

    case MODIFICAR_CANTIDAD:
      porciones[selectedPorcion].horaMinutos = editHora * 60 + editMinuto;
      porciones[selectedPorcion].cantidad = menuIndex;
      menuState = VER_PORCIONES;
      Serial.println("Llamando a funcion strc_a_json desde case modificar_cantidad");
      strc_a_json();
      menuIndex = selectedPorcion;
      break;
  }
}


void imprimirHora(int totalMinutos) {
  int h = totalMinutos / 60;
  int m = totalMinutos % 60;
  if (h < 10) lcd.print("0");
  lcd.print(h);
  lcd.print(":");
  if (m < 10) lcd.print("0");
  lcd.print(m);
}


/*********************************************************************
* Funcion utilizada para obtener la fecha con hora, desde una api
* devuelve un String similar a este:
* {"year":2025,"month":5,"day":23,"hour":18,"minute":50,"seconds":17,"milliSeconds":177,"dateTime":"2025-05-23T18:50:17.1775725"},
*  "date":"05/23/2025","time":"18:50","timeZone":"America/Costa_Rica","dayOfWeek":"Friday","dstActive":false}
*********************************************************************/
String get_time (){
  int intentos=0;
  int httpCode = 0;
  while (intentos <=100 && httpCode<=0){
    Serial.print("connecting to get time: ");   
    Serial.println("Google");
    delay(0);      
    // Use WiFiClient class to create TCP connections   
    WiFiClientSecure client;   
    client.setInsecure();
    HTTPClient http;
    http.begin(client, link_hora);
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    httpCode = http.GET();
    delay(0);
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Lectura de la hora exitosa. Datos recibidos: ");
      Serial.println(payload);
    } else {
      Serial.printf("Error en la solicitud HTTP: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    intentos ++;
  }
  return payload;
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
    Serial.println("Google");      
    // Use WiFiClient class to create TCP connections   
    WiFiClientSecure client;   
    client.setInsecure();
    HTTPClient http;
    http.begin(client, link_servidor);
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
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Dispensando");
  Serial.println("Sirviendo porcion: ");
  digitalWrite(12, HIGH);

  for (int i=1; i<=cantidad;i++){
    Serial.print(String(i)+" ");
    if (dtc_prox()==0){
      while(dtc_prox()==0){
        delay(5);
        //TODO: Agregar millis para detectar cuando el motor se queda trabajo o el mecanismo no da las vueltas completas
      }
    }

    while (dtc_prox()==1){
    }
  }
  digitalWrite(12, LOW);

  Serial.println("Se ha terminado de servir las porciones");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Listo");
}


/*********************************************************************
* Se utiliza esta funcion cuando se desea escribir nueva información al servidor de google
* La función no devuelve ningun valor
* La funcion recibe un unico valor, que corresponde al json que se desea subir
**********************************************************************/
void upload_data(String json){
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, link_servidor);
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  http.addHeader("Content-Type", "application/json");
  int code = http.POST(json);

  if (code > 0) {
    String resp = http.getString();
    Serial.printf("HTTP %d, respuesta: %s\n", code, resp.c_str());
  } else {
    Serial.printf("Error en POST: %s\n", http.errorToString(code).c_str());
  }
  http.end();
}

