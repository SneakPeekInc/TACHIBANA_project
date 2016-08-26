#include <iostream>
#include <sstream>

#include "position.hpp"

using namespace std;

std::string SFEN_HIRATE = "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b - 1";

// ----------------------------------
//  Position::set()とその逆変換sfen()
// ----------------------------------

void Position::clear()
{
  // このゼロクリアによりstateStateもゼロクリアされる
  // std::memset(this, 0, sizeof(Position));
  // st = &startState;
}


// Pieceを綺麗に出力する(USI形式ではない) 先手の駒は大文字、後手の駒は小文字、成り駒は先頭に+がつく。盤面表示に使う。
#ifndef PRETTY_JP
std::string pretty(Piece pc) { return std::string(USI_PIECE).substr(pc * 2, 2); }
#else
std::string pretty(Piece pc) { return std::string(" □ 歩 香 桂 銀 角 飛 金 玉 と 杏 圭 全 馬 龍 菌 王^歩^香^桂^銀^角^飛^金^玉^と^杏^圭^全^馬^龍^菌^王").substr(pc * 3, 3); }
#endif

// sfen文字列で盤面を設定する
void Position::set(std::string sfen)
{
  // 変な入力をされることはあまり想定していない。
  // sfen文字列は、普通GUI側から渡ってくるのでおかしい入力であることはありえないからである。

  // --- 盤面

  // 盤面左上から。Square型のレイアウトに依らずに処理を進めたいため、Square型は使わない。
  File f = FILE_9;
  Rank r = RANK_1;

  std::istringstream ss(sfen);
  // 盤面を読むときにスペースが盤面と手番とのセパレーターなのでそこを読み飛ばさないようにnoskipwsを指定しておく。
  ss >> std::noskipws;

  uint8_t token;
  bool promote = false;
  size_t idx;

#ifndef EVAL_NO_USE
  // PieceListを更新する上で、どの駒がどこにあるかを設定しなければならないが、
  // それぞれの駒をどこまで使ったかのカウンター
  PieceNo piece_no_count[KING] = { PIECE_NO_ZERO,PIECE_NO_PAWN,PIECE_NO_LANCE,PIECE_NO_KNIGHT,
    PIECE_NO_SILVER, PIECE_NO_BISHOP, PIECE_NO_ROOK,PIECE_NO_GOLD };
#endif

  kingSquare[BLACK] = kingSquare[WHITE] = SQ_NB;

  while ((ss >> token) && !isspace(token))
  {
    // 数字は、空の升の数なのでその分だけ筋(File)を進める
    if (isdigit(token))
      f -= File(token - '0');
    // '/'は次の段を意味する
    else if (token == '/')
    {
      f = FILE_9;
      ++r;
    }
    // '+'は次の駒が成駒であることを意味する
    else if (token == '+')
      promote = true;
    // 駒文字列か？
    else if ((idx = PieceToCharBW.find(token)) != string::npos)
    {
      PieceNo piece_no =
        (idx == B_KING) ? PIECE_NO_BKING : // 先手玉
        (idx == W_KING) ? PIECE_NO_WKING : // 後手玉
#ifndef EVAL_NO_USE
        piece_no_count[raw_type_of(Piece(idx))]++; // それ以外
#else
        PIECE_NO_ZERO; // とりあえず駒番号は使わないので全部ゼロにしておけばいい。
#endif

      // 盤面の(f,r)の駒を設定する
      put_piece(f | r, Piece(idx + (promote ? PIECE_PROMOTE : 0)), piece_no);

      // 1升進める
      --f;

      // 成りフラグ、戻しておく。
      promote = false;
    }

  }

  // --- 手番

 ss >> token;
 sideToMove = (token == 'w' ? WHITE : BLACK);
 ss >> token; // 手番と手駒とを分かつスペース

} //end of Position::set


// ----------------------------------
//           Positionの表示
// ----------------------------------

// 盤面を出力する。(USI形式ではない) デバッグ用。
std::ostream& operator<<(std::ostream& os,const Position& pos)
{
  // 盤面
  for (Rank r = RANK_1; r <= RANK_9; ++r)
  {
    for (File f = FILE_9; f >= FILE_1; --f)
      //os << pretty(pos.board[f | r]);
      os << pos.board[f | r];
    os << endl;
  }


#ifndef PRETTY_JP
  // 手駒
  os << "BLACK HAND : " << pos.hand[BLACK] << " , WHITE HAND : " << pos.hand[WHITE] << endl;

  // 手番
  os << "Turn = " << pos.sideToMove << endl;
#else
  os << "先手 手駒 : " << pos.hand[BLACK] << " , 後手 手駒 : " << pos.hand[WHITE] << endl;
  os << "手番 = " << pos.sideToMove << endl;
#endif

  // sfen文字列もおまけで表示させておく。(デバッグのときに便利)
  //os << "sfen " << pos.sfen() << endl;


  return os;
}
