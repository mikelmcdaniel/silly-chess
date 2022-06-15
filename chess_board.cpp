#include <algorithm>
#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "utf8_codepoint.h"
#include "chess_pieces.h"
#include "chess_board.h"

using std::cout;
using std::endl;
using std::istream;
using std::getline;
using std::map;
using std::ostream;
using std::out_of_range;
using std::reverse;
using std::setw;
using std::string;
using std::stringstream;
using std::vector;


const char* team_name(Team team) {
  switch (team) {
    case WHITE:
      return "White";
    case BLACK:
      return "Black";
    case NONE:
      return "None";
  }
  return "UNKNOWN";
}


bool Cell::operator==(Cell other) const {
  return x == other.x && y == other.y;
}

bool Cell::operator!=(Cell other) const {
  return !(*this == other);
}

bool Move::operator==(Move other) const {
  return to == other.to && from == other.from;
}

bool Move::operator!=(Move other) const {
  return !(*this == other);
}


ostream& operator<<(ostream& os, const Cell& cell) {
  return os << static_cast<char>(cell.x + 'a') << cell.y + 1;
}
istream& operator>>(istream& is, Cell& cell) {
  char x_plus_a;
  int y_plus_1;
  is >> x_plus_a >> y_plus_1;
  cell.x = x_plus_a - 'a';
  cell.y = y_plus_1 - 1;
  return is;
}

ostream& operator<<(ostream& os, const Move& move) {
  return os << move.from << move.to;
}
istream& operator>>(istream& is, Move& move) {
  return is >> move.from >> move.to;
}

Board::Board() {
  reset_board();
}

const ChessPiece& Board::operator[](Cell cell) const {
  return *board[cell.y][cell.x];
}

void Board::reset_board() {
  board.resize(8);
  for (vector<const ChessPiece*>& row : board) {
    row.resize(8);
  }

  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      board[y][x] = &EMPTY_SPACE;
    }
  }

  for (int x = 0; x < 8; ++x) {
    board[1][x] = &WHITE_PAWN;
    board[6][x] = &BLACK_PAWN;
  }

  board[0][0] = &WHITE_ROOK;
  board[0][1] = &WHITE_KNIGHT;
  board[0][2] = &WHITE_BISHOP;
  board[0][3] = &WHITE_QUEEN;
  board[0][4] = &WHITE_KING;
  board[0][5] = &WHITE_BISHOP;
  board[0][6] = &WHITE_KNIGHT;
  board[0][7] = &WHITE_ROOK;

  board[7][0] = &BLACK_ROOK;
  board[7][1] = &BLACK_KNIGHT;
  board[7][2] = &BLACK_BISHOP;
  board[7][3] = &BLACK_QUEEN;
  board[7][4] = &BLACK_KING;
  board[7][5] = &BLACK_BISHOP;
  board[7][6] = &BLACK_KNIGHT;
  board[7][7] = &BLACK_ROOK;

  current_teams_turn = WHITE;
}

vector<Move> Board::get_moves() const {
  vector<Move> moves;
  for (int y = 0; y < num_rows(); ++y) {
    for (int x = 0; x < num_cols(); ++x) {
      if (board[y][x]->team == current_teams_turn) {
        board[y][x]->get_moves(*this, Cell(x, y), moves);
      }
    }
  }
  for (Move move : moves) {
    if (!contains(move.to) || !contains(move.from)) {
      stringstream err_msg;
      err_msg << "Board::get_moves got a move that moves to or from a cell that is not on the board: " << move;
      throw out_of_range(err_msg.str());
    }
  }
  return moves;
}

// This function represents how most classical chess ALL_CHESS_PIECES would move.
// This also allows us to add support for more complex "moves", like a pawn
// getting to the end of the board and turning into a queen or some other type
// of piece.
// If we allow the chess piece that's moving to define the move, then we can
// add really interesting custom ALL_CHESS_PIECES that are nothing like normal ALL_CHESS_PIECES!
void Board::make_classical_chess_move(Move move) {
  board[move.to.y][move.to.x] = board[move.from.y][move.from.x];
  board[move.from.y][move.from.x] = &EMPTY_SPACE;
  current_teams_turn = current_teams_turn == WHITE ? BLACK : WHITE;
}

