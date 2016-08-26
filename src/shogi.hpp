#ifndef _SHOGI_H_
#define _SHOGI_H_

#include "extra/config.hpp"

// --------------------
//      手番
// --------------------

// 手番
enum Color { BLACK=0/*先手*/,WHITE=1/*後手*/,COLOR_NB /* =2 */ , COLOR_ALL = 2 /*先後共通の何か*/ , COLOR_ZERO = 0,};

// 相手番を返す
constexpr Color operator ~(Color c) { return (Color)(c ^ 1);  }

// 正常な値であるかを検査する。assertで使う用。
constexpr bool is_ok(Color c) { return COLOR_ZERO <= c && c < COLOR_NB; }

// --------------------
//        筋
// --------------------

//  例) FILE_3なら3筋。
enum File { FILE_1, FILE_2, FILE_3, FILE_4, FILE_5, FILE_6, FILE_7, FILE_8, FILE_9, FILE_NB, FILE_ZERO=0 };

// 正常な値であるかを検査する。assertで使う用。
constexpr bool is_ok(File f) { return FILE_ZERO <= f && f < FILE_NB; }

// USIの指し手文字列などで筋を表す文字列をここで定義されたFileに変換する。
inline File toFile(char c) { return (File)(c - '1'); }

// Fileを綺麗に出力する(USI形式ではない)
// "PRETTY_JP"をdefineしていれば、日本語文字での表示になる。例 → ８
// "PRETTY_JP"をdefineしていなければ、数字のみの表示になる。例 → 8
std::string pretty(File f);

// USI形式でFileを出力する
inline std::ostream& operator<<(std::ostream& os, File f) { os << (char)('1' + f); return os; }

// --------------------
//        段
// --------------------

// 例) RANK_4なら4段目。
enum Rank { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_NB, RANK_ZERO = 0};

// 正常な値であるかを検査する。assertで使う用。
constexpr bool is_ok(Rank r) { return RANK_ZERO <= r && r < RANK_NB; }

// --------------------
//        駒
// --------------------
enum Piece : int32_t
{
  NO_PIECE, PAWN, LANCE, KNIGHT, SILVER, BISHOP, ROOK, GOLD, KING = 8,
  PRO_PAWN, PRO_LANCE, PRO_KNIGHT, PRO_SILVER, HORSE, DRAGON, QUEEN/*未使用*/,

  B_PAWN = 1 , B_LANCE, B_KNIGHT, B_SILVER, B_BISHOP, B_ROOK, B_GOLD , B_KING,
  B_PRO_PAWN, B_PRO_LANCE, B_PRO_KNIGHT, B_PRO_SILVER, B_HORSE, B_DRAGON, B_QUEEN,

  W_PAWN = 17, W_LANCE, W_KNIGHT, W_SILVER, W_BISHOP, W_ROOK, W_GOLD , W_KING,
  W_PRO_PAWN, W_PRO_LANCE, W_PRO_KNIGHT, W_PRO_SILVER, W_HORSE, W_DRAGON, W_QUEEN,
  PIECE_NB, // 終端
  PIECE_ZERO = 0,

  // --- 特殊な定数

  PIECE_PROMOTE = 8, // 成り駒と非成り駒との差(この定数を足すと成り駒になる)
  PIECE_WHITE = 16,  // これを先手の駒に加算すると後手の駒になる。
  PIECE_RAW_NB = 8,  // 非成駒の終端

  PIECE_HAND_ZERO = PAWN, // 手駒の開始位置
  PIECE_HAND_NB = KING  , // 手駒になる駒種の最大+1

  HDK = KING,       // Position::pieces()で使うときの定数。H=Horse,D=Dragon,K=Kingの合体したBitboardが返る。

  // 指し手生成(GeneratePieceMove = GPM)でtemplateの引数として使うマーカー的な値。変更する可能性があるのでユーザーは使わないでください。
  // 値はマイナスにしておくことで、連続的な値になり、テーブルジャンプしやすくする。
  GPM_BR   = -1 ,     // Bishop Rook
  GPM_GBR  = -2 ,     // Gold Bishop Rook
  GPM_GHD  = -3 ,     // Gold Horse Dragon
  GPM_GHDK = -4 ,     // Gold Horse Dragon King
};

// USIプロトコルで駒を表す文字列を返す。
// 駒打ちの駒なら先頭に"D"。
inline std::string usi_piece(Piece pc) { return std::string((pc & 32) ? "D":"")
  + std::string(". P L N S B R G K +P+L+N+S+B+R+G+.p l n s b r g k +p+l+n+s+b+r+g+k").substr((pc & 31) * 2, 2); }

// 駒に対して、それが先後、どちらの手番の駒であるかを返す。
constexpr Color color_of(Piece pc) { return (pc & PIECE_WHITE) ? WHITE : BLACK; }

// 後手の歩→先手の歩のように、後手という属性を取り払った駒種を返す
constexpr Piece type_of(Piece pc) { return (Piece)(pc & 15); }

// 成ってない駒を返す。後手という属性も消去する。
// 例) 成銀→銀 , 後手の馬→先手の角
// ただし、pc == KINGでの呼び出しはNO_PIECEが返るものとする。
constexpr Piece raw_type_of(Piece pc) { return (Piece)(pc & 7); }

// USIで、盤上の駒を表現する文字列
// ※　歩Pawn 香Lance 桂kNight 銀Silver 角Bishop 飛Rook 金Gold 王King
extern std::string PieceToCharBW;

