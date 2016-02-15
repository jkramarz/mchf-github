#include <stdio.h>
#include "./driver.h"
#include "audio_driver.h"
#include "ui_driver.h"
#include "ui_rotary.h"
#include "ui_driver.h"
#include "mchf_hw_i2c2.h"
#include "ui_menu.h"
#include "codec.h"

// Transceiver state public structure
extern __IO TransceiverState    ts;
// Frequency public
extern __IO DialFrequency       df;
// Spectrum display
extern __IO SpectrumDisplay     sd;
// Public USB Keyboard status
extern __IO KeypadState     ks;
// Encoder one public
extern __IO EncoderOneSelection        eos;
// Encoder two public
extern __IO EncoderTwoSelection        ews;
// Encoder three public
extern __IO EncoderThreeSelection      ets;

//*----------------------------------------------------------------------------
//* Function Name       : UiDriverCheckFrequencyEncoder
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
bool UiDriverCheckFrequencyEncoder(void)
{
    int         pot_diff;


    // Skip too regular read of the timer value, to avoid flickering
//  df.update_skip++;
//  if(df.update_skip < FREQ_UPDATE_SKIP)
//      return false;

//  df.update_skip = 0;

    // Load pot value
    df.value_new = TIM_GetCounter(TIM8);

    // Ignore lower value flickr
    if(df.value_new < ENCODER_FLICKR_BAND)
        return false;

    // Ignore higher value flickr
    if(df.value_new > (FREQ_ENCODER_RANGE/FREQ_ENCODER_LOG_D) + ENCODER_FLICKR_BAND)
        return false;

    // No change, return
    if(df.value_old == df.value_new)
        return false;

    UiLCDBlankTiming(); // calculate/process LCD blanking timing

#ifdef USE_DETENTED_ENCODERS
    // SW de-detent routine
    df.de_detent++;
    if(df.de_detent < USE_DETENTED_VALUE)
    {
        df.value_old = df.value_new; // update and skip
        return false;
    }
    df.de_detent = 0;
#endif

    if(ts.txrx_mode != TRX_MODE_RX)     // do not allow tuning if in transmit mode
        return false;

    if(ks.button_just_pressed)      // press-and-hold - button just pressed for "temporary" step size change (not taken effect yet)
        return false;

    if(ts.frequency_lock)
        return false;                       // frequency adjust is locked

    //printf("freq pot: %d \n\r",df.value_new);

    // Encoder value to difference
    if(df.value_new > df.value_old)
        pot_diff = +1;
    else
        pot_diff = -1;

    //printf("pot diff: %d\n\r",pot_diff);

    // Finaly convert to frequency incr/decr
    if(pot_diff < 0)
        df.tune_new -= (df.tuning_step * 4);
    else
        df.tune_new += (df.tuning_step * 4);

    // Updated
    df.value_old = df.value_new;

    return true;
}

