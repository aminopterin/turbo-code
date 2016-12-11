/* * * * * * * * * * * * * * * *
 filename: receiver.cpp
 author: Tzu-Yu Jeng
 date: early Sep. 2016
 description: header for receiver.cpp
* * * * * * * * * * * * * * * */

#include <cmath> // log, sin, cos, sqrt
#include <iostream> // left
#include <iomanip> // setw, setfill
#include <string> // string
#include <cassert> // assert

#include "main.h" // constants
#include "transmitter.h"
#include "receiver.h"

//    (For more info, see Proakis, Digital Communications, 6e)
//    This soft-in soft-out (SISO) receiver is the component of turbo receiver, 
// which consists of two siso receivers fed back to each other.
// It receives a series of system bits, another that's of parity bits, 
// and another external term in the expression of log-likelihood ratio (LLR)
// that neither depends on a priori probability nor systematic bits, 
// but on parity bits --- this part relies on the feedback of another SISO.
//    The arguments are, resp.:
// bit amplitude as const., length of info. bits together with padding 0's,
// external LLR sequence as input, systematic bits as input, parity bits as input,
// decided sequence of bits, external LLR sequence as output, and LLR for overall info. bits.
void sisoReceiver( double amp, const double* pL_aPr_in,
      const double* pY_sys, const double* pY_par, 
      double* pL_ext_out, double* pL_tot )
{
   // to allocate \alpha, \beta, \gamma's.
   // For clarity, I've translated programming variables to conventional ones.
   // Suppose sPrev is the i-th state, and sNow the (i+1)-th:
   // ppAlpha[i][sNow] :=\alpha_i ( \sigma_i ), i=0,...,len
   double** ppAlpha =new double*[lenSent+1];
   // ppBeta[i][sPrev] :=\beta_i ( \sigma_{i-1} ), i=0,...,len
   double** ppBeta =new double*[lenSent+1];
   // pppGamma[i][sPrev][sNow] :=\gamma_{i+1} ( \sigma_i,\sigma_{i+1} ), i=0,...,len-1
   double*** pppGamma  =new double**[lenSent];
   for( int i =0; i <=lenSent; i++ )
   {
      ppAlpha[i] = new double[numState];
      ppBeta[i] = new double[numState];
   }
   for( int i =0; i <=lenSent-1; i++ )
   {
      pppGamma[i] = new double*[numState];
      for( int s =0; s <=numState-1; s++ )
      {
         pppGamma[i][s] =new double[numState];
      }
   }

   // Recall that \alpha's, \beta's, \gamma's are log values!
   tabulateAlphaBetaGamma( ppAlpha, ppBeta, pppGamma, pY_sys, pY_par, pL_aPr_in, amp );

   for( std::size_t i =0; i <= lenSent-1; i++ )
   {
      std::vector< double > list0;
      std::vector< double > list1;
      for( std::size_t sigmaPrev =0; sigmaPrev <=numState-1; sigmaPrev++ )
      {
         for( std::size_t sigmaNow =0; sigmaNow <=numState-1; sigmaNow++ )
         {
            bool holdSys;
            bool holdPar;
            if( !getFromTrellisState( i, sigmaPrev, sigmaNow, holdSys, holdPar ) ){ continue; }

            // otherwise, it's possible to go from sigmaPrev to sigmaNow
            // see Proakis, Digital Communications, 6e, ch.8, p.552, (8.9-8)
            if( holdSys )
            {
               list1. push_back( 2 * infoToAmp(holdSys, amp) * infoToAmp(holdPar, amp) /nn_0 +
                     ppAlpha [i] [sigmaPrev] + ppBeta [i+1] [sigmaNow] );
            }
            else
            {
               list0. push_back( 2 * infoToAmp(holdSys, amp) * infoToAmp(holdPar, amp) /nn_0 +
                     ppAlpha [i] [sigmaPrev] + ppBeta [i+1] [sigmaNow] );
            }
         }
      }

      // saving overall log-likelihoods
      pL_ext_out[i] =logSumExp(list1) -logSumExp(list0);
      pL_tot[i] =4 *amp *pY_sys[i] /nn_0 +pL_aPr_in[i] +pL_ext_out[i];
   }

   // release of memory
   for( int i =0; i <=lenSent; i++ )
   {
      delete[] ppAlpha[i];
      delete[] ppBeta[i];
   }
   delete[] ppAlpha;
   delete[] ppBeta;

   for( int i =0; i <=lenSent-1; i++ )
   {
      for( int s =0; s <=numState-1; s++ ){ delete[] pppGamma[i][s]; }
      delete[] pppGamma[i];
   }
   delete[] pppGamma;
}