// Pieceの整合性の検査。assert用。
constexpr bool is_ok(Piece pc) { return NO_PIECE <= pc && pc < PIECE_NB; }

// --------------------
//        駒箱
// --------------------

// Positionクラスで用いる、駒リスト(どの駒がどこにあるのか)を管理するときの番号。
enum PieceNo {
  PIECE_NO_PAWN = 0, PIECE_NO_LANCE = 18, PIECE_NO_KNIGHT = 22, PIECE_NO_SILVER = 26,
  PIECE_NO_GOLD = 30, PIECE_NO_BISHOP = 34, PIECE_NO_ROOK = 36, PIECE_NO_KING = 38,
  PIECE_NO_BKING = 38, PIECE_NO_WKING = 39, // 先手、後手の玉の番号が必要な場合はこっちを用いる
  PIECE_NO_ZERO = 0, PIECE_NO_NB = 40,
};

// PieceNoの整合性の検査。assert用。
constexpr bool is_ok(PieceNo pn) { return PIECE_NO_ZERO <= pn && pn < PIECE_NO_NB; }

// --------------------
//       手駒
// --------------------

// 手駒
// 歩の枚数を8bit、香、桂、銀、角、飛、金を4bitずつで持つ。こうすると16進数表示したときに綺麗に表示される。(なのはのアイデア)
enum Hand : uint32_t { HAND_ZERO = 0, };


// --------------------
//        升目
// --------------------

// 盤上の升目に対応する定数。
// 盤上右上(１一が0)、左下(９九)が80
enum Square : int32_t
{
  // 以下、盤面の右上から左下までの定数。
  // これを定義していなくとも問題ないのだが、デバッガでSquare型を見たときに
  // どの升であるかが表示されることに価値がある。
  SQ_11, SQ_12, SQ_13, SQ_14, SQ_15, SQ_16, SQ_17, SQ_18, SQ_19,
  SQ_21, SQ_22, SQ_23, SQ_24, SQ_25, SQ_26, SQ_27, SQ_28, SQ_29,
  SQ_31, SQ_32, SQ_33, SQ_34, SQ_35, SQ_36, SQ_37, SQ_38, SQ_39,
  SQ_41, SQ_42, SQ_43, SQ_44, SQ_45, SQ_46, SQ_47, SQ_48, SQ_49,
  SQ_51, SQ_52, SQ_53, SQ_54, SQ_55, SQ_56, SQ_57, SQ_58, SQ_59,
  SQ_61, SQ_62, SQ_63, SQ_64, SQ_65, SQ_66, SQ_67, SQ_68, SQ_69,
  SQ_71, SQ_72, SQ_73, SQ_74, SQ_75, SQ_76, SQ_77, SQ_78, SQ_79,
  SQ_81, SQ_82, SQ_83, SQ_84, SQ_85, SQ_86, SQ_87, SQ_88, SQ_89,
  SQ_91, SQ_92, SQ_93, SQ_94, SQ_95, SQ_96, SQ_97, SQ_98, SQ_99,

  // ゼロと末尾
  SQ_ZERO = 0, SQ_NB = 81,
  SQ_NB_PLUS1 = SQ_NB + 1, // 玉がいない場合、SQ_NBに移動したものとして扱うため、配列をSQ_NB+1で確保しないといけないときがあるのでこの定数を用いる。

  // 方角に関する定数。StockfishだとN=北=盤面の下を意味するようだが…。
  SQ_D  = +1, // 下(Down)
  SQ_R  = -9, // 右(Right)
  SQ_U  = -1, // 上(Up)
  SQ_L  = +9, // 左(Left)

  // 斜めの方角などを意味する定数。
  SQ_RU = int(SQ_U) + int(SQ_R), // 右上(Right Up)
  SQ_RD = int(SQ_D) + int(SQ_R), // 右下(Right Down)
  SQ_LU = int(SQ_U) + int(SQ_L), // 左上(Left Up)
  SQ_LD = int(SQ_D) + int(SQ_L), // 左下(Left Down)
  SQ_RUU = int(SQ_RU) + int(SQ_U), // 右上上
  SQ_LUU = int(SQ_LU) + int(SQ_U), // 左上上
  SQ_RDD = int(SQ_RD) + int(SQ_D), // 右下下
  SQ_LDD = int(SQ_LD) + int(SQ_D), // 左下下
};

// sqが盤面の内側を指しているかを判定する。assert()などで使う用。
// 駒は駒落ちのときにSQ_NBに移動するので、値としてSQ_NBは許容する。
constexpr bool is_ok(Square sq) { return SQ_ZERO <= sq && sq <= SQ_NB; }

// sqが盤面の内側を指しているかを判定する。assert()などで使う用。玉は盤上にないときにSQ_NBを取るのでこの関数が必要。
constexpr bool is_ok_plus1(Square sq) { return SQ_ZERO <= sq && sq < SQ_NB_PLUS1; }

extern File SquareToFile[SQ_NB];

// 筋(File)と段(Rank)から、それに対応する升(Square)を返す。
inline Square operator | (File f, Rank r) { Square sq = (Square)(f * 9 + r); ASSERT_LV2(is_ok(sq)); return sq; }


// --------------------
//  operators and macros
// --------------------

#include "extra/macros.hpp"


#endif // of #ifndef _SHOGI_H_
