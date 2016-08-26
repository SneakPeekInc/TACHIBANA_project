#include <iostream>
#include "shogi.hpp"
#include "position.hpp"

// コンパイル
// g++ -std=c++11 shogi.cpp position.cpp

Piece board[SQ_NB_PLUS1] = {};

std::string PieceToCharBW(" PLNSBRGK        plnsbrgk");

int main(){
  Position pos;
  std::cout << pos << std::endl;
  return 0;
}
