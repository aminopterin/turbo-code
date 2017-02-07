#include <iostream>
#include <cstddef>
#include "testInclude.cpp"


int main(void)
{
   double* pArr =new double[lenTot];
   for( std::size_t i =0; i <=lenTot-1; i++ ) { pArr[i] =static_cast<double>(i); }
   double* pArrPerm =new double[lenTot];

   std::cout << "original:\n";
   for( std::size_t i =0; i <=lenTot-1; i++ )
   {
      std::cout<< pArr[i] << ", ";
      if( (i+1) %len_k ==0 ){ std:: cout << "\n"; }
   }
   std::cout << "\n"; 

   interleave(pArr,pArrPerm);
   std::cout << "after:\n";
   for( std::size_t i =0; i <=lenTot-1; i++ )
   {
      std::cout<< pArrPerm[i] << ", ";
      if( (i+1) %len_l ==0 ){ std:: cout << "\n"; }
   }
   std:: cout << "\n";

   deinterleave(pArrPerm,pArr);
   std::cout << "after:\n";
   for( std::size_t i =0; i <=lenTot-1; i++ )
   {
      std::cout<< pArr[i] << ", ";
      if( (i+1) %len_k ==0 ){ std:: cout << "\n"; }
   }
   std:: cout << "\n";

}
