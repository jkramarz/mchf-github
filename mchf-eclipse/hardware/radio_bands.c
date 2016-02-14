#include "radio_bands.h"

// Band definitions - band frequency size
const ulong size_bands[MAX_BANDS] = {
    BAND_SIZE_80,
    BAND_SIZE_60,
    BAND_SIZE_40,
    BAND_SIZE_30,
    BAND_SIZE_20,
    BAND_SIZE_17,
    BAND_SIZE_15,
    BAND_SIZE_12,
    BAND_SIZE_10,
    BAND_SIZE_6,
    BAND_SIZE_4,
    BAND_SIZE_2,
    BAND_SIZE_70,
    BAND_SIZE_23,
    BAND_SIZE_2200,
    BAND_SIZE_630,
    BAND_SIZE_160
};

// Band definitions - band base frequency value
const ulong tune_bands[MAX_BANDS] = {
    BAND_FREQ_80,
    BAND_FREQ_60,
    BAND_FREQ_40,
    BAND_FREQ_30,
    BAND_FREQ_20,
    BAND_FREQ_17,
    BAND_FREQ_15,
    BAND_FREQ_12,
    BAND_FREQ_10,
    BAND_FREQ_6,
    BAND_FREQ_4,
    BAND_FREQ_2,
    BAND_FREQ_70,
    BAND_FREQ_23,
    BAND_FREQ_2200,
    BAND_FREQ_630,
    BAND_FREQ_160
};
