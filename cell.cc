/***************************************************************************
                                 cell.cc                                  
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

/** @file cell.cc
    @brief 

*/

#include <cell.hh>
#include <ostream>

using namespace std;

Cell::Cell()
  : bits( all )
{}

// Cell::Cell( bool lite = false )
//   : bits( _lite ? all : 0x0000 )
// {}

// Cell::Cell( bool _lite, unsigned fig )
//   : bits( (_lite ? (1ul << fig):~(1ul << fig)) & all )
// {}

Cell::Cell( Figure fig )
  : bits( (1 << fig.value) & all  )
{}

Cell::Cell( Figures figs )
  : bits( figs.value & all  )
{}

Cell
Cell::operator & ( Cell const& _f ) const
{
  return Cell::Figures{bits & _f.bits};
}

Cell
Cell::operator | ( Cell const& _f ) const
{
  return Cell::Figures{bits | _f.bits};
}

Cell
Cell::operator ~ () const
{
  return Cell::Figures{(~bits) & all};
}

Cell&
Cell::operator = ( Cell const& _f )
{
  bits = _f.bits & all;
  return *this;
}

Cell&
Cell::operator &= ( Cell const& rhs )
{
  bits &= rhs.bits & all;
  return *this;
}

bool
Cell::operator == ( Cell const& _f ) const
{
  return (bits & all) == (_f.bits & all);
}

bool
Cell::operator != ( Cell const& _f ) const
{
  return (bits & all) != (_f.bits & all);
}

unsigned
Cell::figure() const
{
  return (bits & (bits -1)) ? 0 : __builtin_ctz(bits);
}

Cell::operator bool () const
{
  return bits != 0ul;
}

ostream&
Cell::dump( ostream& _sink ) const
{
  for (int idx = 9; idx > 0; idx--)
    {
      if ((bits >> idx) & 1)
        _sink << idx;
      else
        _sink << '-';
    }

  return _sink;
}

bool
Cell::keep( Cell const& k )
{
  unsigned old_figures = bits;
  unsigned new_figures = old_figures & k.bits;
  bits = new_figures;
  return new_figures != old_figures;
}

