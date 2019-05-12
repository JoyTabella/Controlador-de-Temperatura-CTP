#include <OneWire.h>
#include <DallasTemperature.h>
#include "U8glib.h"


//DISPLAY
//U8GLIB_ST7920_128X64_1X u8g(10);//este es para uno
U8GLIB_ST7920_128X64_1X u8g(12, 11, 10);  // SPI Com: SCK = en = 18, MOSI = rw = 16, CS = di = 17

//rs 10 , rw 11 , e 12

// pin donde esta conectado dq
const byte pinDatosDQ = 13;
float temperatura = 0;
float tempMinima = 23;  // Seteo de temperatura minima a elección del cliente.
float tempMaxima = 25;  // Seteo de temperatura máxima a elección del cliente.


// instancia a las clases OneWire y DallasTempmerature
OneWire oneWireObjeto(pinDatosDQ);
DallasTemperature sensorDS18B20(&oneWireObjeto);


void setup() {
  //inicio la comunicacion serie
  Serial.begin(9600);
  pinMode(3, OUTPUT);    // pin 3 como salida Led azul ... antes estaba en el 2
  pinMode(4, OUTPUT);   // pin 4 como salida Led rojo.  ... antes estaba en el 3
  pinMode(2, OUTPUT);   // pin 2 como salida Led testigo ON-OFF ... antes estaba en el 4 ..
  pinMode(5, OUTPUT);   // pin 5 Coolers x2
  pinMode(6, OUTPUT);   // pin 6 Peltier negativo
  pinMode(7, OUTPUT);   // pin 7 Peltier positivo
  pinMode(8, OUTPUT);
  digitalWrite(2, HIGH);  // Inicializo el pin 2 en cero
  digitalWrite(3, LOW); // Inicializo el pin 3 en cero
  digitalWrite(4, LOW); // Inicializo el pin 4 en uno.
  digitalWrite(5, LOW); // Inicializo pin 5 en uno, siempre prendidos los coolers.  relay 1
  digitalWrite(6, HIGH);  // peltier negativo  relay 2
  digitalWrite(7, HIGH); // peltier positivo   relay 3

 // Sonido 
  digitalWrite(8, HIGH);
  delay(300);
  digitalWrite(8, LOW);

   
   
  // Inicio el bus onewire
  sensorDS18B20.begin();


  // Display
  u8g.setColorIndex(3);         // definir intensidad, 3 es maximo
  u8g.setColorIndex(1);         // pixeles on, poniendo 0 no salen

}


void loop() {
  float tempAnterior = 0;
  temperatura = sensorDS18B20.getTempCByIndex(0);
  int estadoAnterior = 0;
  int estado = 0;
  
  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );

  // mando el comando para tomar la temperatura del sensor
  Serial.println("Mandando comando al sensor");
  sensorDS18B20.requestTemperatures(); // clase de dallastemperature
  

  //leo y muestro los datos del sensor
  Serial.print("Temperatura sensor: ");
  Serial.println(temperatura);
  // clase de dallas temperature.
  delay(2000);

  if (estadoAnterior != estado) {
    delay(5000);    
  }   
   
  if (temperatura >= tempMaxima) { // pregunta si la temperatura esta mayor o igual que X grados
    digitalWrite(6, HIGH); // apaga relay 2
    digitalWrite(7, LOW); // enciende relay 1
    estado = 1;
  }
  if (temperatura <= tempMinima) {    // pregunta si la temperatura es menor o igual a Y grados
    digitalWrite(6, LOW);      // enciende relay 2
    digitalWrite(7, HIGH);     // apaga relay 1
    estado = 2;
  }
  if (temperatura > tempMinima && temperatura < tempMaxima) {    // pregunta si la temperatura es mayor a Y y menor a X grados
    digitalWrite(6, HIGH);    // apaga relay 2
    digitalWrite(7, HIGH);    // apaga relay 1
    estado = 3;
  }

  estadoAnterior = estado;
  
}

void draw(void) {
  // Otros tipos de letras.
  //u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  u8g.setFont(u8g_font_5x8r);
  u8g.setColorIndex(3);         // definir intensidad, 3 es maximo
  u8g.setColorIndex(1);         // pixeles on, poniendo 0 no salen
  u8g.drawStr( 25, 15, "CONTROLADOR DE");
  u8g.drawStr( 30, 25, "TEMPERATURA");
  u8g.drawStr( 6, 40 , "Temperatura:");
  u8g.enableCursor();
  u8g.setPrintPos(75, 40);
  u8g.print(temperatura);

  if (temperatura <= tempMinima) {
    u8g.drawStr( 3, 55, "Sistema Calefaccionando *");
    digitalWrite(3, LOW);      // Led Azul Apaga
    digitalWrite(4, HIGH);     // Led Rojo Enciende
  } else if (temperatura >= tempMaxima) {
    u8g.drawStr(3, 55, "Sistema Refrigerando %");
    digitalWrite(3, HIGH);   // Led azul Enciende
    digitalWrite(4, LOW);    // Led Rojo Apaga
  } else {
    u8g.drawStr( 3, 55, "Sistema Relajado");
    digitalWrite(3, LOW);       // Led Azul Apaga
    digitalWrite(4, LOW);       // Led Rojo Apaga
  }
  
}

