
#include <iostream>
#include <cstddef>
#include <vector>
#include <ctime>
#include <cstdio>

const std::size_t len_k =9;
const std::size_t len_l =12;
const std::size_t lenTot =len_k * len_l;
//std::cout << "pIn_perm[" << idx << "] =pIn[" << idxNew << "]\n";

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

void splitDigits( std::size_t number, bool* pArr, std::size_t len )
{
   std::size_t hold =number;
   std::size_t rem =0;// remainder
   std::size_t msb =1;// see below
   for( std::size_t i =1; i <=len-1; i++ ){ msb *=2; }// msb =2 ^ {len-1}

   for( std::size_t i =0; i <=len-1; i++ )
   {
      rem =hold %msb;
      pArr[i] =(hold -rem)/msb;
      msb /=2;
      hold =rem;
   }
}

/*
void showState( bool tmp, std::size_t r, bool par )
{
   std::cout<< (tmp?1:0);
   std::cout<< " --> || " << r/4;
   r=r%4;
   std::cout<< " --> " << r/2;
   r=r%2;
   std::cout<< " --> " << r;
   std::cout<< " || ~~> " << par << "\n";
}
*/

void pressOneCode( std::size_t idx, std::size_t& state, bool b, bool& par )
 {
    // initialize register holders;
    // [0] is input; [1], [2], [3] are registers.
    bool reg[4] ={false};// universal zero-initializer, all to false
 
    // extracting digits
    std::size_t msb =4;// most significant bit
    std::size_t cnt =1;// 1-based, saving [0] for the upcoming
    std::size_t rest =state;
    while(true)
    {
       if( cnt >=4 ){ break; }
 
       std::size_t rem =rest %msb;// remainder
       rest -=rem;
       reg[cnt] =rest /msb;
       rest =rem;
 
       cnt++;
       msb /= 2;
    }
 
    // feedback
    reg[0] = ( (b != reg[2]) != reg[3] );
    // parity bit
    //if( idx%3 ==0 ){ par =false; }// puncturing
    //else{ par = ( (reg[0] != reg[1]) != reg[3] ); }// as usual
    par = ( (reg[0] != reg[1]) != reg[3] );// as usual
    // push new values into registers
    reg[3] =reg[2];
    reg[2] =reg[1];
    reg[1] =reg[0];
 
    // save registers back into "state"
    state =0;
    if( reg[1] ){ state +=4; }
    if( reg[2] ){ state +=2; }
    if( reg[3] ){ state +=1; }
 }
