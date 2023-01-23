/***************************************************************************
                                 sudoku.cc                                  
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

/** @file sudoku.cc
    @brief 

*/

#include <sudoku.hh>
#include <istream>
#include <ostream>

Sudoku::Sudoku() {}

Sudoku::~Sudoku() {}

void
Sudoku::open( std::istream& source )
{
  int idx = 0;
  char c ;
  
  while (source.get( c ))
    {
      int x = idx % 9, y = idx / 9;
      idx++;
      switch( c ) {
      case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
        cells[y][x] = Cell( true, (int)(c-'0') );
        break;
      case 'x': case '.':
        cells[y][x] = Cell( true );
        break;
      default:
        idx--;
      }
    }
}

std::ostream&
Sudoku::dumpiteration( std::ostream& _sink, int _step, int _depth ) const
{
  while( _depth-- > 0 ) _sink << ' ';
  dump( (_sink << "=== step " << _step << " ===\n"), _depth ) << std::endl;
  return _sink;
};

unsigned
Sudoku::figcount() const
{
  unsigned count = 0;
  for (int y = 0; y < 9; y++)
    for (int x = 0; x < 9; x++)
      count += (cells[y][x].figure() != 0);
  return count;
}


void
Sudoku::play( std::ostream& logger, int _depth )
{
  Sudoku copy;
  int step = 0;
  unsigned last_figcount = 0;
  do
    {
      if (int delta = figcount() - last_figcount)
        {
          last_figcount += delta;
          dumpiteration( logger, step++, _depth );
        }

      copy = *this;

      // Direct-rule application
      for (int y = 0; y < 9; y++)
        {
          for (int x = 0; x < 9; x++)
            {
              if (not cells[y][x])
                throw PlayError();

              int fig = cells[y][x].figure();
        
              if (fig == 0)
                continue;
        
              Cell filter = Cell( false, fig );
              // applying row rules
              for (int xx = 0; xx < 9; xx++)
                {
                  if (xx == x)
                    continue;
                  cells[y][xx] &= filter;
                }

              // applying col rules
              for (int yy = 0; yy < 9; yy++)
                {
                  if (yy == y)
                    continue;
                  cells[yy][x] &= filter;
                }

              // applying block rules
              int xbase = (x/3)*3;
              int ybase = (y/3)*3;
              for (int idx = 0; idx < 9; idx++)
                {
                  int xx = xbase + (x % 3);
                  int yy = ybase + (y / 3);
                  if (xx == x and yy == y)
                    continue;
                  cells[yy][xx] &= filter;
                }
            }
        }
    
      // blocked col-row filtering rule (pseudo localization)
      for (int fig = 1; fig <= 9; fig++)
        {
          Cell figure( true, fig );
          Cell filter( false, fig );

          for (int meta = 0; meta < 9; meta++)
            {
              int ybase = (meta / 3)*3;
              int xbase = (meta % 3)*3;
              int col = -1;
              int row = -1;
              for (int idx = 0; idx < 9; idx++)
                {
                  int yy = ybase + (idx/3);
                  int xx = xbase + (idx%3);
                  if (cells[yy][xx] & figure)
                    {
                      if( col == -1 or col == xx )
                        col = xx;
                      else
                        col = -2;
                      if( row == -1 or row == yy )
                        row = yy;
                      else
                        row = -2;
                    }
                }

              if (col >= 0 and col < 9 and row < 0)
                {
                  // col lozalisation
                  for (int y = 0; y < 9; y++)
                    {
                      if( (meta/3) == (y/3) )
                        continue;
                      cells[y][col] = cells[y][col] & filter;
                    }
                }

              if (row >= 0 and row < 9 and col < 0)
                {
                  // row localization
                  for( int x = 0; x < 9; x++ ) {
                    if( (meta%3) == (x/3) )
                      continue;
                    cells[row][x] = cells[row][x] & filter;
                  }
                }
            }          
        }

      // unknown
      for (int fig = 1; fig <= 9; fig++)
        {
          Cell figure( true, fig );

          // applying row rules
          for (int y = 0; y < 9; y++)
            {
              int xx = -1;
              for (int x = 0; x < 9; x++)
                {
                  if (cells[y][x] & figure)
                    {
                      if (xx == -1)
                        xx = x;
                      else
                        xx = -2;
                    }
                }

              if (xx == -1)
                throw PlayError();

              if (xx >= 0 and xx < 9)
                cells[y][xx] = figure;
            }
      
          // applying col rules
          for (int x = 0; x < 9; x++)
            {
              int yy = -1;
              for (int y = 0; y < 9; y++)
                {
                  if (cells[y][x] & figure)
                    {
                      if (yy == -1)
                        yy = y;
                      else
                        yy = -2;
                    }
                }

              if( yy == -1 )
                throw PlayError();

              if( yy >= 0 and yy < 9 )
                cells[yy][x] = figure;
            }
      
          // applying block rules
          for (int meta = 0; meta < 9; meta++)
            {
              int ybase = (meta / 3)*3;
              int xbase = (meta % 3)*3;
              int idx2 = -1;
              for (int idx = 0; idx < 9; idx++)
                {
                  int yy = ybase + (idx/3);
                  int xx = xbase + (idx%3);
                  if (cells[yy][xx] & figure)
                    {
                      if (idx2 == -1)
                        idx2 = idx;
                      else
                        idx2 = -2;
                    }
                }

              if( idx2 == -1 )
                throw PlayError();

              if( idx2 >= 0 and idx2 < 9 ) {
                int yy = ybase + (idx2/3);
                int xx = xbase + (idx2%3);
                cells[yy][xx] = figure;
              }
            }          
        }
    } while( copy != *this );
  
  // Is the game finished ?
  for (int y = 0; y < 9; y++)
    {
      for (int x = 0; x < 9; x++)
        {
          if (cells[y][x].figure() != 0)
            continue;

          // Nope, need brute force exploration
          logger << "Trial and error\n";

          for (int fig = 1; fig <= 9; fig++)
            {
              Cell trial( true, fig );
              if (cells[y][x] & trial)
                {
                  copy = *this;
                  copy.cells[y][x] = trial;
                  try
                    {
                      copy.play(logger, _depth + 1);
                      *this = copy;
                      return;
                    }
                  catch( PlayError const& ) {}
                }
            }

          // Unfeasible
          throw PlayError();
        }
    }
}

void
Sudoku::save(std::ostream& sink) const
{
  dump( sink );
}

std::ostream&
Sudoku::dump(std::ostream& _sink, int _depth) const
{
  for( int y = 0; y < 9; y++ )
    {
      for (int depth = _depth; depth > 0; depth--) _sink << ' ';

      if (y == 3 or y == 6) _sink << '\n';

      for (int x = 0; x < 9; x++)
        {
          if (x == 3 or x == 6) _sink << ' ';
          int fig = cells[y][x].figure();
          if (fig >= 1 and fig <= 9) _sink << fig;
          else                       _sink << '.';
        }

      _sink << '\n';
    }

  return _sink;
}

Sudoku&
Sudoku::operator=( Sudoku const& _s ) {
  for( int y = 0; y < 9; y++ )
    for( int x = 0; x < 9; x++ )
      cells[y][x] = _s.cells[y][x];
  return *this;
}

bool
Sudoku::operator!=( Sudoku const& _s ) const {
  for( int y = 0; y < 9; y++ )
    for( int x = 0; x < 9; x++ )
      if( cells[y][x] != _s.cells[y][x] )
        return true;
  return false;
}
