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

    Windowing(WindowType window_type = WindowType::HAMMING, int window_size_samples = 1);

    void setWindowSizeSamples(int window_size_samples);

    sample_t applyWindowToSample(sample_t sample);
    

    /*
    Apply windowing to a single sample within a window.
    
    Parameters:
    sample - sample to process
    sample_number - index of the sample within the window

    Return: Windowed sample value
    */
    sample_t applyWindowToNumberedSample(sample_t sample, int sample_number);


    /*
    Apply windowing to a window-sized buffer.
    
    Parameters:
    sample_buffer - buffer to apply windowing to. Buffer is treated as an array with length=size of window.

    Return: Windowed sample value
    */
    void applyWindowToBuffer(sample_t* sample_buffer);

    void resetIndex();

private:

    WindowType m_window_type;
    int m_window_size_samples;
    int m_sample_number = 0;
};

Windowing::Windowing(WindowType window_type, int window_size_samples)
{
    m_window_type = window_type;
    m_window_size_samples = window_size_samples;
}

void Windowing::setWindowSizeSamples(int window_size_samples)
{
    m_window_size_samples = window_size_samples;
    resetIndex();
}

sample_t Windowing::applyWindowToSample(sample_t sample)
{
    sample_t windowed_sample = applyWindowToNumberedSample(sample, m_sample_number);
    if (++m_sample_number == m_window_size_samples)
    {
        m_sample_number = 0;
    }
    return windowed_sample;
}

sample_t Windowing::applyWindowToNumberedSample(sample_t sample, int sample_number)
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

void Windowing::resetIndex()
{
    m_sample_number = 0;
}