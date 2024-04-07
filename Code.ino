/*
  | Arduino   | MAX7219/MAX7221 |
  | --------- | --------------- |
  | MOSI (11) | DIN (1)         |
  | SCK (13)  | CLK (13)        |
  | I/O (7)*  | LOAD/CS (12)    |
 */


#define      PUT                 *outb
#define      ss_pin_high         |= 4
#define      ss_pin_low          &= ~4
#define      complete_transfer   *spsr & 0x80
#define      no_op_address       0
#define      no_op_data          0
#define      matrix_4            4
#define      matrix_3            3
#define      matrix_2            2
#define      matrix_1            1
#define      matrix_row          i + 1
#define      matrix_4_data       arr[i]
#define      matrix_3_data       arr[i+8]
#define      matrix_2_data       arr[i+16]
#define      matrix_1_data       arr[i+24]


volatile char *dirb = 0x24;
volatile char *outb = 0x25;
volatile char *spdr = 0x4E;
volatile char *spsr = 0x4d;
volatile char *spcr = 0x4C;




//-------------------------------------------------------------------------//
//                              Delay Function                             //
//-------------------------------------------------------------------------//

void delay1(uint8_t time){
  while(time--){
    for(volatile uint8_t i=0; i<100; i++);
  }
}




//-------------------------------------------------------------------------//
//                              SPI Functions                              //
//-------------------------------------------------------------------------//

void SPI_Init(void){

  *dirb = 0x2C;          // SCK, MOSI, SS set as Output & MISO as Input
  PUT ss_pin_high;       // SS pin is high
  *spcr |= 0x53;         // SPE = MSTR = SPR0 = SPR1 = 1
}

void SPI_Transmit(volatile uint8_t cData){

  *spdr = cData;
  while(!(complete_transfer)); // Wait until the Serial Transfer is completed
  delay1(2); 
}




//---------------------------------------------------------------------------//
//                            7219 Matrix Function                           //
//---------------------------------------------------------------------------//

void send_7219_matrix(uint8_t address, uint8_t value, uint8_t matrix_no) {

  PUT ss_pin_low;  // Make SS pin Low
  delay1(2); 

  SPI_Transmit(address);
  SPI_Transmit(value);

  while(--matrix_no){             // This while loop selects the individual 8*8 matrix or individual MAX7219 IC
    SPI_Transmit(no_op_address);  // There is no operation in the current matrix
    SPI_Transmit(no_op_data);
  }

  delay1(2);
  PUT ss_pin_high;  // Make SS pin High
}




//---------------------------------------------------------------------------//
//                                Main Function                              //
//---------------------------------------------------------------------------//

int main(){

  SPI_Init();

  volatile char arr[] = {

    0b01111111,
    0b01111111,
    0b00000011,
    0b01111111,
    0b01111111,
    0b00000011,
    0b01111111,
    0b01111111, // E

    0b00111110,
    0b01111111,
    0b01100011,
    0b00000011,
    0b00000011,
    0b01100011,
    0b01111111,
    0b00111110,  // C

    0b01111111,
    0b01111111,
    0b00000011,
    0b01111111,
    0b01111111,
    0b00000011,
    0b01111111,
    0b01111111, // E

    0b01100011,
    0b01100011,
    0b01100111,
    0b01101111,
    0b01111011,
    0b01110011,
    0b01100011,
    0b01100011,  // N

  };


  for(volatile char i=0; i<8; i++){
    send_7219_matrix((matrix_row), matrix_4_data, matrix_4);
    send_7219_matrix((matrix_row), matrix_3_data, matrix_3);
    send_7219_matrix((matrix_row), matrix_2_data, matrix_2);
    send_7219_matrix((matrix_row), matrix_1_data, matrix_1);
  }

  while(1){}
}
