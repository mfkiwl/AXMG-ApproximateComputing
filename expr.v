module ex( \pi0 , \pi1 , \pi2 , \pi3 , \pi4 , \pi5 , \po0 );
  input \pi0 , \pi1 , \pi2 , \pi3 , \pi4 , \pi5 ;
  output \po0 ;
  wire zero , w7 , w8 , w9 , w10 , w11 ;
  assign zero = 0;
  assign w7 = \pi4 & \pi5 ;
  assign w8 = \pi2 & \pi3 ;
  assign w9 = ( \pi1 & w7 ) | ( \pi1 & ~w8 ) | ( w7 & ~w8 ) ;
  assign w10 = ( \pi0 & w7 ) | ( \pi0 & w9 ) | ( w7 & w9 ) ;
  assign w11 = w8 | w10 ;
  assign \po0 = w11 ;
endmodule
