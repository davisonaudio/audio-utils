/*

au_DelayLine.h

Author: Matt Davison
Date: 31/12/2023

*/

template<typename T, unsigned long buffer_length>
class DelayLine
{
public:

    T step(T write_value)
    {
        T read_value = m_buffer[m_index];
        m_buffer[m_index] = write_value;
        m_index = (m_index + 1) % buffer_length;
    }

private:
    T m_buffer[buffer_length]; 
    unsigned long m_index = 0;

};