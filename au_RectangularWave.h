/*

au_RectangularWave.h

Author: Matt Davison
Date: 30/12/2023

Note that this is a very basic implementation of a rectangular wave generator, with no interpolation implemented.

*/

#pragma once

#include "au_config.h"

class RectangularWave
{
public:
    struct Setup
    {
        sample_t sample_rate_hz;
        sample_t frequecy_hz;
        sample_t duty_cycle;        //Between 0 and 1
    };

    void setup(Setup setup_config)
    {
        m_period_samples = setup_config.sample_rate_hz / setup_config.frequecy_hz;
        m_duty_cycle_samples = setup_config.duty_cycle * m_period_samples;
    }

    /*
     * Call once per sample for a floating point output (-1 or 1) is required (for audio).
     */
    sample_t process()
    {
        return processBool() ? 1.0 : -1.0;
    }

    /*
     * Call once per sample if a digital output is required (e.g. for a manual PWM output)
     */
    bool processBool()
    {
        if (m_current_cycle_samples > m_period_samples)
        {
            m_current_cycle_samples -= m_duty_cycle_samples;
        }

        bool output_val = m_current_cycle_samples <= m_duty_cycle_samples;
        m_current_cycle_samples += 1.0;
        return output_val;
    }

private:
    sample_t m_duty_cycle_samples;
    sample_t m_period_samples;
    sample_t m_current_cycle_samples = 1.0; // Stores position within current cycle of waveforn in samples


};