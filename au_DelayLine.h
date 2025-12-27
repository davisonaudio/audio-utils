/*

au_DelayLine.h

Author: Matt Davison
Date: 31/12/2023

*/

template<typename T, unsigned long max_buffer_length>
class DelayLine
{
public:

    void setDelayLength(unsigned long delay_length_samples)
    {
        m_delay_length_samples = delay_length_samples;
    }

    T step(T write_value)
    {
        T return_val = read();
        write(write_value);
        step();
        return return_val;
    }

    T read()
    {
        return m_buffer[m_index];
    }

    write(T write_value)
    {
        m_buffer[m_index] = write_value;
    }

    void step()
    {
        m_index = (m_index + 1) % m_delay_length_samples;
    }

private:
    T m_buffer[max_buffer_length]; 
    unsigned long m_index = 0;
    unsigned long m_delay_length_samples = max_buffer_length;

};