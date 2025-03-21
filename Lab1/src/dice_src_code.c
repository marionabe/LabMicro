/*
Universidad de Costa Rica
Código para laboratorio #1: Introducción a microcontroladores y manejo de GPIOS
Autor: Jose Mario Navarro Bejarano
Carné: B75398
Descripción: Este es el código que permite el funcionamiento del dado diseñado para el microcontrolador PIC12F783. El funcionamiento
 consiste en esperar a que se realice un click, luego de lo cual se encienden los leds correspondientes y finalmente se genera un 
 delay que permite observar los leds. Luego se reinicia el bucle.

*/

#include <pic14/pic12f683.h>


void delay (unsigned int tiempo);    //Función para realizar los retardos
  
static short int click_dtc = 0;      //Para almacenar el valor de GP5, Input de click
static short int counter = 1;        //Contador que aumenta cada ciclo while, utilizado como generador de numeros pseudo aleatorio

int main(void)
{
    unsigned int i;
    unsigned int j;
    
    TRISIO = 0b00100000; //Poner todos los pines como salidas expecto P5
    GPIO=0b000000;       //Apagar todas las salidas
    WDTPS0=0b1;          //Configurar la cantidad de ciclos de espera del WD timer
    WDTPS1=0b1;
    WDTPS2=0b0;
    WDTPS3=0b1;

        // Bucle principal
        while (1){ 
            //Bucle para esperar click
            while (click_dtc == 0){
                counter++;
                if (counter==7){
                    counter=1;
                }
                click_dtc = GP5;
            }

            //Encender los leds correspondientes al numero seleccionado
            switch (counter){
                case 1: GPIO = 0b000001; break;
                case 2: GPIO = 0b000010; break;
                case 3: GPIO = 0b000011; break;
                case 4: GPIO = 0b000100; break;
                case 5: GPIO = 0b000101; break; 
                case 6: GPIO = 0b000110; break;
                default: GPIO = 0b000000;
            }
    
            //Generador de delay
            for(i=0;i<1000;i++){
                for(j=0;j<1275;j++){
                    counter++;
                    if (counter==7){
                        counter=1;
                    }
                }
            }

            GPIO = 0b000000;       //Apagar todas las salidas
            click_dtc = 0;         

    } 
}

