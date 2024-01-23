/*

ToneGenerator.h

Author: Matt Davison
Date: 22/01/2024

*/

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

    void setFrequency(sample_t frequency_hz)
    {
        m_frequency = frequency;
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
    m_phase = 0;
    m_frequency = 0;
    m_inverse_sample_rate = 0;

};