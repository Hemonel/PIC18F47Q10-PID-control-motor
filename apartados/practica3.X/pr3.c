/* 
 * File:   pr3.c
 * Author: eloyc
 *
 * Created on 15 de febrero de 2023, 17:23
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
/*#ifndef _XTAL_FREQ
#define _XTAL_FREQ 4000000
#endif*/
#pragma config FEXTOSC=OFF, LVP=ON, WDTE=OFF, RSTOSC=HFINTOSC_1MHZ

int velocidad_act;
unsigned int suma=0;

void configuraPA(){
    ANSELA=0; //puerto A en digital
    TRISA=0; //puerto A como salidas
}
void configuraAD(){
    ADCON1=0;
    ADCON2=0; //modo basico
    ADCON3=0; //no funciones matematicas
    ADREF=0; //referencias de convetidor, positiva=Vdd, negativa=Vss
    ADPCH=0; //selecion de ANA0 como entrada para el CAD
    ADACQ=0; //tiempo de adquisicion controlado por programa
    ADRPT=0; //conversion unica
    ADACT=0; //No disparo externo
    ADCON0=0x94; //justificacion a la derecha, permiso de funcionamiento y reloj FRC
    TRISA=0x01; //canal A.0 (ANA0 o RA0) como entrada, resto del puerto salida
    ANSELA=0x01;//canal A.0 como entrada analogica
    ADCON0=0x95; //inicia la conversion
}
void configuraT3(){
    T3CLK=0x01; //temporizador funcionando a f/4
    T3CON=0x32; //divisor de frecuencia 8, lec y escr de 16 bits
    T3GCON=0; //inhibida funcion GATE
    TMR3=0xC2F7; //carga valor 
    
    TMR3GATE=0; //entrada del gate selecionada por T3GPPS
    T3GPPS=0x15; //gate -> RC5
    ANSELCbits.ANSELC5=0; //RC5 como digital
    TRISCbits.TRISC5=1; //RC5 como entrada
    T3GCONbits.GE=1; //TMR3 es controlado por la señal de GATE seleccionada
    T3GCONbits.GPOL=1; //actua por nivel alto
    T3GCONbits.GTM=1; //la señal gate se complementa a cada flanco de entrada*/
    
    T3CONbits.ON=1; //permiso de funcionamiento
}
void configuraT1(){
    ANSELCbits.ANSELC0=0; //RC0 como digital
    TRISCbits.TRISC0=1; //RC0 como entrada
    T1CLK=0; //temporizador funcionando segun T1CKIPPS
    T1CKIPPS=0x10; //entrada del TMR1 = RC0
    T1CON=0x02; //divisor de frecuencia 1, lec y escr de 16 bits
    T1GCON=0; //inhibida funcion GATE
    TMR1=0; //sin valor inicial
}
void configuraPWM(){
    LATCbits.LATC1=0;
    TRISCbits.TRISC1=0; //configura el terminal del modulo CCP como salida
    ANSELCbits.ANSELC1=0;
    RC1PPS=0x06; //RC1->CCP2
    
    CCP2CON=0; //se inicializa a 0 el registro de control
    CCP2CONbits.CCP2MODE=12; //se establece el modo PWM en la unidad CCP
    CCPTMRSbits.C2TSEL=1; //se selecciona el timer 2
    
    T2CLKCON=1; //T2CS FOSC/4
    T2PR=0xFF; //pwm periodo
    T2CON=0x80; //prescalado=1 postescalado=1 encendido temporizador 2
    CCP2CONbits.CCP2EN=1; //habilitacion de la unidad
}
void configuraINT0(){
    ANSELBbits.ANSELB4=0; //RB4 como digital
    TRISBbits.TRISB4=1; //RB4 como entrada
    INT0PPS=12; //INT0->RB4
    INTCONbits.INT0EDG=0; //interrupcion por flancco de bajada
    IPR0bits.INT0IP=1; //prioridad alta para la interrupcion
}

void regulacion(){
    suma+=ADRES-(velocidad_act<<2); //calculo integral
    if (suma>=1024) //si el integrador es mayor o igual a 2^10
        suma=1023; 
}

void __interrupt(high_priority) interrupciones(void){
    if (PIR4bits.TMR3IF==1){ //interrupcion del timer 3
        T1CONbits.ON=0; //para TMR1
        TMR3=0xC2F7; //carga valor  en el temporizador 3
        PIR4bits.TMR3IF=0; //se pone a 0 el flag del timer3
        velocidad_act=TMR1; //guarda el valor de la velocidad
        TMR1=0; //reinicia el contador del TMR1
        T1CONbits.ON=1; //inicia TMR1
        regulacion();
        CCPR2=suma; //carga en el PWM el valor integral
        LATA=velocidad_act;
        ADCON0bits.GO=1;  //reinicia el CAD
    }
    if (PIR0bits.INT0IF==1){ //interrupcion de parada de emergencia
        if (T3CONbits.ON==1){
            T3CONbits.ON=0; //para TMR3
            ADCON0bits.GO=0; //para CAD
            T1CONbits.ON=0; //para TMR1
            CCP2CONbits.CCP2EN=0; //para pwm
            LATA=0;
        }
        else{
            T3CONbits.ON=1; //inicia TMR3
            ADCON0bits.GO=1; //inicia CAD
            T1CONbits.ON=1; //inicia TMR1
            CCP2CONbits.CCP2EN=1; //inicia pwm
            LATA=velocidad_act;
        }
        PIR0bits.INT0IF=0; //se pone a 0 el flag del INT0
    }
}

int main(int argc, char** argv) {
    configuraPA();
    configuraAD();
    configuraT3();
    configuraT1();
    configuraPWM();
    configuraINT0();
    
    INTCONbits.IPEN=1; //PERMISO PRIORIDADES
    INTCONbits.GIEH=1; //habilita interrupciones de alta prioridad
    IPR4bits.TMR3IP=1; //prioridad alta para timer3
    PIR4bits.TMR3IF=0; //se pone a 0 el flag del timer3
    PIE4bits.TMR3IE=1; //habilitacion de interrupciones para timer3
    PIR0bits.INT0IF=0; //pone a 0 el flag de interrucion
    PIE0bits.INT0IE=1; //permiso interrupcion del INT0
    
    while(1){
        continue;
    }
    return (EXIT_SUCCESS);
}

