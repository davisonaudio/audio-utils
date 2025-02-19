/*

au_Onepole.h

Author: Matt Davison
Date: 04/03/2024

*/

#pragma once

#include "au_config.h"

class Onepole
{

public:

    Onepole(sample_t b1_value = 0.0, bool normalise_gain = true)
    {
        setB1(b1_value, normalise_gain);
    }

    void setB1(sample_t b1_value, bool normalise_gain = true)
    {
        m_b1 = b1_value;
        if (normalise_gain)
        {
            m_a0 = 1 - abs(m_b1);
        }
    }
    sample_t process(sample_t input_sample)
    {
        m_z1 = (input_sample * m_a0) + (m_z1 * m_b1 * -1.0);
        return m_z1;
    }

private:
    sample_t m_a0, m_b1, m_z1 = 0.0;
};