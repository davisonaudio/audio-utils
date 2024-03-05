/*

audio_utils_config.h

Author: Matt Davison
Date: 24/12/2023

*/

#pragma once

#include <math.h>

#define sample_t double

sample_t dBToLin(sample_t db_value)
{
    return pow(10, db_value / 20.0);
}

sample_t linTodB(sample_t db_value)
{
    return 0;
}