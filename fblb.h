#ifndef _FBLB_H
#define _FBLB_H

#include <stdint.h>


// note that we use a special format here
#define EXT_OFFSET_AB_X 123000
#define EXT_OFFSET_AB_Y -300


uint8_t _fblb_read_lines(void);

void _fblb_module_select(uint8_t channel);

#endif
