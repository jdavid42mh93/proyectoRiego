//*************** Sistema de Riego *****************************************   
//**************************************************************************

//librerias
#include <LiquidCrystal_I2C.h> 
#include <Wire.h>
#include <RTClib.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Keypad.h>

//declaracion de variables
RTC_DS3231 rtc;

bool riegoInicio = true; 
bool riegoFin = true;

//constantes para el numero de filas y columnas
const byte filas = 4;
const byte columnas = 4;
char keys[filas][columnas] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
};

//variables para la conexion del teclado con el arduino
byte pinesFilas[filas] {9,8,7,6};
byte pinesColumnas[columnas] = {5,4,3,2};

int sistemaApagado = 10; //indicador de sistema de riego apagado
int valvulaAgua = 11; //indicador de sistema de riego encendido
int sensorAmbiente_TempHum = 12; //sensor que mide la humedad y temperatura del ambiente
int sensorTierra_Hum = A0; //sensor que mide la humedad de la tierra

//variables que mediran la temperatura y humedad del ambiente y del suelo
int temperaturaAmbiente;
int humedadAmbiente;
int humedadTierra;

int porcentajeHumedad, horaM, horaT, minuto;

LiquidCrystal_I2C lcd(0x27,20,4);
DHT dht(sensorAmbiente_TempHum, DHT11); 
Keypad teclado = Keypad(makeKeymap(keys), pinesFilas, pinesColumnas, filas, columnas);
DateTime fecha;

char pulsacion;
int contador = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  pinMode(valvulaAgua, OUTPUT);
  pinMode(sistemaApagado, OUTPUT);
  pinMode(sensorTierra_Hum, INPUT);
  
  if(!rtc.begin()){
    Serial.println("Modulo RTC no encontrado!");
    while(1);
  }
  //rtc.adjust(DateTime(__DATE__, __TIME__));
}

void loop() {
  pulsacion = teclado.getKey();
  if(pulsacion != 0){
    lcd.clear();
    delay(100);
  }
  
  if(contador == 0){ mostrarInformacion();accederMenu();}
  if(contador == 1){ menu0(); accion0();}
  if(contador == 2){ menu1(); accion1();}
  if(contador == 3){ menu2(); accion2();}
  if(contador == 4){ menu3(); accion3();}
  if(contador == 5){ menu4(); accion4();}
  if(contador == 6){ menu5(); accion5();}
  
  sistemaRiegoM();
  sistemaRiegoT();
}
void sistemaRiegoM(){
  if(fecha.hour() == horaM && fecha.minute() == minuto){
    if( riegoInicio == true){
      accionarValvula();
      riegoInicio = false;
    }
  }

  if(fecha.hour() == horaM && fecha.minute() == minuto+1){
    if( riegoFin == true){
      detenerValvula();
      riegoFin = false;
    }
  }

  if(fecha.hour() == horaM && fecha.minute() == minuto+2){
    riegoInicio = true;
    riegoFin = true;
  }
}

void sistemaRiegoT(){
  if(fecha.hour() == horaT && fecha.minute() == minuto){
    if( riegoInicio == true){
      accionarValvula();
      riegoInicio = false;
    }
  }

  if(fecha.hour() == horaT && fecha.minute() == minuto+1){
    if( riegoFin == true){
      detenerValvula();
      riegoFin = false;
    }
  }

  if(fecha.hour() == horaT && fecha.minute() == minuto+2){
    riegoInicio = true;
    riegoFin = true;
  }
}
void sensorTempHum(){
  temperaturaAmbiente = dht.readTemperature();
  humedadAmbiente = dht.readHumidity();
  lcd.setCursor(0,1); 
  lcd.print("TA:   ");
  lcd.print(temperaturaAmbiente);
  lcd.setCursor(10,1); 
  lcd.print("HA:   ");
  lcd.print(humedadAmbiente);
}

