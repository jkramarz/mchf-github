#ifndef __UI_ENCODER_DRIVER_H
#define __UI_ENCODER_DRIVER_H

#include "mchf_board.h"

bool     UiDriverCheckFrequencyEncoder(void);
void     UiDriverCheckEncoderOne(void);
void     UiDriverCheckEncoderTwo(void);
void     UiDriverCheckEncoderThree(void);
void     UiDriverChangeEncoderOneMode(uchar skip);
void     UiDriverChangeEncoderTwoMode(uchar skip);
void     UiDriverChangeEncoderThreeMode(uchar skip);

#endif