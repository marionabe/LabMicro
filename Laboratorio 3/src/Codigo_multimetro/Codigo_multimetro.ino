

double valorMultimetro1;
double valorMultimetro2;
double valorMultimetro3;
double valorMultimetro4;

int dc_1_en;
int dc_2_en;
int dc_3_en;
int dc_4_en;
int uart_en;

double temp;
String texto;
String texto_uart;

//__________________
// Esta sección del código se obtuvo del ejemplo de pc8544
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
int espacioReglones=13;

void setup() {
// Multimetro 1
pinMode(A0, INPUT);
pinMode(A1, INPUT);
pinMode(A2, INPUT);
pinMode(A3, INPUT);

pinMode(8, INPUT);
pinMode(9, INPUT);
pinMode(10, INPUT);
pinMode(11, INPUT);
pinMode(2, INPUT);

pinMode(12, OUTPUT); // LED de advertencia
digitalWrite(12, LOW); // Inicialmente apagado

display.begin();
display.setContrast(75);

Serial.begin(9600);
}

void loop() {
valorMultimetro1=analogRead(A0);
valorMultimetro2=analogRead(A1);
valorMultimetro3=analogRead(A2);
valorMultimetro4=analogRead(A3);

bool excesoTension = false;

dc_1_en=digitalRead(8);
dc_2_en=digitalRead(9);
dc_3_en=digitalRead(10);
dc_4_en=digitalRead(11);

display.setTextSize(1);
display.setTextColor(BLACK);
display.clearDisplay();


//
//__________Inicio de codigo para v1
//

// El switch se cierra para activar el modo dc

//Decidir si la tensión que se tiene es DC o AC
if (dc_1_en==1){
  //Calcular tensión con base en la fórmula obtenida
  valorMultimetro1=(0.0537*(valorMultimetro1)-24.926);
  
  //Desplegar info DC
  display.setCursor(0,0*espacioReglones);
  texto = "V1: "+String(valorMultimetro1)+ "V_DC";
  display.println(texto);

} else {
  //Reiniciar el medidor AC
  valorMultimetro1=0;
  // Medir varias veces para encontrar el valor máximo
  for (int i = 0; i<100; i++){
    temp=analogRead(A0);
    if (temp > valorMultimetro1){
      valorMultimetro1 = temp;
    }
  }
  //Calcular la tensión AC pico con base en la fórmula 
  valorMultimetro1=0.0538*valorMultimetro1-9.9656;
  //Calcular valor rms de la tensión AC
  valorMultimetro1=0.707*valorMultimetro1-7.065;
    
  //Desplegar info AC
  display.setCursor(0,0*espacioReglones);
  texto = "V1: "+String(valorMultimetro1)+ "Vrms";
  display.println(texto);
}

//Imprimir mensaje de peligro por exceso de tensión
  if ((valorMultimetro1>20 || valorMultimetro1 < -20) && dc_1_en){
    excesoTension = true;
    delay(20);
    display.clearDisplay();
    texto = "!Exc. tension!";
    display.println(texto);
    delay(4);
  } else if((valorMultimetro1>14.14) && dc_1_en==0){
    excesoTension = true;
    delay(20);
    display.clearDisplay();
    texto = "!Exc. tension!";
    display.println(texto);
    delay(4);
  }

//
//__________Fin de codigo para v1
//


//
//__________Inicio de codigo para v2
//

// El switch se cierra para activar el modo dc

//Decidir si la tensión que se tiene es DC o AC
if (dc_2_en==1){
  //Calcular tensión con base en la fórmula obtenida
  valorMultimetro2=(0.0537*(valorMultimetro2)-24.926);
  
  //Desplegar info DC
  display.setCursor(0,1*espacioReglones);
  texto = "V2: "+String(valorMultimetro2)+ "V_DC";
  display.println(texto);

} else {
  //Reiniciar el medidor AC
  valorMultimetro2=0;
  // Medir varias veces para encontrar el valor máximo
  for (int i = 0; i<100; i++){
    temp=analogRead(A1);
    if (temp > valorMultimetro2){
      valorMultimetro2 = temp;
    }
  }
  //Calcular la tensión AC pico con base en la fórmula 
  valorMultimetro2=0.0538*valorMultimetro2-9.9656;
  //Calcular valor rms de la tensión AC
  valorMultimetro2=0.707*valorMultimetro2-7.065;
    
  //Desplegar info AC
  display.setCursor(0,1*espacioReglones);
  texto = "V2: "+String(valorMultimetro2)+ "Vrms";
  display.println(texto);
}

//Imprimir mensaje de peligro por exceso de tensión
  if ((valorMultimetro2>20 || valorMultimetro2 < -20) && dc_2_en){
    excesoTension = true;
    delay(20);
    display.clearDisplay();
    texto = "!Exc. tension!";
    display.println(texto);
    delay(4);
  } else if((valorMultimetro2>14.14) && dc_2_en==0){
    excesoTension = true;
    delay(20);
    display.clearDisplay();
    texto = "!Exc. tension!";
    display.println(texto);
    delay(4);
  }

//
//__________Fin de codigo para v2
//

//
//__________Inicio de codigo para v3
//

// El switch se cierra para activar el modo dc

//Decidir si la tensión que se tiene es DC o AC
if (dc_3_en==1){
  //Calcular tensión con base en la fórmula obtenida
  valorMultimetro3=(0.0537*(valorMultimetro3)-24.926);
  
  //Desplegar info DC
  display.setCursor(0,2*espacioReglones);
  texto = "V3: "+String(valorMultimetro3)+ "V_DC";
  display.println(texto);

} else {
  //Reiniciar el medidor AC
  valorMultimetro3=0;
  // Medir varias veces para encontrar el valor máximo
  for (int i = 0; i<100; i++){
    temp=analogRead(A2);
    if (temp > valorMultimetro3){
      valorMultimetro3 = temp;
    }
  }
  //Calcular la tensión AC pico con base en la fórmula 
  valorMultimetro3=0.0538*valorMultimetro3-9.9656;
  //Calcular valor rms de la tensión AC
  valorMultimetro3=0.707*valorMultimetro3-7.065;
    
  //Desplegar info AC
  display.setCursor(0,2*espacioReglones);
  texto = "V3: "+String(valorMultimetro3)+ "Vrms";
  display.println(texto);
}

//Imprimir mensaje de peligro por exceso de tensión
  if ((valorMultimetro3>20 || valorMultimetro3 < -20) && dc_3_en){
    excesoTension = true;
    delay(20);
    display.clearDisplay();
    texto = "!Exc. tension!";
    display.println(texto);
    delay(4);
  } else if((valorMultimetro3>14.14) && dc_3_en==0){
    excesoTension = true;
    delay(20);
    display.clearDisplay();
    texto = "!Exc. tension!";
    display.println(texto);
    delay(4);
  }

//
//__________Fin de codigo para v3
//

//
//__________Inicio de codigo para v4
//

// El switch se cierra para activar el modo dc

//Decidir si la tensión que se tiene es DC o AC
if (dc_4_en==1){
  //Calcular tensión con base en la fórmula obtenida
  valorMultimetro4=(0.0537*(valorMultimetro4)-24.926);
  
  //Desplegar info DC
  display.setCursor(0,3*espacioReglones);
  texto = "V4: "+String(valorMultimetro4)+ "V_DC";
  display.println(texto);

} else {
  //Reiniciar el medidor AC
  valorMultimetro4=0;
  // Medir varias veces para encontrar el valor máximo
  for (int i = 0; i<100; i++){
    temp=analogRead(A3);
    if (temp > valorMultimetro4){
      valorMultimetro4 = temp;
    }
  }
  //Calcular la tensión AC pico con base en la fórmula 
  valorMultimetro4=0.0538*valorMultimetro4-9.9656;
  //Calcular valor rms de la tensión AC
  valorMultimetro4=0.707*valorMultimetro4-7.065;
    
  //Desplegar info AC
  display.setCursor(0,3*espacioReglones);
  texto = "V4: "+String(valorMultimetro4)+ "Vrms";
  display.println(texto);
}

//Imprimir mensaje de peligro por exceso de tensión
  if ((valorMultimetro4>20 || valorMultimetro4 < -20) && dc_4_en){
    excesoTension = true;
    delay(20);
    display.clearDisplay();
    texto = "!Exc. tension!";
    display.println(texto);
    delay(4);
  } else if((valorMultimetro4>14.14) && dc_4_en==0){
    excesoTension = true;
    delay(20);
    display.clearDisplay();
    texto = "!Exc. tension!";
    display.println(texto);
    delay(4);
  }

//
//__________Fin de codigo para v4
//

if (excesoTension) {
  digitalWrite(12, HIGH);
} else {
  digitalWrite(12, LOW);
}

  display.display();
  

uart_en=digitalRead(2);
if (uart_en==1){
  //Enviar info a pc
  texto_uart=String(valorMultimetro1)+" "+String(valorMultimetro2)+" "+String(valorMultimetro3)+" "+String(valorMultimetro4);
  Serial.println(texto_uart);
}

}
