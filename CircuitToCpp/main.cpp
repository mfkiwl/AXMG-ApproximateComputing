#include <iostream>
#include <vector>
#include <string>

int pi0 , pi1 , pi2 , pi3 , pi4 , pi5 ;
int po0 ;
int zero , w7 , w8 , w9 , w10 , w11 ;

int main() {
    zero = 0;
    w7 = pi4 & pi5 ;
    w8 = pi2 & pi3 ;
    w9 = ( pi1 & w7 ) | ( pi1 & ~w8 ) | ( w7 & ~w8 ) ;
    w10 = ( pi0 & w7 ) | ( pi0 & w9 ) | ( w7 & w9 ) ;
    w11 = w8 | w10 ;
    po0 = w11 ;
    return 0;
}