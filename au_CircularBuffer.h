/*

au_CircularBuffer.h

Author: Matt Davison
Date: 30/12/2023

*/

template<typename T, unsigned long buffer_length>
class CircularBuffer
{
public:

    void write(T new_buffer_val)
    {
        m_buffer[m_write_index++] = new_buffer_val;

        //Detect overrun - write point is more than buffer length ahead of the read point
        if (++m_read_write_distance > buffer_length)
        {
            m_overrun = true;
        }

        m_write_index = m_write_index % buffer_length;
    }

    T read()
    {
        T read_val = m_buffer[m_read_index++];

        //Detect underrun - read point is ahead of write point
        if (--m_read_write_distance < 0)
        {
            m_underrun = true;
        }

        m_read_index = m_read_index % buffer_length;
        return read_val;
    }

    bool overRun()
    {
        bool has_overrun = m_overrun;
        m_overrun = false;
        return has_overrun;
    }

    bool underRun()
    {
        bool has_underrun = m_underrun;
        m_underrun = false;
        return has_underrun;
    }

    long itemsInBuffer()
    {
        return m_read_write_distance;
    }

private:
    T m_buffer[buffer_length]; 
    unsigned long m_write_index = 0;
    unsigned long m_read_index = 0;

    long m_read_write_distance = 0;

    //Flags to detect over/under runs of buffer - set when it occurs during read/write, cleared after calling respective flag getter functions (overRun() and underRun() )
    bool m_overrun = false;
    bool m_underrun = false;
};