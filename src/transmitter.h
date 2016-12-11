/* * * * * * * * * * * * * * * *
 filename: transmitter.h 
 author: Tzu-Yu Jeng
 date: early Sep. 2016
 description: header for transmitter.cpp
* * * * * * * * * * * * * * * */
#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <cstddef> // size_t
#include "main.h" // constants

// transmitter coding:
void rscTransmitter( const bool*, bool* );
void pressOneCode( std::size_t, std::size_t&, bool, bool& );
void splitDigits( std::size_t, bool*, std::size_t );
void generateInfoBits( bool* );
void interleave( const double*, double* );
void interleave( const bool*, bool* );

// channel noise:
void signalThruAWGN( std::size_t, bool*, double*, double );

// to print:
void printAligned( std::ostringstream*, const std::string& );
void printAligned( std::ostringstream*, double );
void printAligned( std::ostringstream*, std::size_t );

// debug XXX
/* * * * * * * debug XXX * * * * * * * * */
void printPtr( std::ostream*, double*, std::size_t );
void printPtr( std::ostream*, bool*, std::size_t );

#endif
