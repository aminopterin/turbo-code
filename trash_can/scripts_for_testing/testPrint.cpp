#include <iostream>
#include <cstddef>
#include <vector>
#include <ctime>
#include <cstdio>

void pressOneCode( std::size_t idx, std::size_t& state, bool b, bool& par );
void showState( bool, std::size_t, bool );

/* * * * * * * * * */

int main(void){
   std::size_t state=0;
   std::size_t oldState=0;
   std::srand(std::time(NULL));
   for( std::size_t i=0; i<=10; i++ )
   {
      bool par;
      bool b=((rand()%2==1)?true:false);
      oldState =state;
      pressOneCode(i,state,b,par);
      showState( b, oldState, par );
   }
}
