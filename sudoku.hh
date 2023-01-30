/***************************************************************************
                                 sudoku.hh                                  
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

/** @file sudoku.hh
    @brief 

*/

#ifndef SUDOKU_H
#define SUDOKU_H

#include <cell.hh>
#include <iosfwd>
#include <stdint.h>

struct Sudoku
{
  struct PlayError {};
  
  Sudoku();
  ~Sudoku();
  
  void                  open(std::istream& source);
  void                  open(std::istream&& source) { return open( source ); }
  void                  play(std::ostream& logger, int _depth = 0);
  bool                  rulestep();
  void                  save(std::ostream& sink) const;
  void                  save(std::ostream&& sink) const { return save( sink ); }
  std::ostream&         dumpiteration( std::ostream& _sink, int _step, int _depth ) const;
  std::ostream&         dump(std::ostream& _sink, int _depth = 0) const;
  unsigned              figcount() const;

  struct Zone { enum Code { Row, Column, Region, end, beg = Row  } code; };

  Cell&                 getcell( Zone::Code, int index, int offset );
  
  Sudoku&               operator=(Sudoku const& _s);
  bool                  operator!=(Sudoku const& _s) const;

  Cell                  cells[9][9];

  static struct Combinations
  {
    enum { count = 1<<9 };
    Combinations();
    Cell items[count];
    Cell* begin() { return &items[1]; }
    Cell* end() { return &items[count-1]; }
  } combinations;
};

#endif // SUDOKU_H
