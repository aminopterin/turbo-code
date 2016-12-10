/* * * * * * * * * * * * * * * *
 filename: main.cpp
 author: Tzu-Yu Jeng
 date: early Sep. 2016
 description: to execute the turbo transmitter, and to simulate
   channel, and to execute the turbo receiver, and finally to save 
   results of SNR vs. BER in a pure text file.
* * * * * * * * * * * * * * * */

#include <cmath> // log, sqrt, round
#include <cstdlib> // srand
#include <ctime> // time
#include <iostream> // cout
#include <sstream> // stringstream
#include <fstream> // ofstream
#include <string> // string

#include "main.h" // constants
#include "transmitter.h"
#include "receiver.h" 

int main( int argc, char** argv )
{
   time_t tStartTot =std::time(0);// starting time of whole program

   // check command line arguments validity
   std::string programName ="turbo";// filename of executable; for error message
   if( argc != 2 )
   {
      std::cerr << "Too few or too many arguments!\n"
            << "Usage: (in such order) "
            << programName << " <output_file>\n";
      exit(1);// indicating failure
   }

   // initialize and check whether output path is good
   std::ofstream* pFileOutput =new std::ofstream;
   pFileOutput-> open (argv[1]);
   if( pFileOutput-> fail() )
   {
      std::cerr << "For some reason, the path \"" << argv[1]
            << "\" cannot be opened ... v_v";
      exit(1);// indicating failure
   }

   // all data to be written into text file:
   double** savedTable =new double* [numCurve];
   for( std::size_t idxCurve =0; idxCurve <=numCurve-1; idxCurve++ )
   {
      savedTable[idxCurve] =new double[numPtSnr];
      for( std::size_t cntSnr =0; cntSnr <=numPtSnr-1; cntSnr++ ){ savedTable [idxCurve] [cntSnr] =0; }
   }

   // but first save into a string stream to facilitate typesetting:
   // (in total, numCurve +1 entries; the 1st being table head)
   std::ostringstream** savedStreams =new std::ostringstream* [numCurve+1];
   for( std::size_t idxCurve =0; idxCurve <=numCurve; idxCurve++ ){ savedStreams[idxCurve] =new std::ostringstream; }
   
   // allocation of pointers and print remaining of the 1st column
   for( std::size_t idxCurve =0; idxCurve <=numCurve-1; idxCurve++ ){ printAligned( savedStreams[ idxCurve+1 ], listNumIter[ idxCurve ] ); }
   // for clarity, print table head first
   printAligned( savedStreams[0], "" );// table header
   double snr =startSnr;
   for( std::size_t cntSnr =0; cntSnr <=numPtSnr-1; cntSnr++ )
   {
      snr *= multSnr;
      printAligned( savedStreams[0], 10 *log(snr) /log(10) );// table header
   }

   std::srand (std::time(0));// to initialize random seed

   //~ ~ ~ ~ ~ ~ ~ ~ ~ calculation begins ~ ~ ~ ~ ~ ~ ~ ~ ~//
   // for each block: (cntBlock does not appear)
   for( std::size_t cntBlock =0; cntBlock <=numBlock-1; cntBlock++ )
   {
      std::cout << "Starting " << cntBlock+1 << "-th simulation:\n";
      snr =startSnr;// declared just above
      //for each snr value:
      for( std::size_t cntSnr =0; cntSnr <=numPtSnr-1; cntSnr++ )
      {
         time_t tStart, tEnd; // to find time elapsed
         tStart = std::time(0); // for reference

         snr *=multSnr;
         std::cout << "   For snr = " << snr << ", process running...\n";

         // allocation of memory
         // information bits
         bool* pX =new bool[lenSent]; zero( pX, lenSent );
         // parity bits through convolutional code
         bool* pX_par =new bool[lenSent]; zero( pX_par, lenSent );
         // interleaved information bits
         bool* pX_perm =new bool[lenSent]; zero( pX_perm, lenSent );
         // parity bits through convolutional code, of interleaved sequence,
         bool* pX_perm_par =new bool[lenSent]; zero( pX_perm_par, lenSent );

         /* * * * * * * * * * * * * * * * * */
         // transmission end
         generateInfoBits(pX);// generate information bits
         rscTransmitter(pX, pX_par);// convolutional code
         interleave(pX, pX_perm);// interleaved
         rscTransmitter(pX_perm, pX_perm_par);// convolutional code after interleaving

         // systematic bits, passed through channel
         double* pY_sys =new double[lenSent]; zero( pY_sys, lenSent );
         // above interleaved
         double* pY_sys_perm =new double[lenSent]; zero( pY_sys_perm, lenSent );
         // parity bits, passed through channel
         double* pY_par =new double[lenSent]; zero( pY_par, lenSent );
         // parity bits of interleaved sequence, passed through channel
         double* pY_perm_par =new double[lenSent]; zero( pY_perm_par, lenSent );

         /* * * * * * * * * * * * * * * * * */
         // inside channel
         signalThruAWGN( lenSent, pX, pY_sys, std::sqrt(snr) );
         signalThruAWGN( lenSent, pX_par, pY_par, std::sqrt(snr) );
         signalThruAWGN( lenSent, pX_perm_par, pY_perm_par, std::sqrt(snr) );

         // to release memory. Only pX is used later for comparison.
         delete[] pX_par;
         delete[] pX_perm;
         delete[] pX_perm_par;

         interleave(pY_sys, pY_sys_perm);// to be fed into 2nd SISO-decoder

         /* * * * * * * * * * * * * * * * * */
         // to start decision process
         double* pL_ext_12 =0;// external info. from interleave to non-interleave
         double* pL_ext_21 =0;// external info. from non-interleave to interleave
         double* pInput1 =0;// external LLR being input to 1st SISO decoder
         double* pInput2 =0;// external LLR being input to 2nd SISO decoder
         double* pL_info =0;// decoded info. bit's LLR
         double* pL_perm_info =0;// decoded interleved info. bit's LLR

         // to print B.E.R. or each number of iteration for this S.N.R.
         std::size_t idxCurve =0;// start from 1st no. of iter.
         std::size_t nextNumIter =listNumIter[idxCurve];
         std::size_t cntIter =0;// count of no. of iteration

         // iterative decoding
         pInput1 =new double[lenSent]; zero( pInput1, lenSent );
         while(true)
         {
            pL_ext_12 =new double[lenSent]; zero( pL_ext_12, lenSent );
            pL_info =new double[lenSent]; zero( pL_info, lenSent );

            sisoReceiver( std::sqrt(snr), pInput1, pY_sys,
                  pY_par, pL_ext_12, pL_info );

            delete[] pInput1;
            pInput2 =new double[lenSent]; zero( pInput2, lenSent );

            interleave(pL_ext_12, pInput2);

            delete[] pL_ext_12;
            pL_ext_21 =new double[lenSent]; zero( pL_ext_21, lenSent );
            pL_perm_info =new double[lenSent]; zero( pL_perm_info, lenSent );

            sisoReceiver( std::sqrt(snr), pInput2, pY_sys_perm,
                  pY_perm_par, pL_ext_21, pL_perm_info );

            delete[] pInput2;
            pInput1 =new double[lenSent]; zero( pInput1, lenSent );

            deinterleave(pL_ext_21, pInput1);

            delete[] pL_ext_21;

            // manual loop control
            cntIter++;
            if( cntIter !=nextNumIter )
            {// only when data are recorded, do we need both LLR info's
               delete[] pL_info;
               delete[] pL_perm_info;
               continue;
            }

            // cntIter ==nextNumIter, thus, to be recorded
            std::size_t numErr =0;// no. of error.
            // In the following, only need to compare up to lenTot
            
            double pFinalInfo[lenSent] ={0};
            deinterleave(pL_perm_info, pFinalInfo);
            for( std::size_t i =0; i <=lenTot-1; i++ )
            {
               pFinalInfo[i] +=pL_info[i];
               pFinalInfo[i] /=2;
               if( pX[i] && (pFinalInfo[i]<0) ){ numErr++; }
               if( (!pX[i]) && (pFinalInfo[i]>0) ){ numErr++; }
            }
            savedTable [idxCurve] [cntSnr] +=static_cast<double>(numErr) /lenTot;

            std::cout << "      iteration " << nextNumIter << " done!\n";
            delete[] pL_info;
            delete[] pL_perm_info;

            if( idxCurve ==numCurve -1 ){ break; }
            idxCurve++;
            nextNumIter =listNumIter[idxCurve];
         }// end iterative decoding
    
         // to release memory of the rest arrays
         // (to be freed once the innermost block ends)
         delete[] pInput1;// recall: originally saved for next loop
         delete[] pX;
         delete[] pY_sys;
         delete[] pY_sys_perm;
         delete[] pY_par;
         delete[] pY_perm_par;

         // to print present time
         tEnd =std::time(0);
         time_t tDiff =std::difftime(tEnd, tStart);
         std::cout << "   ...case snr = " << snr << " completed, for which "
               << tDiff /3600 << "h "
               << (tDiff /60) %60 << "m "
               << tDiff %60 << "s has elapsed.\n";
      }// end for each snr
   }// end for each block

   for( std::size_t idxCurve =0; idxCurve <=numCurve-1; idxCurve++ )
   {
      for( std::size_t cntSnr =0; cntSnr <=numPtSnr-1; cntSnr++ )
      {
         printAligned( savedStreams[idxCurve+1], 10 *log( savedTable [idxCurve] [cntSnr] /numBlock ) /log(10) );
      }
      delete[] savedTable [idxCurve];
   }
   delete[] savedTable;

   // combine all strings of data and output
   std::ostringstream streamTot;
   for( std::size_t idxCurve =0; idxCurve <=numCurve; idxCurve++ )// numCurve +1 rows in total
   {
      streamTot << savedStreams[idxCurve]-> str() << "\n";
      delete savedStreams[idxCurve];
   }
   delete[] savedStreams;

   (*pFileOutput) << "Turbo code with rate " << rateCode <<",\n"
         << "with block size " << lenSent << ",\n"
         << "averaged over " << numBlock << " blocks.\n"
         << "(thus, Shannon limit = " << snrShannon << "dB)\n"
         << "row: no. of iteration.\n"
         << "column: E_b / N_0.\n"
         << "content: B.E.R.\n\n";
   (*pFileOutput) << streamTot.str();
   delete pFileOutput;

   time_t tEndTot =std::time(0);
   time_t tDiffTot =std::difftime(tEndTot, tStartTot);
   std::cout << "The whole program takes, in total, "
         << tDiffTot /3600 << "h "
         << (tDiffTot /60) %60 << "m "
         << tDiffTot %60 << "s.\n";
}
