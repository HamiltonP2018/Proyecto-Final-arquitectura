#include <EEPROM.h>
#include <RotaryEncoder.h>

//encoder
#define PIN_IN1 38 //clk
#define PIN_IN2 39 //data
#define sw 40
#define led 10

RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
int soundPin=A2;
int maxOpc = 6;
int antiguoClk;
int antiguosw;
int count = 0;
int pos = 0;
String menuOpc[] = {"1.UTempH", "2.UTempL", "3.UmbLuz", "4.UmbSonido", "5.Reset","6.Continuar"};
String menuOpcEsc[] = {"TempH", "TempL", "Luz", "Sonido", "Reset","Continuar"};

char numeroTemp[3]="00";
float temperaturaAlta=30;
char numeroTempL[3]="00";
float temperaturaBaja=25;

bool continuar = false;

char numeroLuz[4]="000";
float luzValor=200;

char numeroMicro[3]="00";
float sonido=30;
int buttonState = 1;

void InicializarMenu();
void ejecutarMenu();
void LeerEeprom();
void reset();
void mainMenu();
void guardarEeprom();
void menu();
void encoderMenu();
int encoderValor(int sensor);
void run_option(char opc);
void TemperaturaHigh();
void TemperaturaLow();
void LuzVar();
void SonidoVar();
float numer(char caracter[],int tam);


void InicializarMenu() {
      Serial.begin(9600);
      lcd.begin(16,2);
      pinMode(ROJO, OUTPUT);
      pinMode(AZUL, OUTPUT);
      pinMode(VERDE, OUTPUT);
      digitalWrite(AZUL, HIGH);
      digitalWrite(AZUL, LOW);
      pinMode(led, OUTPUT);
      pinMode(PIN_IN1, INPUT);
      pinMode(PIN_IN2, INPUT);
      pinMode(sw, INPUT);
      antiguoClk = digitalRead(PIN_IN1);
      antiguosw = digitalRead(sw);
      LeerEeprom();
      menu();
}

void ejecutarMenu()
{ 
  
  LeerEeprom();
  InicializarMenu();

  do{
      mainMenu();
      
    }while(continuar == false);

     continuar = false;
   
    
  }


/*
  * Procedimiento que lee en la memoria eprom los umbrales de los sensores<br>
  * <b>post: </b> procedimiento que se encarga de analizar el valor de los 
  * umbrales de y ponerlos en la memoria eeprom
  * 
*/
void LeerEeprom(){
     temperaturaAlta = EEPROM.read(0);
     temperaturaBaja = EEPROM.read(1);
     luzValor = (EEPROM.read(2)*10)+EEPROM.read(3);
     sonido = EEPROM.read(4);
}


/*
  * Procedimiento que resetea los umbrales <br>
  * <b>post: </b> procedimiento que se encarga de poner los umbrales 
  * de los sensores con los valores predeterminados
  * 
*/

void reset(){
       EEPROM.write(0, 30); 
       EEPROM.put(1, 25); 
       EEPROM.put(2, 300/10); 
       EEPROM.write(3, 300%10);
       EEPROM.write(4, 30); 
}


/*
  * Procedimiento que guardar los valores de los umbrales<br>
  * <b>post: </b> procedimiento que se encarga de poner los umbrales 
  * de los sensores con los valores dados por el usuario
  * 
*/  
void guardarEeprom(){
       EEPROM.write(0, temperaturaAlta);   
       EEPROM.write(1, temperaturaBaja); 
       EEPROM.write(2, luzValor/10); 
       EEPROM.write(3, (int)luzValor%10);
       EEPROM.write(4, sonido); 
}
void mainMenu(){
      encoderMenu();
      char key = keypad.getKey();
      if (key){
        if((key- '0')==count+1 ||(key- '0')==count+2)
          run_option(key);
      }
}

/*
  * Procedimiento que se encarga de mostrar el menu<br>
  * <b>post: </b> procedimiento que se encarga de mostrar el 
  * menu cada dos opciones y las imprmie en el LCD, validando que no se salga del 
  * rango de opciones maximas
*/

void menu(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(menuOpc[count]);
      
    lcd.setCursor(0,1);
    lcd.print(menuOpc[count+1]);
}