//*----------------------------------------------------------------------------
//* Function Name       : UiDriverCheckEncoderOne
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void UiDriverCheckEncoderOne(void)
{
    char    temp[10];
    int     pot_diff;

    eos.value_new = TIM_GetCounter(TIM3);

    // Ignore lower value flickr
    if(eos.value_new < ENCODER_FLICKR_BAND)
        return;

    // Ignore lower value flickr
    if(eos.value_new > (ENCODER_ONE_RANGE/ENCODER_ONE_LOG_D) + ENCODER_FLICKR_BAND)
        return;

    // No change, return
    if(eos.value_old == eos.value_new)
        return;

#ifdef USE_DETENTED_ENCODERS
    // SW de-detent routine
    eos.de_detent++;
    if(eos.de_detent < USE_DETENTED_VALUE)
    {
        eos.value_old = eos.value_new; // update and skip
        return;
    }
    eos.de_detent = 0;
#endif

    //printf("gain pot: %d\n\r",gs.value_new);

    // Encoder value to difference
    if(eos.value_new > eos.value_old)
        pot_diff = +1;
    else
        pot_diff = -1;

    //printf("pot diff: %d\n\r",pot_diff);

    UiLCDBlankTiming(); // calculate/process LCD blanking timing

    // Take appropriate action
    switch(ts.enc_one_mode)
    {
        // Update audio volume
        case ENC_ONE_MODE_AUDIO_GAIN:
        {
            // Convert to Audio Gain incr/decr
            if(pot_diff < 0)
            {
                if(ts.audio_gain)
                    ts.audio_gain -= 1;
            }
            else
            {
                ts.audio_gain += 1;
                if(ts.audio_gain > ts.audio_max_volume)
                    ts.audio_gain = ts.audio_max_volume;
            }

            // Value to string
            sprintf(temp,"%02d",ts.audio_gain);

            // Update screen indicator
            UiLcdHy28_PrintText((POS_AG_IND_X + 38),(POS_AG_IND_Y + 1), temp,White,Black,0);
            //
            // If using a serial (SPI) LCD, hold off on updating the spectrum scope for a time AFTER we stop twiddling the tuning knob.
            //
            if(sd.use_spi)
                ts.hold_off_spectrum_scope  = ts.sysclock + SPECTRUM_SCOPE_SPI_HOLDOFF_TIME_TUNE;   // schedule the time after which we again update the spectrum scope
            //
            break;
        }

        // Sidetone gain or compression level
        case ENC_ONE_MODE_ST_GAIN:
        {
            if(ts.dmod_mode == DEMOD_CW)    {   // In CW mode - adjust sidetone gain
                // Convert to Audio Gain incr/decr
                if(pot_diff < 0)
                {
                    if(ts.st_gain)
                        ts.st_gain -= 1;
                }
                else
                {
                        ts.st_gain += 1;
                        if(ts.st_gain > SIDETONE_MAX_GAIN)      // limit value to proper range
                            ts.st_gain = SIDETONE_MAX_GAIN;
                }
                UiDriverChangeStGain(1);
                //
                // If using a serial (SPI) LCD, hold off on updating the spectrum scope for a time AFTER we stop twiddling the tuning knob.
                //
                if(sd.use_spi)
                    ts.hold_off_spectrum_scope  = ts.sysclock + SPECTRUM_SCOPE_SPI_HOLDOFF_TIME_TUNE;   // schedule the time after which we again update the spectrum scope
                //
                /*
                // Value to string
                sprintf(temp,"%02d",ts.st_gain);

                // Update screen indicator
                UiLcdHy28_PrintText((POS_SG_IND_X + 30),(POS_SG_IND_Y + 1), temp,White,Black,0);
                */
            }
            else    {       // In voice mode - adjust audio compression level
                // Convert to Audio Gain incr/decr
                if(pot_diff < 0)
                {
                    if(ts.tx_comp_level)    // Do not allow setting below 1 from main screen
                        ts.tx_comp_level--;
                }
                else
                {
                    ts.tx_comp_level++;
                    if(ts.tx_comp_level > TX_AUDIO_COMPRESSION_MAX)     // limit value to proper range
                        ts.tx_comp_level = TX_AUDIO_COMPRESSION_MAX;
                }
                UiCalcTxCompLevel();        // calculate values for selection compression level
                UiDriverChangeCmpLevel(1);  // update on-screen display
                //
                // If using a serial (SPI) LCD, hold off on updating the spectrum scope for a time AFTER we stop twiddling the tuning knob.
                //
                if(sd.use_spi)
                    ts.hold_off_spectrum_scope  = ts.sysclock + SPECTRUM_SCOPE_SPI_HOLDOFF_TIME_TUNE;   // schedule the time after which we again update the spectrum scope
                //
            }

            break;
        }

        default:
            break;
    }

    // Updated
    eos.value_old = eos.value_new;

}
//
//*----------------------------------------------------------------------------
//* Function Name       : UiDriverCheckEncoderTwo
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void UiDriverCheckEncoderTwo(void)
{
    //char  temp[10];
    int     pot_diff;


    ews.value_new = TIM_GetCounter(TIM4);

    // Ignore lower value flickr
    if(ews.value_new < ENCODER_FLICKR_BAND)
        return;

    // Ignore lower value flickr
    if(ews.value_new > (ENCODER_TWO_RANGE/ENCODER_TWO_LOG_D) + ENCODER_FLICKR_BAND)
        return;

    // No change, return
    if(ews.value_old == ews.value_new)
        return;

#ifdef USE_DETENTED_ENCODERS
    // SW de-detent routine
    ews.de_detent++;
    if(ews.de_detent < USE_DETENTED_VALUE)
    {
        ews.value_old = ews.value_new; // update and skip
        return;
    }
    ews.de_detent = 0;
#endif

    //printf("gain pot: %d\n\r",gs.value_new);

    // Encoder value to difference
    if(ews.value_new > ews.value_old)
        pot_diff = +1;
    else
        pot_diff = -1;

    //printf("pot diff: %d\n\r",pot_diff);

    UiLCDBlankTiming(); // calculate/process LCD blanking timing

    if(ts.menu_mode)    {
        if(pot_diff < 0)    {
            if(ts.menu_item)    {
                ts.menu_item--;
            }
            else    {
                if(!ts.radio_config_menu_enable)
                    ts.menu_item = MAX_MENU_ITEM-1; // move to the last menu item (e.g. "wrap around")
                else
                    ts.menu_item = (MAX_MENU_ITEM + MAX_RADIO_CONFIG_ITEMS)-1;  // move to the last menu item (e.g. "wrap around")
            }
        }
        else    {
            ts.menu_item++;
            if(!ts.radio_config_menu_enable)    {
                if(ts.menu_item >= MAX_MENU_ITEM)   {
                    ts.menu_item = 0;   // Note:  ts.menu_item is numbered starting at zero
                }
            }
            else    {
                if(ts.menu_item >= MAX_MENU_ITEM + MAX_RADIO_CONFIG_ITEMS)  {
                    ts.menu_item = 0;   // Note:  ts.menu_item is numbered starting at zero
                }
            }
        }
        ts.menu_var = 0;            // clear variable that is used to change a menu item
        UiDriverUpdateMenu(1);      // Update that menu item
        //
        // If using a serial (SPI) LCD, hold off on updating the spectrum scope for a time AFTER we stop twiddling the tuning knob.
        //
        if(sd.use_spi)
            ts.hold_off_spectrum_scope  = ts.sysclock + SPECTRUM_SCOPE_SPI_HOLDOFF_TIME_TUNE;   // schedule the time after which we again update the spectrum scope
        //
        goto skip_update;
    }
    if(ts.txrx_mode == TRX_MODE_RX) {
        //
        // Take appropriate action
        switch(ts.enc_two_mode)
        {
            case ENC_TWO_MODE_RF_GAIN:
            {
                if(ts.dmod_mode != DEMOD_FM)    {   // is this *NOT* FM?  Change RF gain
                    // Convert to Audio Gain incr/decr
                    if(pot_diff < 0)
                    {
                        if(ts.rf_gain)
                            ts.rf_gain -= 1;
                    }
                    else
                    {
                        ts.rf_gain += 1;
                        if(ts.rf_gain > MAX_RF_GAIN)
                            ts.rf_gain = MAX_RF_GAIN;
                    }
                    //
                    // get RF gain value and calculate new value
                    //
                    UiCalcRFGain();     // convert from user RF gain value to "working" RF gain value
                    UiDriverChangeRfGain(1);    // change on screen
                    //
                    // If using a serial (SPI) LCD, hold off on updating the spectrum scope for a time AFTER we stop twiddling the tuning knob.
                    //
                    if(sd.use_spi)
                        ts.hold_off_spectrum_scope  = ts.sysclock + SPECTRUM_SCOPE_SPI_HOLDOFF_TIME_TUNE;   // schedule the time after which we again update the spectrum scope
                    //
                    break;
                }
                else    {       // it is FM - change squelch setting
                    if(pot_diff < 0)
                    {
                        if(ts.fm_sql_threshold)
                            ts.fm_sql_threshold -= 1;
                    }
                    else
                    {
                        ts.fm_sql_threshold += 1;
                        if(ts.fm_sql_threshold > FM_SQUELCH_MAX)
                            ts.fm_sql_threshold = FM_SQUELCH_MAX;
                    }
                    //
                    // get RF gain value and calculate new value
                    //
                    UiDriverChangeRfGain(1);    // change on screen
                    //
                    // If using a serial (SPI) LCD, hold off on updating the spectrum scope for a time AFTER we stop twiddling the tuning knob.
                    //
                    if(sd.use_spi)
                        ts.hold_off_spectrum_scope  = ts.sysclock + SPECTRUM_SCOPE_SPI_HOLDOFF_TIME_TUNE;   // schedule the time after which we again update the spectrum scope
                    //
                    break;
                }
            }

            // Update DSP/NB setting
            case ENC_TWO_MODE_SIG_PROC:
            {
                if(ts.dsp_active & 8)   {   // is it in noise blanker mode?
                    // Convert to NB incr/decr
                    if(pot_diff < 0)
                    {
                        if(ts.nb_setting)
                            ts.nb_setting -= 1;
                    }
                    else
                    {
                        ts.nb_setting += 1;
                        if(ts.nb_setting > MAX_NB_SETTING)
                            ts.nb_setting = MAX_NB_SETTING;
                    }
                }
                else if(ts.dsp_active & 1)  {   // only allow adjustment if DSP NR is active
                    // Convert to NB incr/decr
                    if(pot_diff < 0)
                    {
                        if(ts.dsp_nr_strength)
                            ts.dsp_nr_strength -= 1;
                    }
                    else
                    {
                        ts.dsp_nr_strength += 1;
                        if(ts.dsp_nr_strength > DSP_NR_STRENGTH_MAX)
                            ts.dsp_nr_strength = DSP_NR_STRENGTH_MAX;
                    }
                    audio_driver_set_rx_audio_filter();
                }
                // Signal processor setting
                UiDriverChangeSigProc(1);
                //
                // If using a serial (SPI) LCD, hold off on updating the spectrum scope for a time AFTER we stop twiddling the tuning knob.
                //
                if(sd.use_spi)
                    ts.hold_off_spectrum_scope  = ts.sysclock + SPECTRUM_SCOPE_SPI_HOLDOFF_TIME_TUNE;   // schedule the time after which we again update the spectrum scope
                //
                break;
            }

            default:
                break;
        }
    }

skip_update:

    // Updated
    ews.value_old = ews.value_new;
}

