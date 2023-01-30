/***************************************************************************
                                 cell.hh                                  
                             -----------------
    begin                : Thu May 22 2003
    authors              : Yves Lhuillier
    email                : yves.lhuillier@gmail.com
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 3        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

/** @file cell.hh
    @brief 

*/

#ifndef CELL_H
#define CELL_H

#include <iosfwd>
#include <stdint.h>

struct Cell
{
  enum { count = 9, all = 0x3fe };
  struct Figure { unsigned value; };
  struct Figures { unsigned value; };

  Cell();
  Cell( Figure fig );
  Cell( Figures figs );


  Cell                operator & ( Cell const& _f ) const;
  Cell                operator | ( Cell const& _f ) const;
  Cell                operator ~ () const;
  
  bool                keep( Cell const& exc );
  Cell&               operator = ( Cell const& _f );
  Cell&               operator &= ( Cell const& rhs );
  
  bool                operator == ( Cell const& _f ) const;
  bool                operator != ( Cell const& _f ) const;
  operator bool () const;
  unsigned            figures() const { return bits; }
  unsigned            popcount() const { return __builtin_popcount(bits); }
  unsigned            figure() const;
  
  std::ostream&       dump( std::ostream& ) const;

private:
  unsigned            bits;
};

#endif // CELL_H
