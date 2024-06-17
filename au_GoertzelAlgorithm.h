/*
    Goertzel Algorithm
    Created by Matt Davison on 29/11/2023

    For more info: https://en.wikipedia.org/wiki/Goertzel_algorithm
*/

#pragma once

#include "au_config.h"
#include "math.h"


class GoertzelAlgorithm
{
public:

    struct SetupParameters
    {
        int sample_rate;
        int window_size_periods;
        sample_t target_frequency;
    };


    struct QValues
    {
        sample_t q1 = 0;
        sample_t q2 = 0;
        void reset()
        {
            q1 = 0;
            q2 = 0;
        }
    };

    void setup(const SetupParameters& setup_parameters);
    void setTargetFrequencyHz(sample_t target_frequency_hz);
    void process(sample_t new_sample, QValues& q_vals);

    

    sample_t getMagnitudeQuick(QValues q_vals);

    struct ComplexPolarForm
    {
        sample_t magnitude;
        sample_t phase;
    };

    ComplexPolarForm getComplexMagnitudeAndPhase(QValues q_vals);
    
 protected:   
    int m_window_length_samples;
    int m_window_size_periods;

private:
    sample_t m_coefficient;
    sample_t m_sine_of_omega;
    sample_t m_cosine_of_omega;

    sample_t m_target_frequency;
    int m_sample_rate;


    void recalcCoefficients();
};

/*
For use in realtime environments, this class updates the Q values sample by sample, rather than bulk processing at the end of each window
*/
class RealtimeGoertzel : public GoertzelAlgorithm
{
public:


    void processSample(sample_t new_sample)
    {
        process(new_sample, m_current_q_values);
        if (++m_samples_in_window_processed == m_window_length_samples)
        {
            m_last_q_values = m_current_q_values;
            reset();
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
        m_current_q_values.reset();
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



/*
 * GoertzelAlgorithm implementation
 */

void GoertzelAlgorithm::setup(const SetupParameters& setup_parameters)
{
    m_sample_rate = setup_parameters.sample_rate;
    m_window_size_periods = setup_parameters.window_size_periods;
    m_target_frequency = setup_parameters.target_frequency;
    recalcCoefficients();
}

void GoertzelAlgorithm::setTargetFrequencyHz(sample_t target_frequency_hz)
{
    m_target_frequency = target_frequency_hz;
    recalcCoefficients();
}

void GoertzelAlgorithm::process(sample_t new_sample, QValues& q_vals)
{
    sample_t q0 = new_sample + (m_coefficient * q_vals.q1) - q_vals.q2;
    q_vals.q2 = q_vals.q1;
    q_vals.q1 = q0;
}

sample_t GoertzelAlgorithm::getMagnitudeQuick(QValues q_vals)
{
    return sqrt( (q_vals.q1 * q_vals.q1) + (q_vals.q2 * q_vals.q2) - (m_coefficient * q_vals.q1 * q_vals.q2));
}

GoertzelAlgorithm::ComplexPolarForm GoertzelAlgorithm::getComplexMagnitudeAndPhase(QValues q_vals)
{
    ComplexPolarForm mag_and_phase;
    sample_t real = q_vals.q1 - (q_vals.q2 * m_cosine_of_omega);
    sample_t imaginary = q_vals.q2 * m_sine_of_omega;
    mag_and_phase.magnitude = sqrt( (real * real) + (imaginary * imaginary);
    mag_and_phase.phase = atan(imaginary / real);
}

void GoertzelAlgorithm::recalcCoefficients()
{
    sample_t omega = (2.0 * M_PI * m_target_frequency) / m_sample_rate;
    m_coefficient = 2.0 * cos(omega);
    m_sine_of_omega = sin(omega);
    m_cosine_of_omega = cos(omega);

    m_window_length_samples = (int) m_window_size_periods * (m_sample_rate / m_target_frequency);
}