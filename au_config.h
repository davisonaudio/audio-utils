/*

au_config.h

Author: Matt Davison
Date: 24/12/2023

*/

#pragma once

#include <math.h>

#define sample_t float

sample_t dBToLin(sample_t db_value)
{
    return pow(10, db_value / 20.0);
}

sample_t linTodB(sample_t db_value)
{
    return 0;
}

template <typename T>
constexpr sample_t intToNormalised(T integer_value) {
  return integer_value < 0
    ? -static_cast<sample_t>(integer_value) / std::numeric_limits<T>::min()
    :  static_cast<sample_t>(integer_value) / std::numeric_limits<T>::max();
}

template <typename T>
constexpr T normalisedToInt(sample_t normalised_value) {
  return normalised_value < 0
    ? -static_cast<sample_t>(normalised_value) * std::numeric_limits<T>::min()
    :  static_cast<sample_t>(normalised_value) * std::numeric_limits<T>::max();
}