/*
  * Procedimiento que se encarga de mover el menu<br>
  * <b>post: </b> procedimiento que se encarga de mover el 
  * menu cada dos opciones y las imprmie en el LCD, validando que no se salga del 
  * rango de opciones maximas
*/
void encoderMenu(){
  int state_clk = digitalRead(PIN_IN1);
  int state_dt = digitalRead(PIN_IN2);
  
  if(antiguoClk == HIGH && state_clk == LOW){
    if(state_dt == LOW){
      count--;
    }else{
      count++;
    }
    
    if(count < 0) count = maxOpc-2;
    else if(count > maxOpc-2) count = 0;
    
    menu();
  }

  delay(5);
  antiguoClk = state_clk;
}
//funcion que ejecuta la opcion del menu

/*
  * Funcion que se encarga de retornar el valor del encoder<br>
  * <b>post: </b> funcion que detecta en que valro se encuentra 
  * el encodes para retornar este valor
*/
int encoderValor(int sensor){
      int varion = 0;
      int pos = sensor;
      encoder.tick();
      int newPos = encoder.getPosition();
      if (pos != newPos) {
         newPos = newPos*(1); 
        pos = newPos;
      } 
      varion = pos;  
      delay(5);
      return varion;
}


 /*
  * Procedimiento que ejecuta el menu<br>
  * <b>post: </b> procedimiento que se encarga de ejecutar 
  * la opcion seleccionada por el usuario
  * @parametri char opc
*/


void run_option(char opc){
  switch (opc){
  case '1':
    TemperaturaHigh();
    break;
  case '2':
    TemperaturaLow();
    break;
  case '3':
    LuzVar();
    break;
  case '4':
  SonidoVar();
    break;
  case '5':
    lcd.setCursor(0,0);
    lcd.print(menuOpcEsc[4]);
    lcd.print("               ");
    reset();
    LeerEeprom();
    lcd.setCursor(0,1);
    lcd.print("                  ");
    delay(2000);
    break;
  case '6':
    lcd.setCursor(0,0);
    lcd.print(menuOpcEsc[5]);
    lcd.print("               ");
    guardarEeprom();
    lcd.setCursor(0,1);
    lcd.print("                  ");
    delay(2000);
    continuar = true;
    break;
  }
  menu();
}



//// sensores

 /*
  * Procedimiento que acomoda el umbral alto de temperatura<br>
  * <b>post: </b> procedimiento que se encarga de leer el nuevo valor
  * del umbral alto de temperatua
*/
void TemperaturaHigh(){
 int value = 0;
  int temp = temperaturaAlta;
  char bandera='x';
  lcd.setCursor(0,0);
  lcd.print(menuOpcEsc[0]);
  lcd.print(":");
  lcd.print(temperaturaAlta);
  lcd.print("c");
  lcd.setCursor(0, 1);
  lcd.print("                        ");
  while(bandera!='*'){    
    if (value != encoderValor(temperaturaAlta)){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(menuOpcEsc[0]);
      lcd.print(":");
      lcd.print(temperaturaAlta);
      lcd.print("c");
      lcd.setCursor(0, 1);
      lcd.setCursor(0, 1);
      temp = temperaturaAlta + value;
      lcd.print(temp);
      lcd.setCursor(0, 1);
      value = encoderValor(temperaturaAlta);
    }
    char key = keypad.getKey();
    if (key){
      if(key=='#'){
        if(temp>temperaturaBaja && temp!=0){
          temperaturaAlta =  temp;
        
          guardarEeprom();
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(menuOpcEsc[0]);
          lcd.print(":");
          lcd.print(temperaturaAlta);
          lcd.print("c");

        }
        lcd.setCursor(0, 1);
      lcd.println("                        ");
        pos=0;
      }else if(key=='*')
        bandera='*';
       
    }
    if(pos == 3)
      pos=0;
  }
  pos=0;
   encoder.setPosition(0);
}

//configuracion de temperatura low
 /*
  * Procedimiento que acomoda el umbral bajo de temperatura<br>
  * <b>post: </b> procedimiento que se encarga de leer el nuevo valor
  * del umbral bajo de temperatua
*/

