// project config

#ifndef SRC_CONFIG_H
#define SRC_CONFIG_H

const unsigned numParams = 8;

//#define HW_VER 1
#define HW_VER 2

// enable 14bit mode, otherwise 7bit
#define CTRL_HIRES
#ifdef CTRL_HIRES
// max control value
#define VAL_MAX ((1<<14)-1)
#else
#define VAL_MAX=((1<<7)-1)
#endif

#endif // SRC_CONFIG_H
