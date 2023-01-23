/***************************************************************************
                                 cell.cc                                  
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

/** @file cell.cc
    @brief 

*/

#include <cell.hh>
#include <ostream>

using namespace std;

Cell::Cell()
  : figures( 0x03fe )
{}

Cell::Cell( bool _lite = false )
  : figures( _lite ? 0x03fe : 0x0000 )
{}

Cell::Cell( bool _lite, unsigned fig )
  : figures( (_lite ? (1ul << fig):~(1ul << fig)) & 0x03fe )
{}

Cell::Cell( uint32_t _flags )
  : figures( _flags & 0x03fe )
{}

Cell
Cell::operator & ( Cell const& _f ) const
{
  return Cell( (figures & _f.figures) & 0x03fe );
}

Cell
Cell::operator | ( Cell const& _f ) const
{
  return Cell( (figures | _f.figures) & 0x03fe );
}

Cell
Cell::operator ~ () const
{
  return Cell( (~figures) & 0x03fe );
}

Cell&
Cell::operator = ( Cell const& _f )
{
  figures = _f.figures & 0x03fe;
  return *this;
}

Cell&
Cell::operator &= ( Cell const& rhs )
{
  figures &= rhs.figures & 0x03fe;
  return *this;
}

bool
Cell::operator == ( Cell const& _f ) const
{
  return (figures & 0x03fe) == (_f.figures & 0x03fe);
}

bool
Cell::operator != ( Cell const& _f ) const
{
  return (figures & 0x03fe) != (_f.figures & 0x03fe);
}

unsigned
Cell::figure() const
{
  return (figures & (figures -1)) ? 0 : __builtin_ctz(figures);
}

Cell::operator bool () const
{
  return figures != 0ul;
}

ostream&
Cell::dump( ostream& _sink ) const
{
  for (int idx = 9; idx > 0; idx--)
    {
      if ((figures >> idx) & 1)
        _sink << idx;
      else
        _sink << '-';
    }

  return _sink;
}
