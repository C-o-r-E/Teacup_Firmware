#include "fblb.h"

#include <util/delay.h>

#include "config.h"

#include	"sersendf.h"

// Added to have debug serial print
#define serprintf(...) sersendf_P(__VA_ARGS__)

uint8_t _spi_initialized;

uint8_t current_channel;

void delay_ms(unsigned char ms);


//might use this for controlling the solenoids
void _fblb_module_select(uint8_t channel)
{
	current_channel = channel;
	return;
}

uint8_t _fblb_get_selected_channel(void)
{
	return current_channel;
}

