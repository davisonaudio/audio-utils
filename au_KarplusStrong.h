/*

au_KarplusStrong.h

Author: Matt Davison
Date: 25/04/2025

*/

#pragma once

#define SAMPLE_RATE 48000

#include "stdint.h"
#include "au_config.h"

namespace AudioUtils
{

    class KarplusStrong {
    public:

        static constexpr float LOWEST_FUNDAMENTAL_HZ = 20.0;
        static constexpr int MAX_DELAY = static_cast<int>(SAMPLE_RATE / LOWEST_FUNDAMENTAL_HZ);
        static constexpr int BUFFER_SIZE = MAX_DELAY + 2;

        KarplusStrong() 
        {
            damping = 0.98f;
            blend = 0.9f;
            lowpassCoeff = 0.5f;
            pluckPos = 0.2f;
            writeIndex = 0;
            delayLength = 200.0f;
            targetDelay = 200.0f;
            for (int i = 0; i < BUFFER_SIZE; ++i) buffer[i] = 0.0f;
        }
    
        void setFrequency(sample_t freq) 
        {
            if (freq < 20.0f) freq = 20.0f;
            freq = auClamp(freq, LOWEST_FUNDAMENTAL_HZ, SAMPLE_RATE / 2 ); //Clamp between delay line length and Nyquist
            targetDelay = SAMPLE_RATE / freq;
        }
    
        void setDamping(sample_t d) { damping = auClamp(d, 0.0f, 1.0f); }
        void setBlend(sample_t b) { blend = auClamp(b, 0.0f, 1.0f); }
        void setLowpass(sample_t a) { lowpassCoeff = auClamp(a, 0.0f, 1.0f); }
        void setPluckPosition(sample_t pos) { pluckPos = auClamp(pos, 0.01f, 0.99f); }
    
        void pluck() 
        {
            int len = static_cast<int>(targetDelay);
            delayLength = static_cast<float>(len);
            for (int i = 0; i < len; ++i) buffer[i] = fastRand();
    
            int M = static_cast<int>(pluckPos * len);
            for (int i = M; i < len; ++i) {
                buffer[i] -= buffer[i - M];
            }
            writeIndex = len % BUFFER_SIZE;
            lowpassState = 0.0f;
        }
    
        sample_t process(sample_t input = 0.0f) 
        {
            // Change delay length gradually to avoid artefacts during plucks
            sample_t slew = 0.01f;
            delayLength += slew * (targetDelay - delayLength);
    
            sample_t readIndex = writeIndex - delayLength;
            if (readIndex < 0) readIndex += BUFFER_SIZE;
            
            // Linear interpolation for non-integer read indexes
            int i1 = static_cast<int>(readIndex);
            int i2 = (i1 + 1) % BUFFER_SIZE;
            sample_t frac = readIndex - static_cast<float>(i1);
            sample_t sample = (1.0f - frac) * buffer[i1] + frac * buffer[i2];
    
            // Random for blend (between string and drum)
            sample_t sign = (fastRand() > 0.0f) ? 1.0f : -1.0f;
            sample_t blended = (1.0f - blend) * sample + blend * sign * sample;
    
            // Apply lowpass 
            lowpassState = (1.0f - lowpassCoeff) * blended + lowpassCoeff * lowpassState;
            sample_t y = input + damping * lowpassState;
    
            //Write new sample into delay line buffer
            buffer[writeIndex] = y;
            writeIndex = (writeIndex + 1) % BUFFER_SIZE;
    
            return auClamp(y, -1.0f, 1.0f);
        }
    
    private:
        sample_t buffer[BUFFER_SIZE];
        sample_t lowpassState;
        int writeIndex;
        sample_t delayLength;
        sample_t targetDelay;
    
        sample_t damping;
        sample_t blend;
        sample_t lowpassCoeff;
        sample_t pluckPos;
    
        uint32_t seed = 123456;
        sample_t fastRand() // PRNG with range from -1.0 to +0.9999389648
        {
            seed = seed * 1664525 + 1013904223;
            return ((seed >> 16) & 0x7FFF) / 16384.0f - 1.0f;
        }
    };

} //Namespace AudioUtils
