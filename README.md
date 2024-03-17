REGULACIÓN DE VELOCIDAD EN BUCLE CERRADO (BC) DE UN MOTOR DE CC CON UN CONTROL PI y PARADAS DEL SISTEMA
-

El objetivo de este proyecto es realizar un programa que segule la velocidad de un motor tomando en cuenta cual es la velocidad actual y cual es la velocidad objetivo.

El proyecto se realizó para la asignatura de Laboratorio de sistemas dixitais programables del 4º curso del Grado en Ingeniería en Electrónica Industrial y Automática.

Componentes:
-
- Placa Curiosity High Pin Count (contiene un PIC18F47Q10 de Micrchip)
- Amplificador L293
- Sensor de barrera SX-4070
- Regulador LM78L05
- Motor de cc
- Diodo BYV27
- Resistencias de 170, 470, 1k ohmios
- Potenciometro de 10k ohmios
- Condensador de 100 nF
- Fuente de alimentación de tensión entre 8 a 12 V

Esquema eléctrico
-

![esquema montaje](https://github.com/Hemonel/PIC18F47Q10-PID-control-motor/assets/153218898/a6d788b1-af94-4dec-90f0-481ce6c95623)


Funcionamiento del PIC
-

El sistema está basado en interrupciones, una para marcha/paro del sistema y otra para lectura de la velocidad actual y cálculo de nueva velocidad.
- Periféricos internos del PIC usados
  - ADCON: converidor analógico digital. Lee el valor de tensión del potenciometro.
  - TMR3: temporizador con permiso de interrupción que se configurará a 0,5 s.
  - TMR1: temporizador que funciona como contador al tener su entrada de reloj conectada al sensor de barrera.
  - CCPR2: generador de PWM que se usará para regular la velocidad del motor.
  - INT0: interrupción para entrada. Se usará con el pulsador SW1.
- Interrupciones

  - Marcha/paro
  
    Cuando se pulsa el botón SW1 de la placa se detiene o inicia la lectura y conversión.
  
  - Lectura y cálculo
  
    Cuando el TMR3 llega a su fin el sistema:
    - Lee el valor de TMR1, el valor medido es 4 veces la cantidad de vueltas realizadas (el sistema de medida son 4 aspas sobre el eje del motor).
    - Tras esto reinicia TMR3 para que interrumpa en 0,5 s y pone TMR1 a 0.
    - A continuación calcula la resta del valor objetivo de velocidad (tensión del potenciometro) menos la velocidad medida y lo suma a una variable que acumula todas las diferencias. El resultado se carga en el periferico CCPR2.
    - Finalmente se reinicia el ADC.

Datos extra
-

Se adjuntan manuales de la placa Curiosity, del PIC, del Sensor de barrera SX-4070, del regulador LM78L05 y del amplificador L293 en la carpeta manuales.

El proyecto se realizó en varias fases, se adjuntan pdfs con la descrición de los objetivos y los programas de cada parte en la carpeta apartados.
