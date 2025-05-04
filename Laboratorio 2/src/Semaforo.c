#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <time.h>
#include <avr/interrupt.h>

// PORTB - Pines de LEDs
#define LDPP    PB1
#define LDAP   PB2
#define LDPD      PB3
#define LDPV    PB4
#define LDAV   PB5
#define LDVD      PB6

// PORTD - Pines de botones
#define B1      PD4
#define B2      PD5

typedef enum {
    STATE_A,  // Verde
    STATE_B,  // Verde parpadea
    STATE_C,  // Ámbar
    STATE_D   // Rojo
} state_t;

volatile state_t current_state = STATE_A;
volatile uint8_t button_request = 0;
volatile uint16_t overflow_counter = 0;
volatile uint8_t blink_counter = 0;

// Interrupciones en botones PD4 y PD5
ISR(PCINT2_vect) {
    if (PIND & (1 << B1) || PIND & (1 << B2)) {
        button_request = 1;
    }
}

// Interrupción por overflow del Timer
ISR(TIMER1_OVF_vect) {
    overflow_counter++;

    if (current_state == STATE_B && overflow_counter % 2 == 0) {
        PORTB ^= (1 << LDPV);  // Parpadea verde vehículo
    }

    if (current_state == STATE_D && overflow_counter >= 20 && blink_counter < 6) {
        PORTB ^= (1 << LDPP);  // Parpadea verde peatón
        blink_counter++;
    }
}

void apagar_leds() {
    PORTB &= ~((1 << LDPV) | (1 << LDAV) | (1 << LDVD) |
               (1 << LDPP) | (1 << LDAP) | (1 << LDPD));
}

void setup() {
    // LEDs como salida
    DDRB |= (1 << LDPV) | (1 << LDAV) | (1 << LDVD) |
            (1 << LDPP) | (1 << LDAP) | (1 << LDPD);

    DDRD &= ~((1 << B1) | (1 << B2)); // Botones como entrada

    GIMSK |= (1 << PCIE2);           // Habilita PCINT[23:16]
    PCMSK2 |= (1 << B1) | (1 << B2); // Habilita interrupciones por cambio en PD4 y PD5

    TCCR1B |= (1 << CS12);
    TIMSK |= (1 << TOIE1); // Habilita overflow

    PORTD |= (1 << B1) | (1 << B2); // Activa pull-up interno


    sei(); // Interrupciones globales
}

int main(void) {
    setup();

    while (1) {
        switch (current_state) {
            case STATE_A:
                apagar_leds();
                PORTB |= (1 << LDPV) | (1 << LDPD);
                button_request = 0;      
                while (!button_request); // Espera hasta que se presione botón
                current_state = STATE_B;
                break;

            case STATE_B:
                overflow_counter = 0;
                while (overflow_counter < 6); // ~3 s de parpadeo
                PORTB |= (1 << LDPV);         // Asegura verde encendido
                current_state = STATE_C;
                break;

            case STATE_C:
                apagar_leds();
                PORTB |= (1 << LDAV) | (1 << LDPD);
                overflow_counter = 0;
                while (overflow_counter < 6); 
                current_state = STATE_D;
                break;

            case STATE_D:
                apagar_leds();
                PORTB |= (1 << LDVD) | (1 << LDPP);
                overflow_counter = 0;
                blink_counter = 0;

                while (blink_counter < 6);      
                PORTB &= ~(1 << LDPP);  // Asegura apagado
                PORTB |= (1 << LDAP);
                overflow_counter = 0;
                while (overflow_counter < 4); // ~2 s
                button_request = 0;
                current_state = STATE_A;
                break;
        }
    }
}

