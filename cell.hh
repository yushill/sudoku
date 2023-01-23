/***************************************************************************
                                 cell.hh                                  
                             -----------------
    begin                : Thu May 22 2003
    copyright            : (C) 2003-2011 CEA and Universite Paris Sud
    authors              : Yves Lhuillier
    email                : yves.lhuillier@cea.fr
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
  Cell();
  Cell( bool _lite );
  Cell( bool _lite, unsigned _f );

  Cell                operator & ( Cell const& _f ) const;
  Cell                operator | ( Cell const& _f ) const;
  Cell                operator ~ () const;
  
  Cell&               operator = ( Cell const& _f );
  Cell&               operator &= ( Cell const& rhs );
  
  bool                  operator == ( Cell const& _f ) const;
  bool                  operator != ( Cell const& _f ) const;
  operator bool () const;
  unsigned              figure() const;
  
  std::ostream&         dump( std::ostream& ) const;

private:
  uint32_t              figures;
  
  Cell( uint32_t );
};

#endif // CELL_H
