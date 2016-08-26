#ifndef _POSITION_H_
#define _POSITION_H_

#include "shogi.hpp"

// --------------------
//     局面の定数
// --------------------

// 平手の開始局面
extern std::string SFEN_HIRATE;


// --------------------
//       盤面
// --------------------

// 盤面
struct Position
{
  Position(){
    set_hirate();
  }

  // 初期化
  void clear();

  // sfen文字列で盤面を設定する
  // ※　内部的にinit()は呼び出される。
  void set(std::string sfen);

  // 局面のsfen文字列を取得する
  // ※ USIプロトコルにおいては不要な機能ではあるが、デバッグのために局面を標準出力に出力して
  // 　その局面から開始させたりしたいときに、sfenで現在の局面を出力出来ないと困るので用意してある。
  //const std::string sfen() const;

  // 平手の初期盤面を設定する。
  void set_hirate() { set(SFEN_HIRATE); }

  // 盤面、81升分の駒。
  Piece board[SQ_NB] = {};

  // 手駒
  Hand hand[COLOR_NB];

  // 手番
  Color sideToMove;

  // 玉の位置
  Square kingSquare[COLOR_NB];

  // 駒を配置して、内部的に保持しているBitboardなどを更新する。
  void put_piece( Square sq , Piece pc, PieceNo piece_no);

  // --- デバッグ用の出力 ---

  // 盤面を出力する。(USI形式ではない) デバッグ用。
  friend std::ostream& operator<<(std::ostream& os, const Position& pos);

};


// 駒を配置する。
inline void Position::put_piece(Square sq, Piece pc,PieceNo piece_no)
{
  //ASSERT_LV2(board[sq] == NO_PIECE);
  board[sq] = pc;

  // bitboard用
  //xor_piece(pc, sq);

  // 駒番号をセットしておく必要がある。
  //ASSERT_LV3(is_ok(piece_no));

/*
#ifndef EVAL_NO_USE
  // evalListのほうを更新しないといけない
  evalList.put_piece(piece_no,sq,pc); // sqの升にpcの駒を配置する
#endif
*/

  // 王なら、その升を記憶しておく。
  // (王の升はBitboardなどをみればわかるが、頻繁にアクセスするのでcacheしている。)
  if (type_of(pc) == KING)
    kingSquare[color_of(pc)] = sq;
}




#endif // of #ifndef _POSITION_H_