void TemperaturaLow(){
  int value = 0;
  int temp = temperaturaBaja;
  char bandera='x';
  lcd.setCursor(0,0);
  lcd.print(menuOpcEsc[1]);
  lcd.print(":");
  lcd.print(temperaturaBaja);
  //Serial.println(temperaturaBaja);
  lcd.print("c");
  lcd.setCursor(0, 1);
  lcd.print("                        ");
  //lcd.print(encoderValor());
  while(bandera!='*'){    
    //lcd.print("                                   ");
    if (value != encoderValor(temperaturaBaja)){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(menuOpcEsc[1]);
      lcd.print(":");
      lcd.print(temperaturaBaja);
      lcd.print("c");
      //lcd.println("                        ");
      lcd.setCursor(0, 1);
      lcd.setCursor(0, 1);
      temp = temperaturaBaja + value;
      lcd.print(temp);
      //lcd.println("   ");
      lcd.setCursor(0, 1);
      value = encoderValor(temperaturaBaja);
      //encoderValor();
    }
    char key = keypad.getKey();
    if (key){
      if(key=='#'){
        if(temp<temperaturaAlta && temp!=0){
          temperaturaBaja =  temp;
        
          guardarEeprom();
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(menuOpcEsc[1]);
          lcd.print(":");
          lcd.print(temperaturaBaja);
          lcd.print("c");

        }
        lcd.setCursor(0, 1);
      lcd.println("                        ");
        pos=0;
      }else if(key=='*')
        bandera='*';

    }
    if(pos == 3)
      pos=0;
  }
  pos=0;
   encoder.setPosition(0);
}
//configuracion de luz

/*
  * Procedimiento que acomoda el umbral alto de luz <br>
  * <b>post: </b> procedimiento que se encarga de leer el nuevo valor
  * del umbral de luz
*/
void LuzVar(){
   int value = 0;
  int temp = luzValor;
  char bandera='x';
  lcd.setCursor(0,0);
  lcd.print(menuOpcEsc[2]);
  lcd.print(":");
  lcd.print(luzValor);
  lcd.print("p");
  lcd.setCursor(0, 1);
  lcd.print("                        ");
  while(bandera!='*'){    
    if (value != encoderValor(luzValor)){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(menuOpcEsc[2]);
      lcd.print(":");
      lcd.print(luzValor);
      lcd.print("p");
      lcd.println("                        ");
      lcd.setCursor(0, 1);
      lcd.setCursor(0, 1);
      temp = luzValor + value;
      lcd.print(temp);
      lcd.setCursor(0, 1);
      value = encoderValor(luzValor);
    }
    char key = keypad.getKey();
    if (key){
      if(key=='#'){
        if(luzValor > 0){
          luzValor =  temp;
          guardarEeprom();
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(menuOpcEsc[2]);
          lcd.print(":");
          lcd.print(luzValor);
          lcd.print("p");

        }
        lcd.setCursor(0, 1);
      lcd.println("                        ");
        pos=0;
      }else if(key=='*')
        bandera='*';
       
    }
    if(pos == 4)
      pos=0;
  }
  pos=0;
   encoder.setPosition(0);
}
//configuracion de sonido

 /*
  * Procedimiento que acomoda el umbral alto de sonido<br>
  * <b>post: </b> procedimiento que se encarga de leer el nuevo valor
  * del umbral alto de sonido
*/
void SonidoVar(){
  int value = 0;
  int temp = sonido;
  char bandera='x';
  lcd.setCursor(0,0);
  lcd.print(menuOpcEsc[3]);
  lcd.print(":");
  lcd.print(sonido);
  lcd.print("dB");
  lcd.setCursor(0, 1);
  lcd.print("                        ");
  while(bandera!='*'){    
    if (value != encoderValor(sonido)){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(menuOpcEsc[3]);
      lcd.print(":");
      lcd.print(sonido);
      lcd.print("dB");
      lcd.println("                        ");
      lcd.setCursor(0, 1);
      lcd.setCursor(0, 1);
      temp = sonido + value;
      lcd.print(temp);
      lcd.setCursor(0, 1);
      value = encoderValor(sonido);
    }
    char key = keypad.getKey();
    if (key){
      if(key=='#'){
        if(sonido > 0){
          sonido =  temp;
          guardarEeprom();
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(menuOpcEsc[3]);
          lcd.print(":");
          lcd.print(sonido);
          lcd.print("dB");

        }
        lcd.setCursor(0, 1);
      lcd.println("                        ");
        pos=0;
      }else if(key=='*')
        bandera='*';
       
    }
    if(pos == 4)
      pos=0;
  }
  pos=0;
   encoder.setPosition(0);
}
//Funcion que convierte un arreglo de caracteres en un numero flotante
 /*
  * Procedimiento que tranformar un caracter en numero<br>
  * <b>post: </b> procedimiento que se encarga convertir un numero que
  * se encuentra en formato de caracter a un formato float
  * @parametro char caracter[],int tam
*/
float numer(char caracter[],int tam){//convierte una cadena de caracteres en numero
    float comoEntero=0;
    for(int i=0;i<tam;i++){
        comoEntero+= caracter[i] - '0';
        if(i<tam-1)
            comoEntero*=10;
    }
    return comoEntero;
}
