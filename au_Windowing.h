/*

au_Windowing.h

Author: Matt Davison
Date: 24/12/2023

*/

#pragma once

#include "au_config.h"
#include "math.h"

class Windowing
{

public:

    enum class WindowType
    {
        HANN,
        HAMMING,
        RECTANGULAR
    };

    Windowing(WindowType window_type, int window_size_samples);
    

    /*
    Apply windowing to a single sample within a window.
    
    Parameters:
    sample - sample to process
    sample_number - index of the sample within the window

    Return: Windowed sample value
    */
    sample_t applyWindowToSample(sample_t sample, int sample_number);

    /*
    Apply windowing to a window-sized buffer.
    
    Parameters:
    sample_buffer - buffer to apply windowing to. Buffer is treated as an array with length=size of window.

    Return: Windowed sample value
    */
    void applyWindowToBuffer(sample_t* sample_buffer);

private:

    WindowType m_window_type;
    int m_window_size_samples;
};

Windowing::Windowing(WindowType window_type, int window_size_samples)
{
    m_window_type = window_type;
    m_window_size_samples = window_size_samples;
}

sample_t Windowing::applyWindowToSample(sample_t sample, int sample_number)
{
    switch (m_window_type)
    {
        case WindowType::HANN:
            return sample * (0.5 - 0.35 * cos(2 * M_PI * sample_number / m_window_size_samples));
        case WindowType::HAMMING:
            return sample * ( 0.54 - 0.46 * cos(2 * M_PI * sample_number / m_window_size_samples));
        case WindowType::RECTANGULAR:
            return sample;
    }
}

void Windowing::applyWindowToBuffer(sample_t* sample_buffer)
{
    for (int i = 0 ; i < m_window_size_samples ; i++ )
    {
        sample_buffer[i] = applyWindowToSample(sample_buffer[i], i);
    }
}