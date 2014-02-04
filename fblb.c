#include "fblb.h"

#include <util/delay.h>

#include "config.h"

#include	"sersendf.h"

// Added to have debug serial print
#define serprintf(...) sersendf_P(__VA_ARGS__)

uint8_t _spi_initialized;

uint8_t current_channel;

void delay_ms(unsigned char ms);

void _spi_init(void)
{

  if(_spi_initialized == 1)
    {
      return;
    }

  //First make sure that the spi system is powered
#ifdef        PRR
  PRR &= ~MASK(PRSPI);
#elif defined PRR0
  PRR0 &= ~MASK(PRSPI);
#endif

  DDRB |= MASK(4); // set SS to output
  DDRB |= MASK(5); // set MOSI to output
  DDRB |= MASK(7); // set sck to output
  SPCR = MASK(MSTR) | MASK(SPE) | MASK(SPR0); // spi enabled, master mode
 
  //need to verify if we have to let the register settle before using spi

  _spi_initialized = 1;  
}


uint8_t _spi_send(uint8_t data)
{
  uint16_t count;

  _spi_init();
  SPDR = data; // start the transmission by loading the output byte into the spi data register
  for (count=0; (SPSR & MASK(SPIF)) == 0; count++)
    {
      //this is needed for SPI
      
      if(count > 0xff)
	{
	  return 1; //error?
	}
    }
  
  return 0;
}

//Read from LS165
uint8_t _fblb_read_lines()
{
  //Serial-in => PB4
  //Clock => PD2
  //R/W => PD3

  uint8_t inByte;

  inByte = 0;
  DDRB &= ~MASK(4);
  DDRD |= MASK(2) | MASK(3);
  
  PORTB &= ~MASK(4); //make sure B4 isnt pulled up

  //Load the data from parallel in to the shift reg
  PORTD &= ~MASK(3);
  PORTD |= MASK(3);

  //now let us read the byte (bit by bit)
  int i;
  for(i=0; i<8; i++)
    {
      //read the pin
      if(PINB & MASK(4))
	{
	  inByte |= ( 1<<(7-i) );
	}
      //clock it
        PORTD &= ~MASK(2);
	PORTD |= MASK(2);
    }
 
   return inByte; 
}


//using custom board with max350
void _fblb_module_select(uint8_t channel)
{
  uint8_t chanMask;
  uint8_t ret;
  
  //right now this is custom for 4 mirrored modules
  
  switch (channel)
    {
    case 0:
      chanMask = 0b10000001; //NO0B and NO0A closed state
      break;
    case 1:
      chanMask = 0b01000010; //NO1B and NO1A closed state
      break;
    case 2:
      chanMask = 0b00100100; //NO2B and NO2A closed state
      break;
    case 3:
      chanMask = 0b00011000; //NO3B and NO3A closed state
      break;
    default:
      chanMask = 0b00000000; //No output. All circuit are in open state
      break;
  }

  DDRA |= MASK(0);

  //set the CS low
  PORTA &= ~MASK(0); // PA0

  ret = _spi_send(chanMask);

  //set the CS HIGH
  PORTA |= MASK(0); // PA0

  if (ret != 0)
  {
      serprintf(PSTR("spi_send returned an error!\n"));
  }

  current_channel = channel;

  return;
}

uint8_t _fblb_get_selected_channel(void)
{
	return current_channel;
}

