/*

Biquad.h

Author: Matt Davison
Date: 01/03/2024


Modified from code originally created by Nigel Redmon on 11/24/12
EarLevel Engineering: earlevel.com
Copyright 2012 Nigel Redmon

For a complete explanation of the code:
http://www.earlevel.com/main/2012/11/25/biquad-c-source-code/

*/

#pragma once

#include "audio_utils_config.h"

class Biquad
{
public:
    enum class FilterType
    {
        LOWPASS,
        HIGHPASS,
        BANDPASS,
        NOTCH,
        PEAK,
        LOWSHELF,
        HIGHSHELF
    };

    struct FilterSetup
    {
        sample_t sample_rate_hz;
        sample_t cutoff_freq_hz;
        sample_t quality_factor;
        sample_t filter_gain_db;
        FilterType filter_type;
    };

    struct Coefficients
    {
        sample_t a0, a1, a2, b1, b2;
    };
    

    void setup(FilterSetup filter_setup);

    void setSampleRate(sample_t sample_rate_hz);
    void setCutoff(sample_t cutoff_freq_hz);
    void setQ(sample_t quality_factor);
    void setFilterGain(sample_t filter_gain_db);
    void setType(FilterType filter_type);

    sample_t process(sample_t input_sample);
    void clean();


private:

    void calcCoefficients();

    Coefficients m_coefficients;
    sample_t m_z1 = 0.0;
    sample_t m_z2 = 0.0;
    sample_t m_sample_rate_hz, m_cutoff_freq_hz, m_quality_factor, m_filter_gain_db;
    FilterType m_filter_type;

};