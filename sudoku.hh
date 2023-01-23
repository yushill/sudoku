/***************************************************************************
                                 sudoku.hh                                  
                             -----------------
    begin                : Thu May 22 2003
    copyright            : (C) 2003-2011 CEA and Universite Paris Sud
    authors              : Yves Lhuillier
    email                : yves.lhuillier@cea.fr
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
  void                  save(std::ostream& sink) const;
  void                  save(std::ostream&& sink) const { return save( sink ); }
  std::ostream&         dumpiteration( std::ostream& _sink, int _step, int _depth ) const;
  std::ostream&         dump(std::ostream& _sink, int _depth = 0) const;
  unsigned              figcount() const;

  Sudoku&               operator=(Sudoku const& _s);
  bool                  operator!=(Sudoku const& _s) const;

  Cell                  cells[9][9];
};

#endif // SUDOKU_H
