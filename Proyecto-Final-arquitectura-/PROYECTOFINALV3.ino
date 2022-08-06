
#include <StateMachineLib.h>
#include "AsyncTaskLib.h"
#include "SEGURIDAD.h"
#include "MENU.h"
#include "SENSORES.h"



//------------Maquina de estado-----------------------
// Alias
enum State {
  PosicionA = 0,
  PosicionB = 1,
  PosicionC = 2,
  PosicionD = 3
};
enum Input {  
  Reset=0,
  Forward=1,
  Backward=2,
  Unknown=3,
  };
// Stores last user input
Input input;
//Crear una máquina de estado
StateMachine stateMachine(4, 6);


boolean down=false;
unsigned long tiempoInicioKey=0;
unsigned long tiempoFinalKey=0;


//Asynctask es la opcion para una tarea asincrona de la temperatura
AsyncTask asyncTemperatura(2000, true, sensorTemperatura );
AsyncTask asyncLuz(2000, true, sensorLuz );
AsyncTask asyncMicrofono(2000, true, sensorMicrofono);


void setup(){
  Serial.begin(9600);
  Serial.println(Pass);
  // set up the LCD’s number of columns and rows:
  lcd.begin(16, 2);
  //Activa los leds y el boton
  pinMode(ROJO, OUTPUT);
  pinMode(VERDE, OUTPUT);
  pinMode(AZUL, OUTPUT);
  pinMode(buzzer,OUTPUT);
  configurarStateMachine();
}


void loop(){

  actualizarStateMachine();
 
}

//---------------------------------Máquina de estado---------------------------------
//Auxiliar output functions that show the state debug
void outputA(){//seguridad
  Serial.println("A   B   C   D");
  Serial.println("X            ");
  Serial.println();
  input=Input::Unknown;
  lcd.clear();
}
void outputB(){//configuracion
  Serial.println("A   B   C   D");
  Serial.println("    X        ");
  Serial.println();
 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Configuracion");
  delay(2000);
  lcd.clear();
  input=Input::Unknown;
}
void outputC(){//taskManager
  Serial.println("A   B   C   D");
  Serial.println("        X    ");
  Serial.println();
  input=Input::Unknown;
  //Inicia las tareas para la temperatura y luz
  lcd.clear();
  asyncTemperatura.Start();
  asyncLuz.Start();
  asyncMicrofono.Start();
}
void outputD(){//alarm
  Serial.println("A   B   C   D");
  Serial.println("            X");
  Serial.println();
  lcd.clear();
  input=Input::Unknown;
}
//Acciones de los estados
void estadoA(){
  //control de acceso
  Seguridad();
  input=Input::Forward;
}
void estadoB(){
  //menú de configurar
  apagarLeds();
    ejecutarMenu();  
      input=Input::Forward;
}


void estadoC(){//Control de tareas de sensores
  actualizarTareas();
}
void estadoD(){//Alarma
  
  alarma();
  input=Input::Forward;
  delay(4000);
}


//Confugurar maquina de estado
void configurarStateMachine(){
  Serial.begin(9600);
  // Add transitions
  stateMachine.AddTransition(PosicionA, PosicionB, []() { return input == Forward; });
  stateMachine.AddTransition(PosicionB, PosicionA, []() { return input == Backward; });
  stateMachine.AddTransition(PosicionB, PosicionC, []() { return input == Forward; });
  stateMachine.AddTransition(PosicionC, PosicionB, []() { return input == Backward; });
  stateMachine.AddTransition(PosicionC, PosicionD, []() { return input == Forward; });
  stateMachine.AddTransition(PosicionD, PosicionB, []() { return input == Forward; });
  // Add actions
  stateMachine.SetOnEntering(PosicionA, outputA);
  stateMachine.SetOnEntering(PosicionB, outputB);
  stateMachine.SetOnEntering(PosicionC, outputC);
  stateMachine.SetOnEntering(PosicionD, outputD);
  stateMachine.SetOnLeaving(PosicionA, []() {Serial.println("Leaving A"); });
  stateMachine.SetOnLeaving(PosicionB, []() {Serial.println("Leaving B"); });
  stateMachine.SetOnLeaving(PosicionC, []() {Serial.println("Leaving C"); });
  stateMachine.SetOnLeaving(PosicionD, []() {Serial.println("Leaving D"); });
  // Initial state
  stateMachine.SetState(PosicionA, false, true);
}

// Actualiza el estado de la máquina de estado
void actualizarStateMachine(){
  //captura el estado
  int currentState = stateMachine.GetState();
  switch (currentState)
  {
    case PosicionA: estadoA();
                    break;
    case PosicionB: estadoB();
                    break;
    case PosicionC: estadoC();
                    break;
    case PosicionD: estadoD();
                    break;
    default:        Serial.println("estado desconocido");
                    break;
  }
  stateMachine.Update();
}


void actualizarTareas(){
    asyncTemperatura.Update();
    asyncLuz.Update();
    asyncMicrofono.Update();
    if(tareaAsincronica()){
      //Serial.println("downKey");
      input=Input::Backward; //volver a la configuracion
    }
    if(input!=3){// si el estado cambia, las tareas se finalizan
      asyncTemperatura.Stop();
      asyncLuz.Stop();
      asyncMicrofono.Stop();
      analogWrite(ROJO,0);
      analogWrite(VERDE,0);
      analogWrite(AZUL,0);
    }
}

boolean tareaAsincronica(){
  char key=keypad.getKey();
  if(key){
    if(key=='#'){
      if(!down){
        tiempoInicioKey=millis();//Cambiar para que se cumpla
        down=true;
      }else{
        tiempoFinalKey=millis();
      }
    }else{
      tiempoInicioKey=tiempoFinalKey=0;
      down=false;
    }
    if(tiempoFinalKey>tiempoInicioKey+5000 && tiempoInicioKey!=0){
      tiempoInicioKey=tiempoFinalKey=0;
      down=false;
      return true;
    }
    return false;
  }
  else{
      tiempoInicioKey=tiempoFinalKey=0;
      down=false;
      return false;
  }
}
//---------------------------------Calculos de temperatura, luz, microfono---------------------------------
//Funcion para la temperatura

void ejecutarSensores(){
  //int indexS = 0;
  char key = keypad.getKey();
  if (key){
      if(key=='*'){
        input=Input::Backward;
      }
  }    
}



void sensorTemperatura(){
  apagarLeds();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
   //read thermistor value
  long a =1023 - analogRead(A0);
  //the calculating formula of temperature
  float tempC = beta /(log((1025.0*10/a-10) / 10)+beta/298.0)-273.0;
  Serial.print("Temperatura FF5: ");
  Serial.println(tempC);
  ejecutarSensores();
//lcd.setCursor(0, 1);
      lcd.print(tempC);
      lcd.print("c");
    if(tempC>temperaturaAlta){
      input=Input::Forward;
    }else if(tempC>temperaturaBaja){
      encenderAzul();  
    }else{
      encenderVerde();      
    }
    
}


void sensorMicrofono(){
  int value = analogRead(A1);
  Serial.println(value);
  lcd.setCursor(0, 1);
  lcd.print("Microfono: ");
  int Micro = analogRead(A1);
  Serial.print("Micro: ");
  Serial.println(Micro);
  lcd.print(Micro);
  lcd.print("dB");
}

//Funcion para  la luz
void sensorLuz(void){
  lcd.setCursor(0, 1);
  lcd.print("Luz: ");
  int luz=analogRead(A2);
Serial.print("Luz: ");
  Serial.println(luz);
  lcd.print(luz);
  lcd.print("p");
  
}