//
//*----------------------------------------------------------------------------
//* Function Name       : UiDriverCheckEncoderThree
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void UiDriverCheckEncoderThree(void)
{
    int     pot_diff;

    ets.value_new = TIM_GetCounter(TIM5);

    // Ignore lower value flicker
    if(ets.value_new < ENCODER_FLICKR_BAND)
        return;

    // Ignore higher value flicker
    if(ets.value_new > (ENCODER_THR_RANGE/ENCODER_THR_LOG_D) + ENCODER_FLICKR_BAND)
        return;

    // No change, return
    if(ets.value_old == ets.value_new)
        return;

#ifdef USE_DETENTED_ENCODERS
    // SW de-detent routine
    ets.de_detent++;
    if(ets.de_detent < USE_DETENTED_VALUE)
    {
        ets.value_old = ets.value_new; // update and skip
        return;
    }
    ets.de_detent = 0;
#endif

    //printf("fir pot: %d\n\r",fs.value_new);

    // Encoder value to difference
    if(ets.value_new > ets.value_old)
        pot_diff = +1;
    else
        pot_diff = -1;

    //printf("pot diff: %d\n\r",pot_diff);

    UiLCDBlankTiming(); // calculate/process LCD blanking timing

    if(ts.menu_mode)    {
        if(pot_diff < 0)    {
            ts.menu_var--;      // increment selected item
        }
        else    {
            ts.menu_var++;      // decrement selected item
        }
        //
        UiDriverUpdateMenu(1);      // perform update of selected item
        //
        // If using a serial (SPI) LCD, hold off on updating the spectrum scope for a time AFTER we stop twiddling the tuning knob.
        //
        if(sd.use_spi)
            ts.hold_off_spectrum_scope  = ts.sysclock + SPECTRUM_SCOPE_SPI_HOLDOFF_TIME_TUNE;   // schedule the time after which we again update the spectrum scope
        //
        goto skip_update;
    }



    // Take appropriate action
    switch(ts.enc_thr_mode)
    {
        // Update RIT value
        case ENC_THREE_MODE_RIT:
        {
            if(ts.txrx_mode == TRX_MODE_RX) {
                // Convert to RIT incr/decr
                if(pot_diff < 0)
                {
                    ts.rit_value -= 1;
                    if(ts.rit_value < -50)
                        ts.rit_value = MIN_RIT_VALUE;
                }
                else
                {
                    ts.rit_value += 1;
                    if(ts.rit_value > 50)
                        ts.rit_value = MAX_RIT_VALUE;
                }

                // Update RIT
                UiDriverChangeRit(1);

                // Change frequency
                UiDriverUpdateFrequency(1,0);
                //
                // If using a serial (SPI) LCD, hold off on updating the spectrum scope for a time AFTER we stop twiddling the tuning knob.
                //
                if(sd.use_spi)
                    ts.hold_off_spectrum_scope  = ts.sysclock + SPECTRUM_SCOPE_SPI_HOLDOFF_TIME_TUNE;   // schedule the time after which we again update the spectrum scope
                //
            }
            break;
        }

        // Keyer speed
        case ENC_THREE_MODE_CW_SPEED:
        {
            if(ts.dmod_mode == DEMOD_CW)    {       // in CW mode, adjust keyer speed
                // Convert to Audio Gain incr/decr
                if(pot_diff < 0)
                {
                    ts.keyer_speed--;
                    if(ts.keyer_speed < MIN_KEYER_SPEED)
                        ts.keyer_speed = MIN_KEYER_SPEED;
                }
                else
                {
                    ts.keyer_speed++;
                    if(ts.keyer_speed > 48)
                        ts.keyer_speed = MAX_KEYER_SPEED;
                }

                UiDriverChangeKeyerSpeed(1);
                //
                // If using a serial (SPI) LCD, hold off on updating the spectrum scope for a time AFTER we stop twiddling the tuning knob.
                //
                if(sd.use_spi)
                    ts.hold_off_spectrum_scope  = ts.sysclock + SPECTRUM_SCOPE_SPI_HOLDOFF_TIME_TUNE;   // schedule the time after which we again update the spectrum scope
                //
            }
            else    {   // in voice mode, adjust audio gain
                if(ts.tx_audio_source != TX_AUDIO_MIC)  {       // in LINE-IN mode?
                    if(pot_diff < 0)    {                       // yes, adjust line gain
                        ts.tx_line_gain--;
                        if(ts.tx_line_gain < LINE_GAIN_MIN)
                            ts.tx_line_gain = LINE_GAIN_MIN;
                    }
                    else    {
                        ts.tx_line_gain++;
                        if(ts.tx_line_gain > LINE_GAIN_MAX)
                            ts.tx_line_gain = LINE_GAIN_MAX;
                    }
                    //
                    if((ts.txrx_mode == TRX_MODE_TX) && (ts.dmod_mode != DEMOD_CW))     // in transmit and in voice mode?
                        Codec_Line_Gain_Adj(ts.tx_line_gain);       // change codec gain
                }
                else    {
                    if(pot_diff < 0)    {                       // yes, adjust line gain
                        ts.tx_mic_gain--;
                        if(ts.tx_mic_gain < MIC_GAIN_MIN)
                            ts.tx_mic_gain = MIC_GAIN_MIN;
                    }
                    else    {
                        ts.tx_mic_gain++;
                        if(ts.tx_mic_gain > MIC_GAIN_MAX)
                            ts.tx_mic_gain = MIC_GAIN_MAX;
                    }
                    if(ts.tx_mic_gain > 50) {       // actively adjust microphone gain and microphone boost
                        ts.mic_boost = 1;   // software boost active
                        ts.tx_mic_gain_mult = (ts.tx_mic_gain - 35)/3;          // above 50, rescale software amplification
                        if((ts.txrx_mode == TRX_MODE_TX) && (ts.dmod_mode != DEMOD_CW)) {       // in transmit and in voice mode?
                            Codec_WriteRegister(W8731_ANLG_AU_PATH_CNTR,0x0015);    // set mic boost on
                        }
                    }
                    else    {
                        ts.mic_boost = 0;   // software mic gain boost inactive
                        ts.tx_mic_gain_mult = ts.tx_mic_gain;
                        if((ts.txrx_mode == TRX_MODE_TX) && (ts.dmod_mode != DEMOD_CW)) {   // in transmit and in voice mode?
                            Codec_WriteRegister(W8731_ANLG_AU_PATH_CNTR,0x0014);    // set mic boost off
                        }
                    }
                }
                UiDriverChangeAudioGain(1);
                //
                // If using a serial (SPI) LCD, hold off on updating the spectrum scope for a time AFTER we stop twiddling the tuning knob.
                //
                if(sd.use_spi)
                    ts.hold_off_spectrum_scope  = ts.sysclock + SPECTRUM_SCOPE_SPI_HOLDOFF_TIME_TUNE;   // schedule the time after which we again update the spectrum scope
                //
            }
            break;
        }

        default:
            break;
    }

skip_update:

    // Updated
    ets.value_old = ets.value_new;
}

