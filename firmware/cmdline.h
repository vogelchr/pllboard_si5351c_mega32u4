#ifndef CMDLINE_H
#define CMDLINE_H

#include <stdio.h>

extern FILE cmdline_out_stream;

extern void cmdline_eat_char(unsigned char c);
extern void cmdline_tick(void);


#endif