
/** \file exception.h 
  \brief Exception handling. */
#pragma once

#include <stdexcept>

extern char ErrorMessageBuffer[1024];

/** \brief Fancy way to construct std::runtime_error with a given message */
std::runtime_error Exception(const char *Format, ...);

