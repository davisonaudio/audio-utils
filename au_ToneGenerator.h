/*

au_ToneGenerator.h

Author: Matt Davison
Date: 22/01/2024

*/

#pragma once

#include "au_config.h"
#include <cmath>

class ToneGenerator
{
public:

    ToneGenerator(sample_t tone_frequency_hz, sample_t sample_rate_hz, sample_t level_db = 0)
    {
        setFrequency(tone_frequency_hz);
        setSampleRate(sample_rate_hz);
        setLeveldB(level_db);
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

    void setSampleRate(sample_t sample_rate_hz)
    {
        m_inverse_sample_rate = 1 / sample_rate_hz;
    }

    void setLeveldB(sample_t level_db)
    {
        m_level_lin = dBToLin(level_db);
    }

    sample_t process()
    {
        sample_t out = sinf(m_phase);
		m_phase += 2.0f * (sample_t)M_PI * m_frequency * m_inverse_sample_rate;
		if(m_phase > M_PI)
			m_phase -= 2.0f * (sample_t)M_PI;
        return out * m_level_lin;
    }

private:
    sample_t m_phase = 0;
    sample_t m_frequency = 0;
    sample_t m_inverse_sample_rate = 1.0;
    sample_t m_level_lin = 1.0;

};