void sensorHumedad(){
  humedadTierra = map(analogRead(sensorTierra_Hum), 0, 1023, 100, 0);
  lcd.setCursor(0,2); 
  lcd.print("HT: ");
  lcd.print(humedadTierra);
  lcd.print("%");
  lcd.setCursor(10,2);
  lcd.print("          ");
  /*if (humedadTierra > 0 && humedadTierra < porcentajeHumedad ){
    accionarValvula();
  }else{
    if(humedadTierra >= porcentajeHumedad && humedadTierra < 100){
      detenerValvula();    
    }
  }*/
}

DateTime mostrarFechaHora(){
  DateTime fecha = rtc.now();
  lcd.setCursor(0,0); 
  lcd.print(fecha.day());
  lcd.print("/");
  lcd.print(fecha.month());
  lcd.print("/");
  lcd.print(fecha.year());
  lcd.print(" ");
  lcd.print(fecha.hour());
  lcd.print(":");
  lcd.print(fecha.minute());
  lcd.print(":");
  lcd.print(fecha.second());
  return fecha;
}

void accionarValvula(){
  digitalWrite(sistemaApagado, LOW);
  digitalWrite(valvulaAgua, HIGH);
  lcd.setCursor(0,3); 
  lcd.print("Sistema encendido   ");
}
void detenerValvula(){  
  digitalWrite(sistemaApagado, HIGH);
  digitalWrite(valvulaAgua, LOW);
  lcd.setCursor(0,3); 
  lcd.print("Sistema apagado   ");
}

void mostrarInformacion(){
  fecha = mostrarFechaHora();
  sensorTempHum();
  sensorHumedad();
  
}

void accederMenu(){
  if(pulsacion == '#'){contador=1;}
}

void menu0(){
  lcd.setCursor(0,0); lcd.print("     Ajustes        ");
  lcd.setCursor(0,1); lcd.print("H manana  ");
  lcd.setCursor(10,1); lcd.print("H tarde  ");
  lcd.setCursor(0,2); lcd.print("minuto    ");
  lcd.setCursor(10,2); lcd.print("% humedad");
  lcd.setCursor(0,3); lcd.print("Info.     ");
  lcd.setCursor(10,3); lcd.print("Param.   ");
}

void accion0(){
  if(pulsacion == 'A'){contador = 2;}
  if(pulsacion == 'B'){contador = 3;}
  if(pulsacion == 'C'){contador = 4;}
  if(pulsacion == 'D'){contador = 5;}
  if(pulsacion == '0'){contador = 6;}
  if(pulsacion == '*'){contador = 0;} 
}

void menu1(){
  lcd.setCursor(0,0); lcd.print("Seleccione hora");
  lcd.setCursor(0,1); lcd.print("1->08    ");
  lcd.setCursor(10,1); lcd.print("2->09    ");
  lcd.setCursor(0,2); lcd.print("3->10    ");
  lcd.setCursor(10,2); lcd.print("4->11    ");
  lcd.setCursor(0,3); lcd.print("*->Menu Anterior    ");
}

void accion1(){
  if(pulsacion == '1'){horaM = 8;}
  if(pulsacion == '2'){horaM = 9;}
  if(pulsacion == '3'){horaM = 10;}
  if(pulsacion == '4'){horaM = 11;}
  if(pulsacion == '*'){contador = 1;} 
}

void menu2(){
  lcd.setCursor(0,0); lcd.print("Seleccione hora");
  lcd.setCursor(0,1); lcd.print("1->12  ");
  lcd.setCursor(7,1); lcd.print("2->13  ");
  lcd.setCursor(14,1); lcd.print("3->14 ");
  lcd.setCursor(0,2); lcd.print("4->15  ");
  lcd.setCursor(7,2); lcd.print("5->16  ");
  lcd.setCursor(14,2); lcd.print("6->17 ");
  lcd.setCursor(0,3); lcd.print("7->18  ");
  lcd.setCursor(7,3); lcd.print("8->19  ");
  lcd.setCursor(14,3); lcd.print("*->MA ");
}

