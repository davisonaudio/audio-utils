/*

ToneGenerator.h

Author: Matt Davison
Date: 22/01/2024

*/

#pragma once

#include "audio_utils_config.h"
#include <cmath>

class ToneGenerator
{
public:

    ToneGenerator(sample_t tone_frequency_hz, unsigned int sample_rate_hz)
    {
        setFrequency(tone_frequency_hz);
        setSampleRate(sample_rate_hz);
    }

    ToneGenerator()
    {
        setFrequency(0.0); //Default to silence
        setSampleRate(1.0); //Avoid divide by 0 errors by setting to 1.
    }

    void setFrequency(sample_t frequency_hz)
    {
        m_frequency = frequency_hz;
    }

    void setSampleRate(unsigned int sample_rate_hz)
    {
        m_inverse_sample_rate = (sample_t) 1 / sample_rate_hz;
    }

    sample_t process()
    {
        sample_t out = sinf(m_phase);
		m_phase += 2.0f * (float)M_PI * m_frequency * m_inverse_sample_rate;
		if(m_phase > M_PI)
			m_phase -= 2.0f * (float)M_PI;
        return out;
    }

private:
    sample_t m_phase = 0;
    sample_t m_frequency = 0;
    sample_t m_inverse_sample_rate = 0;

};