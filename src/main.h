/* * * * * * * * * * * * * * * *
 filename: main.h
 author: Tzu-Yu Jeng
 date: early Sep. 2016
 description: to define global constants
* * * * * * * * * * * * * * * */

#ifndef MAIN_H 
#define MAIN_H

#include <cstddef> // size_t
#include <vector> // vector
#include <cmath> // pow, log, exp

// numerical const.:
const double negInf =-6.02e+23;// any large magnitude. (Chemistry rocks)
const double pi =3.14159265358979323846;// \pi
const double nn_0 =1.0;// w.l.o.g. in terms of ee_b /nn_0

// simulation-method related:
const std::size_t len_k =60;//first dimension of interleaving square
const std::size_t len_l =90;//second dimension of interleaving square
const std::size_t lenSent =len_k *len_l;// no. of bits sent
const std::size_t lenTermBits =3;// no. of bits required to zero the trellis
const std::size_t lenTot =lenSent -lenTermBits;// block length

const std::size_t pTermSequence[] ={ 0, 4, 6, 2, 3, 7, 5, 1 };// trellis termination sequences
const std::size_t sizeConv =3;// no. of registers of convolution encoder
const std::size_t numState =std::pow( 2.0, static_cast<double>(sizeConv) );// which is 2 ^ sizeConv
const std::size_t numBlock =25;// no. of blocks
const double rateCode =static_cast<double>(3) /5;// code rate
const std::size_t idxShift =7;// XXX

// data point specification:
const double snrShannon =(std::exp( 2 *rateCode ) -1) /2;
const double snrShannon_dB =10 *std::log(snrShannon);
const double multSnr =1.1;// incremental multiple of snr axis
const double startSnr =5.0;
//const double startSnr =snrShannon /multSnr;// starting value of snr
const std::size_t numPtSnr =8;// no. of snr values considered
const std::size_t init[] ={ 1, 2, 4, 6, 9, 12, 18, 24 };// initialization helper
const std::vector<std::size_t> listNumIter( init, init +sizeof(init) /sizeof(init[0]) );// each curve with fixed iteration no.
const std::size_t numCurve =listNumIter.size();// no. of curves 
const double columnWidth =8.0;// width of column

#endif