//*----------------------------------------------------------------------------
//* Function Name       : UiDriverChangeEncoderOneMode
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void UiDriverChangeEncoderOneMode(uchar skip)
{
    uchar l_mode;

    if(ts.menu_mode)    // bail out if in menu mode
        return;

    if(!skip)
    {
        ts.enc_one_mode++;
        if(ts.enc_one_mode > ENC_ONE_MAX_MODE)
            ts.enc_one_mode = ENC_ONE_MODE_AUDIO_GAIN;

        l_mode = ts.enc_one_mode;
    }
    else
    {
        ts.enc_one_mode = ENC_ONE_MAX_MODE + 1;
        l_mode          = 100;
    }

    switch(l_mode)
    {
        case ENC_ONE_MODE_AUDIO_GAIN:
        {
            // Audio gain enabled
            UiDriverChangeAfGain(1);

            // Sidetone disabled
            if(ts.dmod_mode == DEMOD_CW)
                UiDriverChangeStGain(0);
            else
                UiDriverChangeCmpLevel(0);
            //

            break;
        }

        case ENC_ONE_MODE_ST_GAIN:
        {
            // Audio gain disabled
            UiDriverChangeAfGain(0);

            if(ts.dmod_mode == DEMOD_CW)
                UiDriverChangeStGain(1);
            else
                UiDriverChangeCmpLevel(1);
            //

            break;
        }

        // Disable all
        default:
        {
            // Audio gain disabled
            UiDriverChangeAfGain(0);

            // Sidetone enabled
            if(ts.dmod_mode == DEMOD_CW)
                UiDriverChangeStGain(0);
            else
                UiDriverChangeCmpLevel(0);
            //

            break;
        }
    }
}
//
//*----------------------------------------------------------------------------
//* Function Name       : UiDriverChangeEncoderTwoMode
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void UiDriverChangeEncoderTwoMode(uchar skip)
{
    uchar   l_mode;

    if(ts.menu_mode)    // bail out if in menu mode
        return;

    if(!skip)
    {
        ts.enc_two_mode++;
        if(ts.enc_two_mode >= ENC_TWO_MAX_MODE)
            ts.enc_two_mode = ENC_TWO_MODE_RF_GAIN;

        l_mode = ts.enc_two_mode;
    }
    else
    {
        ts.enc_two_mode = ENC_TWO_MAX_MODE;
        l_mode          = 100;
    }

    switch(l_mode)
    {
        case ENC_TWO_MODE_RF_GAIN:
        {
            // RF gain
            UiDriverChangeRfGain(1);

            // DSP/Noise Blanker
            UiDriverChangeSigProc(0);
            break;
        }

        case ENC_TWO_MODE_SIG_PROC:
        {
            // RF gain
            UiDriverChangeRfGain(0);

            // DSP/Noise Blanker
            UiDriverChangeSigProc(1);
            break;
        }

        // Disable all
        default:
        {
            // RF gain
            UiDriverChangeRfGain(0);

            // DSP/Noise Blanker
            UiDriverChangeSigProc(0);

            break;
        }
    }
}

