#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define ENCODER_CLK 13
#define ENCODER_DT  14
#define ENCODER_SW  16

LiquidCrystal_I2C lcd(0x27, 16, 2);

int lastEncoderCLK = HIGH;
bool buttonPressed = false;

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
  REGISTRO
};

State menuState = MAIN_MENU;
int menuIndex = 0;
int scrollIndex = 0;
int selectedPorcion = -1;

struct Porcion {
  int cantidad = 0;
  int horaMinutos = 0; // minutos totales desde medianoche
  bool servido = false;
  int cantidadReal = 0;
  int horaServido = -1; // minutos totales cuando se sirvió
};

Porcion porciones[10];
int numPorciones = 0;

int editHora = 0;
int editMinuto = 0;

void setup() {
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  mostrarMenu();
}

void loop() {
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

  // Lectura de clic del encoder
  if (digitalRead(ENCODER_SW) == LOW && !buttonPressed) {
    delay(50); // Antirrebote simple
    if (digitalRead(ENCODER_SW) == LOW) {
      buttonPressed = true;
      handleSelect();
      mostrarMenu();
    }
  } else if (digitalRead(ENCODER_SW) == HIGH) {
    buttonPressed = false;
  }
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
      lcd.print(menuIndex == 1 ? "> Registro" : "  Registro");
      break;

    case VER_PORCIONES:
      if (numPorciones == 0) {
        if (menuIndex < 0) menuIndex = 1;
        if (menuIndex > 1) menuIndex = 0;
        lcd.setCursor(0, 0);
        lcd.print(menuIndex == 0 ? "> Crear nueva" : "  Crear nueva");
        lcd.setCursor(0, 1);
        lcd.print(menuIndex == 1 ? "> Regresar" : "  Regresar");
      } else {
        if (menuIndex < 0) menuIndex = numPorciones;
        if (menuIndex > numPorciones) menuIndex = 0;

        for (int i = 0; i < 2; i++) {
          int idx = (scrollIndex + i) % (numPorciones + 1);
          lcd.setCursor(0, i);
          if (idx == menuIndex) lcd.print("> ");
          else lcd.print("  ");
          if (idx < numPorciones) {
            lcd.print("P");
            lcd.print(idx + 1);
            lcd.print(": ");
            lcd.print(porciones[idx].cantidad);
            lcd.print(" @ ");
            imprimirHora(porciones[idx].horaMinutos);
          } else {
            lcd.print("Regresar");
          }
        }
      }
      break;

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

    case REGISTRO:
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
        menuState = REGISTRO;
        menuIndex = 0;
        scrollIndex = 0;
      }
      break;

    case VER_PORCIONES:
      if (numPorciones == 0) {
        if (menuIndex == 0) {
          menuState = CREAR_PORCION_HORA;
          menuIndex = 0;
        } else {
          menuState = MAIN_MENU;
          menuIndex = 0;
        }
      } else {
        if (menuIndex == numPorciones) {
          menuState = MAIN_MENU;
          menuIndex = 0;
        } else {
          selectedPorcion = menuIndex;
          menuState = PORCION_OPCIONES;
          menuIndex = 0;
        }
      }
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
        menuState = VER_PORCIONES;
        menuIndex = 0;
      } else {
        menuState = VER_PORCIONES;
        menuIndex = 0;
      }
      break;

    case REGISTRO:
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
