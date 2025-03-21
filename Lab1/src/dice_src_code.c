#include <pic14/pic12f683.h>


void delay (unsigned int tiempo);    //Función para realizar los retardos
void mostrar_numero(short int num);  //Función utilizada para mostrar los numeros en los displays
  
static short int click_dtc = 0;      //Para almacenar el valor de GP5, Input de click
static short int reg_num_gen[10];    //Buffer para almacenar los valores generados
static short int indice = 0;         //Indice utilizado para almacenar los numeros generados en el buffer
static short int num_inv = 0;        //Para indicar si un numero generado es invalido porque ya existe en el buffer
static short int counter = 1;        //Contador que aumenta cada ciclo while, utilizado como generador de numeros pseudo aleatorio
static unsigned int time = 150;      //Variable de tiempo utilizada para hacer retardos
static short int i;                  //Indice utilizado para cada ciclo for
short int numero_aleatorio = 1;

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

    

        while (1){ 
    

            while (click_dtc == 0){
                
                //Este contador se aumenta cada click y se reinicia cuando llega a 10
                //Se utiliza como numero pseudo-aleatorio
                counter++;
                if (counter==7){
                    counter=1;
                }
                click_dtc = GP5;
            }
    
    
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



     
       
       
        /*
        GPIO=0b111111;
        //Este contador se usa como generador de numero pseudoaleatorios
        numero_aleatorio++;
        if (numero_aleatorio==7){
            numero_aleatorio = 0;
        }

        
        switch (numero_aleatorio){
            case 1: GPIO = 0b000000; break;
            case 2: GPIO = 0b000011; break;
            case 3: GPIO = 0b000111; break;
            case 4: GPIO = 0b001111; break;
            case 5: GPIO = 0b011111; break; 
            case 6: GPIO = 0b111111; break;
            default: GPIO = 0b000000;
        }
    
        delay(10000);


        
        //Esperar al click
        while (click_dtc == 0){
            //Este contador se aumenta cada click y se reinicia cuando llega a 10
            //Se utiliza como numero pseudo-aleatorio
            counter++;
            if (counter==10){
                counter=0;
            }
            click_dtc = GP5;
        }
        
        if (indice == 10){  //Entrar si ya se tienen 10 numero almacenados
            indice = 0;
            for (i=0; i<10; i++){
                reg_num_gen[i]=70;
            }
            GPIO = 0b011111;
            delay(time);
            mostrar_numero(99);
            delay(time);
            GPIO = 0b011111;
            delay(time);
            mostrar_numero(99);
            delay(time);
            GPIO = 0b011111;
            delay(time);
            mostrar_numero(99);
            delay(time);
            GPIO = 0b011111;
        }else{ //Si no hay 10 numeros generados, guardar el nuevo y comprobar que no se tiene almacenado ya
            do{
                num_inv=0;
                for (i=0; i<indice+1; i++){
                    if (counter==reg_num_gen[i]){
                        num_inv=1;
                    }
                }
                //Si ya se tiene almacenado, aumentar en 1 y volver a comprobar que no se tiene almacenado ese otro numero
                if (num_inv==1){
                    counter++;
                }
                if (counter==10){
                    counter=0;
                }
            }while (num_inv==1);
            mostrar_numero(counter);
            reg_num_gen[indice] = counter;
            indice++;
        } 
        //Esperar a que se suelte el click actual
        while (click_dtc == 1){
            click_dtc = GP5;
       }
       */
    } 
}

void mostrar_numero(short int num){
    switch (num){
        case 0: GPIO = 0b000000; break;
        case 1: GPIO = 0b000001; break;
        case 2: GPIO = 0b000010; break;
        case 3: GPIO = 0b000011; break;
        case 4: GPIO = 0b000100; break; 
        case 5: GPIO = 0b000101; break;
        case 6: GPIO = 0b000110; break;
        case 7: GPIO = 0b000111; break;
        case 8: GPIO = 0b010000; break;
        case 9: GPIO = 0b010001; break;
        case 99: GPIO = 0b010010; break; 
        default: GPIO = 0b000000;
    }
}

void delay(unsigned int tiempo)
{
    unsigned int i;
    unsigned int j;
    
    for(i=0;i<tiempo;i++)
       for(j=0;j<1275;j++);
}