// To find Bahl-Cocke-Jelinek-Raviv (BCJR) algoritm's coefficients.
// \gamma's are computed first, and \alpha's and \beta's need \gamma's. 
// Recall that they are log values!
void tabulateAlphaBetaGamma( double** ppA, double** ppB, double*** pppG,
      const double* pY_sys, const double* pY_par, const double* pL_a, std::size_t aa )
{
   // to initialize
   ppA[0][0] =0;
   ppB[lenSent][0] =0;
   for( std::size_t s =1; s <=numState-1; s++ )
   {
      ppA[0][s] =negInf;
      ppB[lenSent][s] =negInf;
   }

   // to save \gamma's
   // sPrev corresponds to i =0,...,len-1;
   // sNow corresponds to i =1,...,len.
   for( std::size_t i =0; i <=lenSent-1; i++ )
   {
      for( std::size_t sPrev =0; sPrev <=numState-1; sPrev++ )
      {
         for( std::size_t sNow =0; sNow <=numState-1; sNow++ )
         {
            // according to the transition from sPrev to sNow:
            bool pBit_sys;// info bit of system
            bool pBit_par;// info bit of parity
            double pC_sys;// amplitude of system
            double pC_par;// amplitude of parity

            // if false, it's impossible of sPrev to proceed to sNow
            if( !getFromTrellisState( i, sPrev, sNow, pBit_sys, pBit_par ) ){ continue; }

            // otherwise, trellis exists that goes from sPrev to sNow
            // pBit_sys, pBit_par have been assigned inside condition
            pC_sys =infoToAmp( pBit_sys, aa );
            pC_par =infoToAmp( pBit_par, aa );

            double diffSys =pY_sys[i] -pC_sys;
            double diffPar =pY_par[i] -pC_par;
            // see Proakis, Digital Communications, 6e, ch.8, p.545, (8.8-19)
            double tmpProb =(pBit_sys) ? ( 1/( exp(-pL_a[i]) +1) ) : ( 1/(exp(pL_a[i]) +1) );
            pppG[i][sPrev][sNow] = log(tmpProb) -log(pi) -log(nn_0) -
                  (diffSys *diffSys +diffPar *diffPar) /nn_0;
         }// end for sNow
      }// end for sPrev
   }

   // to save \alpha's
   // sNow corresponds to i =1,...,len.
   // sPrev corresponds to i =0,...,len-1;
   for( std::size_t i =0; i <=lenSent-1; i++ )
   {
      for( std::size_t sNow =0; sNow <=numState-1; sNow++ )
      {
         std::vector<double> listA;
         for( std::size_t sPrev =0; sPrev <=numState-1; sPrev++ )
         {
            bool pBit_sys;// info bit of system
            bool pBit_par;// info bit of parity
            if( !getFromTrellisState( i, sPrev, sNow, pBit_sys, pBit_par ) ){ continue; }

            // see Proakis, Digital Communications, 6e, ch.8, p.545, (8.8-27)
            listA. push_back( ppA[i][sPrev] +pppG[i][sPrev][sNow] );
         }
         ppA[i+1][sNow] =logSumExp(listA);
      }
   }

   // to save \beta's (in reverse order)
   // sNow corresponds to i =len,...,1;
   // sPrev corresponds to i =len-1,...,0.
   for( std::size_t i =lenSent; i >=1; i-- )
   {
      for( std::size_t sPrev =0; sPrev <=numState-1; sPrev++ )
      {
         std::vector<double> listB;
         for( std::size_t sNow =0; sNow <=numState-1; sNow++ )
         {
            bool pBit_sys;// info bit of system
            bool pBit_par;// info bit of parity
            if( !getFromTrellisState( i, sPrev, sNow, pBit_sys, pBit_par ) ){ continue; }

            // see Proakis, Digital Communications, 6e, ch.8, p.545, (8.8-27)
            listB. push_back( ppB[i][sNow] +pppG[i-1][sPrev][sNow] );
         }
         ppB[i-1][sPrev] =logSumExp(listB);
      }
   }
}

// to get system and parity bits from previous and present states,
// returning true if found, false if not.
bool getFromTrellisState( std::size_t idx, std::size_t sPrev, std::size_t sNow,
      bool& holdSys, bool& holdPar )
{
   bool parTest;
   std::size_t holdState =sPrev;

   pressOneCode( idx, holdState, false, parTest );
   if( holdState ==sNow )
   {
      holdSys =false;
      holdPar =parTest;
      return true;
   }

   holdState =sPrev;
   pressOneCode( idx, holdState, true, parTest );
   if( holdState ==sNow )
   {
      holdSys =true;
      holdPar =parTest;
      return true;
   }

   return false;
}

/* * * * * * * * helper functions * * * * * * * */

double logSumExp( const std::vector<double>& vec_in )
{
   std::vector<double> vec(vec_in);
   std::sort( vec.begin(), vec.end(), std::greater<double>() );// sorted in decreasing order
   size_t sizeVec =vec.size();
   if( sizeVec ==1 ){ return vec[0]; }

   //return vec[0]; // XXX

   double sumExpDiff =0;// sum of exponentail of difference
   for( size_t idx =1; idx <= sizeVec -1; idx++ )
   {
      if( vec[idx] <minExpArg ){ continue; }
      sumExpDiff  +=std::exp( vec[idx]-vec[0] );
   }

   return vec[0] +std::log(1+sumExpDiff);
}

double infoToAmp( bool pBit, double amp ){ return ( (pBit) ? amp : (-amp) ); }

// function overloading: for double
void deinterleave( const double* pIn_perm, double* pIn )
{
   for( std::size_t k =0; k <= len_k-1; k++ )
   {
      for( std::size_t l =0; l <= len_l-1; l++ )
      {
         std::size_t idx =k *len_l +l;
         std::size_t kNew =idx %len_k;
         std::size_t lNew =(idx -kNew) /len_k;
         std::size_t idxNew =kNew *len_l +lNew;
         pIn[idx] =pIn_perm[idxNew];
      }
   }
}

// function overloading: for bool
void deinterleave( const bool* pIn_perm, bool* pIn )
{
   for( std::size_t k =0; k <= len_k-1; k++ )
   {
      for( std::size_t l =0; l <= len_l-1; l++ )
      {
         std::size_t idx =k *len_l +l;
         std::size_t kNew =idx %len_k;
         std::size_t lNew =(idx -kNew) /len_k;
         std::size_t idxNew =kNew *len_l +lNew;
         pIn[idx] =pIn_perm[idxNew];
      }
   }
}

// function overloading: for double
void zero( double* p, std::size_t len )
{
   for( std::size_t i =0; i <=len-1; i++ ){ p[i] =0; }
}

// function overloading: for bool
void zero( bool* p, std::size_t len )
{
   for( std::size_t i =0; i <=len-1; i++ ){ p[i] =0; }
}

