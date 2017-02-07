
#include <iostream>
#include <string>
#include "testInclude.cpp"

int main(void)
{
// following: for testing function splitDigits
/*
for( std::size_t i=0; i <=15; i++ )
{
bool* p =new bool[4];
//std::cout << "beginning: i: " << i << "\n";
splitDigits(i,p,4);
std::cout<< std::to_string(i) << "becomes: " << p[0] << p[1] << p[2] << p[3] << "\n";
delete[] p;
}
*/

// following: finding terminating sequence
/*
   std::size_t nBit =3;
   std::size_t powBit =8;
   std::size_t nState =8;
   bool* pArr =new bool[nBit];
   std::size_t* terminationTable =new std::size_t[nState];

   for( std::size_t state=1; state <=nState-1; state++ )
   {
      std::cout << "for state " << state << ": \n";
      for( std::size_t n=0; n <=powBit-1; n++ )
      {
         std::size_t newState =state;
         splitDigits( n, pArr, nBit );
         //std::cout << "trying sequence " << n << ": \n";
         for( std::size_t idx=0; idx<=nBit-1; idx++ )
         {
            bool par;
            pressOneCode( idx, newState, pArr[idx], par );
            //showState( pArr[idx], newState, par );
            //showState( 0, newState, par );
         }
         if( newState ==0 ){ terminationTable[state] =n; goto tag_found; }
      }
      std::cout << "search failed when input state = " << state << "; exit.";
      exit(1);
      tag_found: ;
   }

   std::cout<< "all trellis-termination sequences found!\n";
   for( std::size_t state=1; state <=nState-1; state++ )
   {
      std::cout<< "for state " << state << ", input " << terminationTable[state] << ".\n";
   }
   delete[] terminationTable;
*/
   std::size_t pTermSequence[] ={ 0, 4, 6, 2, 3, 7, 5, 1 };
   for( std::size_t s =0; s <=7; s++ )
   {
      std::size_t sigma =s;
      bool holdPar =0;
      std::cout << "for state " << sigma << ": ";
      bool pTermBits[3] ={false};
      splitDigits( pTermSequence[sigma], pTermBits, 3 );
      for( std::size_t m =0; m <=2; m++ )
      {
         pressOneCode( m, sigma, pTermBits[m], holdPar );
      }
      std::cout << "in the end, sigma = " << sigma << "\n";
   }
}
