/*

Biquad.cpp

Author: Matt Davison
Date: 01/03/2024

*/

#include "Biquad.h"
#include <math.h>

void Biquad::setup(FilterSetup filter_setup)
{
    m_sample_rate_hz = filter_setup.sample_rate_hz;
    m_cutoff_freq_hz = filter_setup.cutoff_freq_hz;
    m_quality_factor = filter_setup.quality_factor;
    m_filter_gain_db = filter_setup.filter_gain_db;
    m_filter_type = filter_setup.filter_type;
    clean();
    calcCoefficients();
}

void Biquad::setSampleRate(sample_t sample_rate_hz)
{
    m_sample_rate_hz = sample_rate_hz;
    calcCoefficients();
}

void Biquad::setCutoff(sample_t cutoff_freq_hz)
{
    m_cutoff_freq_hz = cutoff_freq_hz;
    calcCoefficients();
}

void Biquad::setQ(sample_t quality_factor)
{
    m_quality_factor = quality_factor;
    calcCoefficients();
}

void Biquad::setFilterGain(sample_t filter_gain_db)
{
    m_filter_gain_db = filter_gain_db;
    calcCoefficients();
}

void Biquad::setType(FilterType filter_type)
{
    m_filter_type = filter_type;
    calcCoefficients();
}

sample_t Biquad::process(sample_t input_sample)
{
    sample_t output_sample = (input_sample * m_coefficients.a0) + m_z1;
    m_z1 = (input_sample * m_coefficients.a1) + m_z2 - (output_sample * m_coefficients.b1);
    m_z2 = (input_sample * m_coefficients.a2) - (output_sample * m_coefficients.b2);
    return output_sample;
}

void Biquad::clean()
{
    m_z1 = 0;
    m_z2 = 0;
}

void Biquad::calcCoefficients()
{
    sample_t norm;
	sample_t V = pow(10, fabs(m_filter_gain_db) / 20.0);
	sample_t K = tan(M_PI * (m_cutoff_freq_hz / m_sample_rate_hz));
	switch (m_filter_type) {
	case FilterType::LOWPASS:
		norm = 1 / (1 + K / m_quality_factor + K * K);
		m_coefficients.a0 = K * K * norm;
		m_coefficients.a1 = 2 * m_coefficients.a0;
		m_coefficients.a2 = m_coefficients.a0;
		m_coefficients.b1 = 2 * (K * K - 1) * norm;
		m_coefficients.b2 = (1 - K / m_quality_factor + K * K) * norm;
		break;
	case FilterType::HIGHPASS:
		norm = 1 / (1 + K / m_quality_factor + K * K);
		m_coefficients.a0 = 1 * norm;
		m_coefficients.a1 = -2 * m_coefficients.a0;
		m_coefficients.a2 = m_coefficients.a0;
		m_coefficients.b1 = 2 * (K * K - 1) * norm;
		m_coefficients.b2 = (1 - K / m_quality_factor + K * K) * norm;
		break;
	case FilterType::BANDPASS:
		norm = 1 / (1 + K / m_quality_factor + K * K);
		m_coefficients.a0 = K / m_quality_factor * norm;
		m_coefficients.a1 = 0;
		m_coefficients.a2 = -m_coefficients.a0;
		m_coefficients.b1 = 2 * (K * K - 1) * norm;
		m_coefficients.b2 = (1 - K / m_quality_factor + K * K) * norm;
		break;
	case FilterType::NOTCH:
		norm = 1 / (1 + K / m_quality_factor + K * K);
		m_coefficients.a0 = (1 + K * K) * norm;
		m_coefficients.a1 = 2 * (K * K - 1) * norm;
		m_coefficients.a2 = m_coefficients.a0;
		m_coefficients.b1 = m_coefficients.a1;
		m_coefficients.b2 = (1 - K / m_quality_factor + K * K) * norm;
		break;
	case FilterType::PEAK:
		if (m_filter_gain_db >= 0) { // boost
			norm = 1 / (1 + 1/m_quality_factor * K + K * K);
			m_coefficients.a0 = (1 + V/m_quality_factor * K + K * K) * norm;
			m_coefficients.a1 = 2 * (K * K - 1) * norm;
			m_coefficients.a2 = (1 - V/m_quality_factor * K + K * K) * norm;
			m_coefficients.b1 = m_coefficients.a1;
			m_coefficients.b2 = (1 - 1/m_quality_factor * K + K * K) * norm;
		}
		else { // cut
			norm = 1 / (1 + V/m_quality_factor * K + K * K);
			m_coefficients.a0 = (1 + 1/m_quality_factor * K + K * K) * norm;
			m_coefficients.a1 = 2 * (K * K - 1) * norm;
			m_coefficients.a2 = (1 - 1/m_quality_factor * K + K * K) * norm;
			m_coefficients.b1 = m_coefficients.a1;
			m_coefficients.b2 = (1 - V/m_quality_factor * K + K * K) * norm;
		}
		break;
	case FilterType::LOWSHELF:
		if (m_filter_gain_db >= 0) { // boost
			norm = 1 / (1 + sqrt(2) * K + K * K);
			m_coefficients.a0 = (1 + sqrt(2*V) * K + V * K * K) * norm;
			m_coefficients.a1 = 2 * (V * K * K - 1) * norm;
			m_coefficients.a2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
			m_coefficients.b1 = 2 * (K * K - 1) * norm;
			m_coefficients.b2 = (1 - sqrt(2) * K + K * K) * norm;
		}
		else { // cut
			norm = 1 / (1 + sqrt(2*V) * K + V * K * K);
			m_coefficients.a0 = (1 + sqrt(2) * K + K * K) * norm;
			m_coefficients.a1 = 2 * (K * K - 1) * norm;
			m_coefficients.a2 = (1 - sqrt(2) * K + K * K) * norm;
			m_coefficients.b1 = 2 * (V * K * K - 1) * norm;
			m_coefficients.b2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
		}
		break;
	case FilterType::HIGHSHELF:
		if (m_filter_gain_db >= 0) { // boost
			norm = 1 / (1 + sqrt(2) * K + K * K);
			m_coefficients.a0 = (V + sqrt(2*V) * K + K * K) * norm;
			m_coefficients.a1 = 2 * (K * K - V) * norm;
			m_coefficients.a2 = (V - sqrt(2*V) * K + K * K) * norm;
			m_coefficients.b1 = 2 * (K * K - 1) * norm;
			m_coefficients.b2 = (1 - sqrt(2) * K + K * K) * norm;
		}
		else { // cut
			norm = 1 / (V + sqrt(2*V) * K + K * K);
			m_coefficients.a0 = (1 + sqrt(2) * K + K * K) * norm;
			m_coefficients.a1 = 2 * (K * K - 1) * norm;
			m_coefficients.a2 = (1 - sqrt(2) * K + K * K) * norm;
			m_coefficients.b1 = 2 * (K * K - V) * norm;
			m_coefficients.b2 = (V - sqrt(2*V) * K + K * K) * norm;
		}
		break;
	}
}