void Board::make_move(Move move) {
  if (!contains(move.to) || !contains(move.from)) {
    stringstream err_msg;
    err_msg << "Board::make_move called with a move that moves to or from a cell that is not on the board: " << move;
    throw out_of_range(err_msg.str());
  }
  board[move.from.y][move.from.x]->make_move(*this, move);
}

bool Board::contains(Cell cell) const {
  return cell.x >= 0 && cell.x < num_cols() && cell.y >= 0 && cell.y < num_rows();
}

Team Board::winner() const {
  bool found_white_king = false, found_black_king = false;
  for (int y = 0; y < num_rows(); ++y) {
    for (int x = 0; x < num_cols(); ++x) {
      if (board[y][x] == &WHITE_KING) {
        found_white_king = true;
      } else if (board[y][x] == &BLACK_KING) {
        found_black_king = true;
      }
    }
  }
  if (!found_white_king) {
    return BLACK;
  }
  if (!found_black_king) {
    return WHITE;
  }
  return NONE;
}


ostream& operator<<(ostream& os, const Board& board) {
  // Print "abc..." row
  os << "   ";
  for (char c = 'a'; c < 'a' + board.num_cols(); ++c) {
    os << c;
  }
  os << '\n';

  for (int y = board.num_rows() - 1; y >= 0; --y) {
    os << setw(2) << (y + 1) << ' ';
    for (int x = 0; x < board.num_cols(); ++x) {
      os << board[Cell(x, y)];
    }
    os << setw(2) << (y + 1) << endl;
  }

  // Print "abc..." row
  os << "   ";
  for (char c = 'a'; c < 'a' + board.num_cols(); ++c) {
    os << c;
  }
  os << '\n';
  return os;
}

istream& getline(istream& is, vector<UTF8CodePoint>& line) {
  UTF8CodePoint cp;
  line.clear();
  while((is >> cp) && cp != '\n') {
    line.push_back(cp);
  }
  return is;
}

istream& operator>>(istream& is, Board& board) {
  string abc_line;
  getline(is, abc_line);

  if (abc_line.find("   ") != 0) {
    stringstream msg;
    msg << "First line of Board does not start with \"   \". We got " << abc_line << " instead.";
    throw std::invalid_argument(msg.str());
  }
  for (int i = 3; i < abc_line.size(); ++i) {
    if (abc_line[i] != 'a' - 3 + i) {
      stringstream msg;
      msg 
        << "First line of Board had '" << abc_line[i] << "' at index " 
        << i << ". We got \"" << abc_line << "\" instead.";
      throw std::invalid_argument(msg.str());
    }
  }
  const int num_cols = abc_line.size() - 3;

  board.board.resize(0);
  vector<UTF8CodePoint> line;
  while(true) {
    int y;
    is >> y;
    if (!is) {
      throw std::invalid_argument("istream is not good after trying to read row number in Board.");
    }
    UTF8CodePoint space;
    is >> space;
    if (space != ' ') {
      throw std::invalid_argument("Expected space after row number!");
    }
    board.board.resize(board.board.size() + 1);
    for (int x = 0; x < num_cols; ++x) {
      UTF8CodePoint piece;
      is >> piece;
      if (!is) {
        throw std::invalid_argument("istream is not good after trying to read a piece.");
      }
      auto it = ALL_CHESS_PIECES.find(piece);
      if (it == ALL_CHESS_PIECES.end()) {
        stringstream msg;
        msg << "UTF8CodePoint " << piece << " at (" << x << ", " << y << ") is not in ALL_CHESS_PIECES!";
        throw std::invalid_argument(msg.str());
      }
      board.board[board.board.size() - 1].push_back(it->second);
    }

    int y2;
    is >> y2;
    if (!is) {
      throw std::invalid_argument("istream is not good after trying to read row number in Board.");
    }
    if (y != y2) {
      throw std::invalid_argument("Row numbers at beginning and end of row do not match!");
    }
    UTF8CodePoint new_line;
    is >> new_line;
    if (new_line != '\n') {
      throw std::invalid_argument("Expected new_line after row number!");
    }

    if (y == 1) {
      break;
    }
  }

  reverse(board.board.begin(), board.board.end());
  
  string abc_line2;
  getline(is, abc_line2);
  if (abc_line != abc_line2) {
    stringstream msg;
    msg << "Last line of Board != first line of board. We got " << abc_line2 << " instead.";
    throw std::invalid_argument(msg.str());
  }
  return is;
}

int Board::num_rows() const {
  return board.size();
}

int Board::num_cols() const {
  return board[0].size();
}