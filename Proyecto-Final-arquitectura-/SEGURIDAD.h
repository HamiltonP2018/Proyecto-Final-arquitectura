/*
  LiquidCrystal Library - Hello World
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 This sketch prints "Hello World!" to the LCD
 and shows the time.
  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 31
 * LCD D5 pin to digital pin 32
 * LCD D6 pin to digital pin 33
 * LCD D7 pin to digital pin 34
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 * 
 * ROJO 35
 * VERDE 36
 * AZUL 37
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 modified 7 Nov 2016
 by Arturo Guadalupi
 This example code is in the public domain.
 http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld
*/
#include <LiquidCrystal.h>
#include <Keypad.h>
#define ROJO 35
#define AZUL 36
#define VERDE 37
#define beta 4090 //the beta of the thermistor
int buzzer = 22;

const int rs = 12, en = 11, d4 = 31, d5 = 32, d6 = 33, d7 = 34;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {5,4,3,2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8,7,6}; //connect to the column pinouts of the keypad
//----------------------------------------------------------
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

const char Pass[4] = {'1','2','9','8'};//"1298";
int printer = 0; // variable para imprimir en el LCD
int tries = 0;//intentos 
char PassIng[3] = "";
bool correcto = false;

bool Seguridad();
void validarPassword();
void inicializarSeguridad();
void incorrecto();
void Intentar ();
void alarma();
void LED_RGB(int r,int g,int b);
void encenderVerde();
void encenderRojo();
void encenderAzul();
void apagarLeds();


bool Seguridad(){
    inicializarSeguridad();
      do{
          validarPassword();
      }while(correcto == false);
      return true;
}

void inicializarSeguridad() {
  // set up the LCD's number of columns and rows:
      lcd.begin(16, 2);
      lcd.print("Bienvenido!");
       pinMode(ROJO, OUTPUT);
       pinMode(AZUL, OUTPUT);
       pinMode(VERDE, OUTPUT);
       digitalWrite(AZUL, HIGH);
       digitalWrite(AZUL, LOW);
}

/*
  * ciclo repetetivo que lee lo digitado por el usuario <br>
  * <b>post: </b> procedimiento que se encarga de leer la varible digitada por el usuario
  * y guardarla en un arreglo de caracteres, luego llama al metodo que valida la contraseña.
  * @see intentar();
*/
void validarPassword() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
      char key = keypad.getKey();
      if (key){
        //Serial.println(key);
        lcd.setCursor(printer, 1);
        printer = printer + 1;
      // print the number of seconds since reset:
        lcd.print("*");
       // strncat(PassIng, key, 1);
       PassIng[printer-1] = key;
        if(printer == 4){
          Intentar();
        }
      }
}



/*
  * aumenta el numero de intentos fallidos <br>
  * <b>post: </b> procedimiento que se encarga de inializar la varible @printer en cero
  * para la impresion de la siguiente contraseña a digitar, incrementa los intentos
  * apaga el led rojo y verde, luego enciende el azul.
*/

void incorrecto(){ 
      printer = 0;
      tries = tries + 1;
      char PassIng[3] = "";
        digitalWrite(VERDE, LOW);
        digitalWrite(ROJO, LOW);
        digitalWrite(AZUL, HIGH);

        correcto = false; 
      return;
  }

 /*
     * valida la contraseña digitada por el usuario<br>
     * <b>post: </b> Procedimiento donde se realiza la validacion de la contraseña 
     * digitada por el usuario con la contraseña y preestablecida, haciendo el uso de condicionales
     * si el numero de intentos supera 3 tres se bloquea el durante 5 segundos y se enciende el led rojo.
     * si la contraseña es correcta se enciende el verde y se muesstra un mensaje "CORRECTO!".
     * si no se cumplen ninguna de las anteriores se incrementa el numero de intentos 
     * se muestra un mensaje de "INCORRECTO! y se enciende el led azul"
     * @see incorrecto();
 */
  void Intentar (){
  if (tries < 2){
    if (strcmp(PassIng, Pass) == 0){
      lcd.clear();
      lcd.print("CORRECTO!");
        digitalWrite(AZUL, LOW);
        digitalWrite(ROJO, LOW);
        digitalWrite(VERDE, HIGH);
        
        correcto = true; 
    }
    else{
      Serial.println(strcmp(PassIng, Pass));
      Serial.println(PassIng);
      Serial.println(Pass);
      lcd.clear();
      lcd.print("INCORRECTO!");
      incorrecto();
    }
  }
  else{
    lcd.clear();
    lcd.print("BLOQUEADO!");
     digitalWrite(VERDE, LOW);
        digitalWrite(ROJO, HIGH);
        digitalWrite(AZUL, LOW);
    delay(2000); // 2 minutes pause
    printer = 0;
    tries = 0;
    char PassIng[3] = "";   
    lcd.clear();
      lcd.print("DESBLOQUEADO!");
    incorrecto();
    correcto = false; 
  }
}


void alarma(){
  encenderRojo();
  lcd.setCursor(1,1);
  lcd.print("Alarma sonando");

    for(int i=0;i<1000;i++)
      {
      digitalWrite(buzzer,HIGH);
      delay(1);//wait for 1ms
       digitalWrite(buzzer,LOW);
      delay(1);//wait for 1ms
      }
 

}

void LED_RGB(int r,int g,int b){
  analogWrite(ROJO,r);
  analogWrite(VERDE,g);
  analogWrite(AZUL,b);
}

void encenderVerde(){
    digitalWrite(VERDE, HIGH);
    digitalWrite(ROJO, LOW);
    digitalWrite(AZUL, LOW);
}

void encenderAzul(){
    digitalWrite(VERDE, LOW);
    digitalWrite(ROJO, LOW);
    digitalWrite(AZUL, HIGH);
}

void encenderRojo(){
    digitalWrite(VERDE, LOW);
    digitalWrite(ROJO, HIGH);
    digitalWrite(AZUL, LOW);
}

void apagarLeds(){

    digitalWrite(VERDE, LOW);
    digitalWrite(ROJO, LOW);
    digitalWrite(AZUL, LOW);
  }
