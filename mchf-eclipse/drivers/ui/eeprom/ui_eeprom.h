#ifndef __UI_EEPROM_H
#define __UI_EEPROM_H

#include "mchf_board.h"
#include "audio_driver.h"
#include "ui_driver.h"
#include "ui_rotary.h"
#include "ui_driver.h"
#include "mchf_hw_i2c2.h"

void        UiDriverLoadEepromValues(void);
uint16_t    UiDriverSaveEepromValuesPowerDown(void);
#endif