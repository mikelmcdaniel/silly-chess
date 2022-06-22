#include <iostream>
#include <sstream>
#include <stdexcept>

#include "chess_board.h"

using namespace std;

void test_board_input() {
  Board b;
  stringstream original_board_stream;
  stringstream original_board_stream2;
  stringstream modified_board_stream;
  
  // b.make_move(b.get_moves()[0]);
  original_board_stream << b;

  b.make_move(b.get_moves()[0]);
  b.make_move(b.get_moves()[0]);
  b.make_move(b.get_moves()[0]);
  b.make_move(b.get_moves()[0]);
  b.make_move(b.get_moves()[0]);
  modified_board_stream << b;
  
  original_board_stream >> b;
  original_board_stream2 << b;
  if (original_board_stream.str() != original_board_stream2.str()) {
    stringstream err;
    err 
      << "Failed to read in original board!: \n"
      << "Original board: \n"
      << original_board_stream.str()
      << "\nModified board: \n"
      << modified_board_stream.str()
      << "\nIncorrectly read original board: \n"
      << original_board_stream.str();
    throw runtime_error(err.str());
  }
}

// int main(int argc, const char *argv[]) {
//   test_board_input();
//   return 0;
// }