//*----------------------------------------------------------------------------
//* Function Name       : UiDriverChangeEncoderThreeMode
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void UiDriverChangeEncoderThreeMode(uchar skip)
{
    uchar   l_mode;

    if(ts.menu_mode)    // bail out if in menu mode
        return;

    if(!skip)
    {
        ts.enc_thr_mode++;
        if(ts.enc_thr_mode >= ENC_THREE_MAX_MODE)
            ts.enc_thr_mode = ENC_THREE_MODE_RIT;

        l_mode = ts.enc_thr_mode;
    }
    else
    {
        ts.enc_thr_mode = ENC_THREE_MAX_MODE;
        l_mode          = 100;
    }

    switch(l_mode)
    {
        case ENC_THREE_MODE_RIT:
        {
            // RIT
            UiDriverChangeRit(1);

            if(ts.dmod_mode == DEMOD_CW)
                UiDriverChangeKeyerSpeed(0);
            else
                UiDriverChangeAudioGain(0);

            break;
        }

        case ENC_THREE_MODE_CW_SPEED:
        {
            // RIT
            UiDriverChangeRit(0);

            if(ts.dmod_mode == DEMOD_CW)
                UiDriverChangeKeyerSpeed(1);
            else
                UiDriverChangeAudioGain(1);

            break;
        }

        // Disable all
        default:
        {
            // RIT
            UiDriverChangeRit(0);

            if(ts.dmod_mode == DEMOD_CW)
                UiDriverChangeKeyerSpeed(0);
            else
                UiDriverChangeAudioGain(0);

            break;
        }
    }
}

