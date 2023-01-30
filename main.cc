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

/** @file main.cc
    @brief 

*/

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
      std::string input_name = argv[1], output_name;
      {
        char const* midfix = "_solv";
        std::size_t pos = input_name.rfind('.');
        if (pos != input_name.npos)
          output_name = input_name.substr(0,pos) + midfix + input_name.substr(pos);
        else
          output_name = input_name + midfix;
      }
      
      game.open(std::ifstream( input_name ));
      game.play(std::cout);
      game.save(std::ofstream( output_name ));
    }
  catch( Sudoku::PlayError const& )
    {
      std::cerr << "No solution...\n";
      return 1;
    }


  return 0;
}
