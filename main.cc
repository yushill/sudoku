/***************************************************************************
                                  main.cc
                             -----------------
    begin                : Thu May 22 2003
    authors              : Yves Lhuillier
    email                : yves.lhuillier@gmail.com
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#include <sudoku.hh>
#include <iostream>
#include <fstream>

int
main( int argc, char** argv )
{
  if (argc != 2)
    {
      std::cerr << "Usage: " << argv[0] << " <board_file>\n";
      return 1;
    }

  Sudoku game;

  try
    {
      std::ifstream source( argv[1] );
      game.open( source );
      game.play( std::cout );
    }
  catch( Sudoku::PlayError const& )
    {
      std::cerr << "No solution...\n";
      return 1;
    }

  return 0;
}