void accion2(){
  if(pulsacion == '1'){horaT = 12;}
  if(pulsacion == '2'){horaT = 13;}
  if(pulsacion == '3'){horaT = 14;}
  if(pulsacion == '4'){horaT = 15;}
  if(pulsacion == '5'){horaT = 16;}
  if(pulsacion == '6'){horaT = 17;}
  if(pulsacion == '7'){horaT = 18;}
  if(pulsacion == '8'){horaT = 19;}
  if(pulsacion == '*'){contador = 1;}
}

void menu3(){
  lcd.setCursor(0,0); lcd.print("Seleccione minuto");
  lcd.setCursor(0,1); lcd.print("1->10    ");
  lcd.setCursor(10,1); lcd.print("2->20    ");
  lcd.setCursor(0,2); lcd.print("3->30    ");
  lcd.setCursor(10,2); lcd.print("4->40    ");
  lcd.setCursor(0,3); lcd.print("5->50     "); 
  lcd.setCursor(10,3); lcd.print("6->0    ");
}

void accion3(){
  if(pulsacion == '1'){minuto = 10;}
  if(pulsacion == '2'){minuto = 20;}
  if(pulsacion == '3'){minuto = 30;}
  if(pulsacion == '4'){minuto = 40;}
  if(pulsacion == '5'){minuto = 50;}
  if(pulsacion == '6'){minuto = 0;}
  if(pulsacion == '*'){contador = 1;}
}

void menu4(){
  lcd.setCursor(0,0); lcd.print("Seleccione % humedad");
  lcd.setCursor(0,1); lcd.print("1 10% ");
  lcd.setCursor(6,1); lcd.print("2 20% ");
  lcd.setCursor(12,1); lcd.print("3 30% ");
  lcd.setCursor(0,2); lcd.print("4 40% ");
  lcd.setCursor(6,2); lcd.print("5 50% ");
  lcd.setCursor(12,2); lcd.print("6 60%  ");
  lcd.setCursor(0,3); lcd.print("7 70% ");
  lcd.setCursor(6,3); lcd.print("8 80% ");
  lcd.setCursor(12,3); lcd.print("9 90%  ");
}

void accion4(){
  if(pulsacion == '1'){porcentajeHumedad = 10;}
  if(pulsacion == '2'){porcentajeHumedad = 20;}
  if(pulsacion == '3'){porcentajeHumedad = 30;}
  if(pulsacion == '4'){porcentajeHumedad = 40;}
  if(pulsacion == '5'){porcentajeHumedad = 50;}
  if(pulsacion == '6'){porcentajeHumedad = 60;}
  if(pulsacion == '7'){porcentajeHumedad = 70;}
  if(pulsacion == '8'){porcentajeHumedad = 80;}
  if(pulsacion == '9'){porcentajeHumedad = 90;}
  if(pulsacion == '*'){contador = 1;}
}

void menu5(){
  lcd.setCursor(0,0); lcd.print("    Parametros     ");
  lcd.setCursor(0,1); lcd.print("H-M: "); lcd.print(horaM); lcd.setCursor(7,1); lcd.print("  ");
  lcd.setCursor(10,1); lcd.print("H-T: "); lcd.print(horaT); lcd.setCursor(17,1); lcd.print("  ");
  lcd.setCursor(0,2); lcd.print("Min: "); lcd.print(minuto);
  lcd.setCursor(10,2); lcd.print("% H: "); lcd.print(porcentajeHumedad); lcd.setCursor(17,2); lcd.print("   ");
  lcd.setCursor(0,3); lcd.print("*->MA");
  lcd.setCursor(10,3); lcd.print("         ");
}

void accion5(){
  if(pulsacion == '*'){contador = 1;}
}
