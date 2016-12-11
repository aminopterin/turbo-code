/* * * * * * * * * * * * * * * *
 filename: transmitter.cpp 
 author: Tzu-Yu Jeng
 date: early Sep. 2016
 description: a recursive convolutional transmitter
* * * * * * * * * * * * * * * */

#include <cstdio> // NULL
#include <cstdlib> // rand
#include <cmath> // sqrt, log, sin, cos
#include <ctime> // time
#include <vector> // vector
#include <sstream> // ostringstream
#include <cassert> // assert

#include "main.h" // constants
#include "transmitter.h"
#include "receiver.h"

// a rate 3/5, 8-state recursive systematic convolutional transmitter,
// used in core of 3GPP standard "25.212 TS".
// (but I have simplified several settings)
void rscTransmitter( const bool* pIn, bool* pOutPar )
{
   std::size_t sigma =0;
   for( std::size_t i =0; i <=lenTot-1; i++ )
   {
      pressOneCode( i, sigma, pIn[i], pOutPar[i] );
   }

   // through all trellis terminating bytes:
   bool pTermBits[lenTermBits] ={false};
   splitDigits( pTermSequence[sigma], pTermBits, lenTermBits );
   for( std::size_t m =0; m <=lenTermBits-1; m++ )
   {
      std::size_t idx =lenTot -1 +m;
      pressOneCode( idx, sigma, pTermBits[m], pOutPar[idx] );
   }
   assert(sigma==0);// to make sure the trellis terminates
}

// input bit b into the register having state s
void pressOneCode( std::size_t idx, std::size_t& state, bool b, bool& par )
{
   // initialize register holders;
   // b         ==push==> reg[0], reg[1], reg[2] ==out==> par
   // the input           the registers                   the parity
   bool reg[sizeConv] ={false};// universal zero-initializer, all to false
   bool hold =false;// future reg[0]
   // extracting digits
   splitDigits( state, reg, sizeConv );

   // feedback
   hold = ( (b != reg[1]) != reg[2] );// this is just XOR
   // parity bit
   if( idx%3 ==0 ){ par =false; }// puncturing
   else{ par = ( (hold != reg[0]) != reg[2] ); }// as usual
   // push new values into registers
   reg[2] =reg[1];
   reg[1] =reg[0];
   reg[0] =hold;

   // save registers back into "state"
   state =0;
   if( reg[0] ){ state +=4; }
   if( reg[1] ){ state +=2; }
   if( reg[2] ){ state +=1; }
}

// to convert size_t to binary digits
// pArr[0] is the most significant bit (msb) of num
// ...
// pArr[len-1] is the least significant bit (lsb) of num
void splitDigits( std::size_t number, bool* pArr, std::size_t len )
{
   std::size_t hold =number;
   std::size_t rem =0;// remainder
   std::size_t msb =1;// see below
   for( std::size_t i =1; i <=len-1; i++ ){ msb *=2; }// msb =2 ^ len

   for( std::size_t i =0; i <=len-1; i++ )
   {
      rem =hold %msb;
      pArr[i] =(hold -rem)/msb;
      hold =rem;
      msb /=2;
   }
}

void generateInfoBits( bool* pIn )
{
   // uniform r.v. between -1 to 1
   for( std::size_t i =0; i <=lenTot-1; i++ )
   {
      if( std::rand() %2 ==0 ){ pIn[i] =true; }
      else{ pIn[i] =false; }
   }
   for( std::size_t i =lenTot; i <=lenSent-1; i++ ){ pIn[i] =false; }
}

/* To understand the simplest interleaver which is used here, consider the
list of letters (for sake of illustration) in prior order as such,
   {a,b,c,d,e,f,g,h,i,j,k,l}
Arrange into a rectangle (both indices are 0-based, lenSent in total)
   i, j, k, l  \
   e, f, g, h  len_l
   a, b, c, d  /
   \..len_k../
We see that they would become
   {a,e,i,b,f,j,c,g,k,d,h,l}
after interleaving. In other words, we "flatten" the entries of the transposed
rectangle (shown above) the way the old ones are. */

// function overloading: for double
void interleave( const double* pIn, double* pIn_perm )
{
   for( std::size_t l =0; l <= len_l-1; l++ )
   {
      for( std::size_t k =0; k <= len_k-1; k++ )
      {
         std::size_t idx =l *len_k +k;// the ``linear'' index (zero-based)
         std::size_t lNew =idx %len_l;
         std::size_t kNew =(idx -lNew) /len_l;
         std::size_t idxNew =lNew *len_k +kNew;
         pIn_perm[idx] =pIn[idxNew];
      }
   }
}

// function overloading: for bool
void interleave( const bool* pIn, bool* pIn_perm )
{
   for( std::size_t l =0; l <= len_l-1; l++ )
   {
      for( std::size_t k =0; k <= len_k-1; k++ )
      {
         std::size_t idx =l *len_k +k;
         std::size_t lNew =idx %len_l;
         std::size_t kNew =(idx -lNew) /len_l;
         std::size_t idxNew =lNew *len_k +kNew;
         pIn_perm[idx] =pIn[idxNew];
      }
   }
}

// multiply by information bits each amp times, and send into AWGN
void signalThruAWGN( std::size_t len, bool* info_before, double* sig_after, double amp )
{
   for( std::size_t i =0; i <=len-1; i++ )
   {
      double u1, u2;// uniform r.v.'s
      double n;// channel noises
      u1 =std::rand() / static_cast<double>(RAND_MAX);// uniform from 0 to 1
      u2 =std::rand() / static_cast<double>(RAND_MAX);// ibid.
      // Box-Muller transform. Here, recall nn_0/2 is the variation
      // of Gaussian, cancelling a "2" inside sqrt
      n =std::sqrt(nn_0) * std::sqrt( -std::log(u1) ) * std::cos( 2*pi *u2 );
      sig_after[i] =infoToAmp( info_before[i], amp ) +n;
   }                                                            
}

// function overloading: for string
void printAligned( std::ostringstream* pOStr, const std::string& s )
{
   double w =columnWidth;
   (*pOStr) << std::left << std::setfill(' ') << std::setw(w) << s;
}

// function overloading: for double
void printAligned( std::ostringstream* pOStr, double d )
{
   double w =columnWidth;
   (*pOStr) << std::left << std::setfill(' ') << std::setw(w) 
      << std::fixed << std::setprecision(3) << d;
}

// function overloading: for double
void printAligned( std::ostringstream* pOStr, std::size_t s )
{
   double w =columnWidth;
   (*pOStr) << std::left << std::setfill(' ') << std::setw(w) 
      << std::fixed << std::setprecision(3) << s;
}

/* * * * * * * debug XXX * * * * * * * * */
// overloading for double
void printPtr( std::ostream* pOStr, double* p, std::size_t len )
{
   for( std::size_t i =0; i <=len -1; i++ )
   {
      (*pOStr) << i << ": " << p[i] << '\n';
   }
}

// overloading for bool
void printPtr( std::ostream* pOStr, bool* p, std::size_t len )
{
   for( std::size_t i =0; i <=len -1; i++ )
   {
      (*pOStr) << i << ": " << p[i] << '\n';
   }
}
