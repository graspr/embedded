// read the MCP3304 in quad differential mode, non-bit-banging version, to be modified for single channel max11100
//max11100 datasheet: http://datasheets.maximintegrated.com/en/ds/MAX11100.pdf
//#include <Serial.h>
#include <SPI.h>

#include <HardwareSerial.h>

long ticks = 0;

// for Uno
#define SELPIN 10    // chip-select
#define DATAOUT 11   // MOSI 
#define DATAIN 12    // MISO 
#define SPICLOCK 13  // Clock 

int readvalue; 


void setup(){ 
  //set pin modes 
  pinMode(SELPIN, OUTPUT); 

  // disable device to start with 
  digitalWrite(SELPIN, HIGH); 

  SPI.setClockDivider( SPI_CLOCK_DIV4 ); //Clock = 4mhz
  SPI.setBitOrder(MSBFIRST);	
  SPI.setDataMode(SPI_MODE0); //from max11100 datasheet
  SPI.begin();

  Serial.begin(115200); 
}

int counter = 0;
long tstart = 0;
void loop() {
  
//  Serial.println(tstart);
//  counter = counter + 1;
//  if (counter == 1000) {
//    long tend = millis();
//    long diff = tend - tstart;
//    Serial.print("Time to do 1000 reads:");
//    Serial.print(diff);
//    Serial.print("\n\n");
//    tstart = millis();
//    counter = 0;
//  }
  read_adc(); 

//  long tcnv = millis() - ticks;
//  Serial.print("Time taken to read was: ");
//  Serial.println(tcnv);

//  delay(1);  //we're trying to read every 100ms.
}

// non-bit-banging version
// maximum clock frequency is 2.1 MHz @ 5V
//
// this is SPI_CLOCK_DIV8

void print_binary(int v, int num_places)
{
    int mask=0, n;

    for (n=1; n<=num_places; n++)
    {
        mask = (mask << 1) | 0x0001;
    }
    v = v & mask;  // truncate v to specified number of places

    while(num_places)
    {

        if (v & (0x0001 << num_places-1))
        {
             Serial.print("1");
        }
        else
        {
             Serial.print("0");
        }

        --num_places;
    }
}

byte b1 = 0, b2 = 0, c3=0;
int read_adc() {
  b1 = -1;
  b2 = -1; //set to easy to debug values when things go wrong.
  c3 = -1;

  //BEGIN Read
  digitalWrite (SELPIN, LOW); // Select adc
  c3 = SPI.transfer(11); //Account for initial byte of all zeros
  // MSBs - high byte
  b1 = SPI.transfer(11);
  // read low byte
  b2 = SPI.transfer(11);
  digitalWrite(SELPIN, HIGH); // turn off adc
  //END Read

//  Serial.print(millis());
//  Serial.print(",");
  print_binary(b1, 8);
//  Serial.print(b1, BIN); 
  Serial.print(",");
  print_binary(b2, 8);
//  Serial.print(b2, BIN); 
//  Serial.print(",");
  Serial.println(" ");

  return 0;
}
