/* * * * * * * * * * * * * * * *
 filename: receiver.hpp
 author: Tzu-Yu Jeng
 date: early Sep. 2016
 description: header for receiver.cpp
* * * * * * * * * * * * * * * */
#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include <cstddef> // size_t
#include <vector> // vector
#include <queue> // queue
#include <iomanip> // setw, setfill
#include "main.hpp" // constants

// chief functions:
void sisoReceiver( double, const double*, 
      const double*, const double*, double*, double* );
void tabulateAlphaBetaGamma( double**, double**, double***, 
      const double*, const double*, const double*, std::size_t );
bool getFromTrellisState( std::size_t, std::size_t, std::size_t, bool&, bool& );

// helper functions:
double logSumExp( const std::vector<double>& );
double infoToAmp( bool, double );
void deinterleave( const double*, double* );
void deinterleave( const bool*, bool* );
void decide( const double*, bool* );
void zero( double*, std::size_t );
void zero( bool*, std::size_t );

#endif
