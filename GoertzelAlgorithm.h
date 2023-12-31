/*
    Goertzel Algorithm
    Created by Matt Davison on 29/11/2023

    For more info: https://en.wikipedia.org/wiki/Goertzel_algorithm
*/

#pragma once

#include "audio_utils_config.h"
#include "math.h"


class GoertzelAlgorithm
{
public:

    struct SetupParameters
    {
        int sample_rate;
        int window_size_samples;
        sample_t target_frequency;
    };
    GoertzelAlgorithm(const SetupParameters& setup_parameters)
    {
		setup(setup_parameters);
    }

    void setup(const SetupParameters& setup_parameters)
    {
        sample_t omega = (2.0 * M_PI * setup_parameters.target_frequency) / setup_parameters.sample_rate;
        m_coefficient = 2.0 * cos(omega);
        m_sine_of_omega = sin(omega);
        m_consine_of_omega = cos(omega);
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

    void process(sample_t new_sample, QValues& q_vals)
    {
        sample_t q0 = new_sample + (m_coefficient * q_vals.q1) - q_vals.q2;
        m_q2 = m_q1;
        m_q1 = q0;
        if (++m_samples_in_window_processed == m_window_length_samples)
        {
            processWindow();
        }
    }

    

    sample_t getMagnitudeQuick(QValues q_vals)
    {
        return sqrt( (q_vals.q1 * q_vals.q1) + (q_vals.q2 * q_vals.2) - (m_coefficient * q_vals.q1 * q_vals.q2));
    }

    struct ComplexPolarForm
    {
        sample_t magnitude;
        sample_t phase;
    };

    ComplexPolarForm getComplexMagnitudeAndPhase(QValues q_vals)
    {
        ComplexPolarForm mag_and_phase;
        sample_t real = q_vals.q1 - (q_vals.q2 * m_cosine_of_omega);
        sample_t imaginary = q_vals.q2 * m_sine_of_omega;
        mag_and_phase.magnitude = sqrt( (real * real) + (imaginary * imaginary);
        mag_and_phase.phase = atan(imaginary / real);
    }

    struct QValues
    {
        sample_t q1 = 0;
        sample_t q2 = 0;
    };

    sample_t m_coefficient;
    sample_t m_sine_of_omega;
    sample_t m_consine_of_omega;
    int m_window_length_samples;
};

/*
For use in realtime environments, this class updates the Q values sample by sample, rather than bulk processing at the end of each window
*/
class RealtimeGoertzel : public GoertzelAlgorithm
{
public:

    RealtimeGoertzel(const SetupParameters& setup_parameters) : GoertzelAlgorithm(SetupParameters)
    {
    }

    void processSample(sample_t new_sample)
    {
        process(new_sample, m_current_q_values);
        if (++m_samples_in_window_processed == m_window_length_samples)
        {
            processWindow();
        }
    }


    

    /*
    Virtual function called when the end of the window is reached, allowing program to get magnitude/phase without continuously polling for new value
    */
    virtual void endOfWindow(){}

    /*
    Get the magnitude of the last complete window
    */
    sample_t getLastMagnitude()
    {
        return getMagnitudeQuick(m_last_q_values);
    }

    /*
    Get the magnitude and phase of the last complete window
    */
    ComplexPolarForm getLastComplexMagnitudeAndPhase()
    {
        return getComplexMagnitudeAndPhase(m_last_q_values);
    }

    void reset()
    {
        m_current_q_values.q1 = 0;
        m_current_q_values.q2 = 0;
        m_samples_in_window_processed = 0;
    }
private:
    QValues m_current_q_values;
    QValues m_last_q_values;
    int m_samples_in_window_processed = 0;
};

/*
This class takes the whole window as a buffer and processes it in one go.
*/
class WholeWindowGoertzel : public GoertzelAlgorithm
{
public:

    RealtimeGoertzel(const SetupParameters& setup_parameters) : GoertzelAlgorithm(SetupParameters)
    {
    }
    
    double getMagnitude(double* window_buffer)
    {
        return getMagnitudeQuick(getQValsForBuffer(window_buffer));
    }
    ComplexPolarForm getMagnitudeAndPhase(double* window_buffer)
    {
        return getComplexMagnitudeAndPhase(getQValsForBuffer(window_buffer));
    }
private:
    QValues getQValsForBuffer(double* buffer)
    {
        QValues q_vals;
        for (int i = 0 ; i < m_window_length_samples ; i++)
        {
            process(buffer[i], q_vals);
        }

        return q_vals;
    }

};