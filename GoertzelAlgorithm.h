/*
    Goertzel Algorithm
    Created by Matt Davison on 29/11/2023

    For more info: https://en.wikipedia.org/wiki/Goertzel_algorithm
*/

#pragma once

#include "math.h"

template <typename sample_t, int MAX_WINDOW_SIZE_SAMPLES>
class GoertzelAlgorithm
{
public:

    struct SetupParameters
    {
        int sample_rate;
        int window_size_samples;
        sample_t target_frequency;
    }
    GoertzelAlgorithm(const SetupParameters& setup_parameters) : setup(setup_parameters)
    {

    }

    void setup(const SetupParameters& setup_parameters)
    {
        sample_t omega = (2.0 * M_PI * setup_parameters.target_frequency) / setup_parameters.sample_rate;
        m_coefficient = 2.0 * cos(omega);
        if (setup_parameters.window_size_samples <= MAX_WINDOW_SIZE_SAMPLES)
        {
            m_window_length_samples = setup_parameters.window_size_samples;
        }
        else
        {
            m_window_length_samples = 0; //error - window size doesn't fix into array!
        }
        
        resetValues();
    }

    void process(sample_t new_sample)
    {
        sample_t q0 = new_sample + (m_coefficient * m_q1) - m_q2;
        m_q2 = m_q1;
        m_q1 = q0;
        if (++m_samples_in_window_processed == WINDOW_SIZE_SAMPLES)
        {
            processWindow();
        }
    }

    sample_t getMagnitude()
    {
        return m_magnitude;
    }

private:

    void processWindow()
    {
        m_magnitude = sqrt( (m_q1 * m_q1) + (m_q2 * m_q2) - (m_coefficient * m_q1 * m_q2));
        resetValues();
    }

    void resetValues()
    {
        m_q1 = 0;
        m_q2 = 0;
        m_samples_in_window_processed = 0;
    }

    sample_t m_q1 = 0;
    sample_t m_q2 = 0;
    sample_t m_coefficient;

    int m_samples_in_window_processed = 0;

    sample_t m_window_buffer[MAX_WINDOW_SIZE_SAMPLES];
    sample_t m_magnitude = 0;

    int m_window_length_samples;
};