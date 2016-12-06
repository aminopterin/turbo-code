/* * * * * * * * * * * * * * * *
 filename: receiver.h 
 author: Tzu-Yu Jeng
 date: early Sep. 2016
 description: header for receiver.cpp
* * * * * * * * * * * * * * * */
#ifndef RECEIVER_H
#define RECEIVER_H

#include <cstddef> // size_t
#include <vector> // vector
#include <queue> // queue
#include <iomanip> // setw, setfill
#include "main.h" // constants

// chief functions:
void sisoReceiver( double, const double*, 
      const double*, const double*, double*, double* );
void tabulateAlphaBetaGamma( float**, float**, float***, 
      const double*, const double*, const double*, std::size_t );
bool getFromTrellisState( std::size_t, std::size_t, std::size_t, bool&, bool& );

// helper functions:
double pickMaxAmongVector( const std::vector<double>& );
double infoToAmp( bool, double );
void deinterleave( const double*, double* );
void deinterleave( const bool*, bool* );
void decide( const double*, bool* );

#endif
