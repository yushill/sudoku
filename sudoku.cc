/***************************************************************************
                                 sudoku.cc
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
#include <istream>
#include <ostream>
#include <set>

Sudoku::Combinations Sudoku::combinations;

Sudoku::Sudoku()
{
  // Starting with all cells marked as all figures possible
  for (int y = 0; y < 9; ++y)
    for (int x = 0; x < 9; ++x)
      cells[y][x] = ~Cell();
}

Sudoku::~Sudoku() {}

void
Sudoku::open( std::istream& source )
{
  int idx = 0;
  char c;

  while (source.get( c ))
    {
      int x = idx % 9, y = idx / 9;
      idx++;
      switch( c ) {
      case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
        cells[y][x] = Cell( Cell::Figure{c-'0'} );
        break;
      case 'x': case '.':
        cells[y][x] = ~Cell();
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

bool
Sudoku::rulestep(std::ostream& logger)
{
  for (auto const& combo: combinations)
    {
      Cell ncombo = ~combo;
      for (Zone::Code zone = Zone::beg; zone < Zone::end; zone = Zone::Code(int(zone + 1)))
        for (int idx = 0; idx < 9; ++idx)
          {
            struct
            {
              bool process(std::ostream& logger, Sudoku& sudoku, Zone::Code zone, int idx, unsigned loc, Cell const& combo)
              {
                unsigned popcount = combo.popcount();
                if (int delta = __builtin_popcount(loc) - popcount)
                  { if (delta < 0) throw PlayError(); else return false; }
                bool moving = false;
                for (int offset = 0; offset < 9; ++offset)
                  moving |= ((loc >> offset) & 1 and sudoku.getcell(zone, idx, offset).keep(combo));
                if (moving)
                  {
                    bool number_exclusion = popcount < 5;
                    unsigned force = number_exclusion ? popcount : 9 - popcount;
                    if (force > 1)
                      {
                        logger << "In " << Zone{zone} << " #" << (idx+1) << ", ";
                        if (number_exclusion)
                          {
                            Cell positions( Cell::Figures{loc << 1} );
                            logger << "positions " << positions.setfmt() << " can only host " << combo.setfmt() << "\n";
                          }
                        else
                          {
                            Cell positions( Cell::Figures{~(loc << 1)} ), ncombo( ~combo);
                            logger << "figures " << ncombo.setfmt() << " can only be at " << positions.setfmt() << "\n";
                          }
                      }
                  }
                return moving;
              }
            } _;

            unsigned exloc = 0, inloc = 0;
            for (int offset = 0; offset < 9; ++offset)
              {
                Cell& cell = getcell(zone, idx, offset);
                if (cell &  combo) inloc |= (1 << offset);
                if (cell & ncombo) exloc |= (1 << offset);
              }

            if (_.process(logger, *this, zone, idx, inloc,  combo))
              return true;

            if (_.process(logger, *this, zone, idx, exloc, ncombo))
              return true;
          }
    }

  for (int ridx = 0; ridx < 9; ++ridx)
    {
      bool moving = false;
      for (int idx = 0; idx < 3; ++idx)
        {
          // Column
          int rx = ridx % 3, ry = ridx / 3;
          Cell rcfilter, crfilter, rlfilter, lrfilter;
          for (int offset = 0; offset < 9; ++offset)
            {
              // Missing from column outside region
              if (offset/3 != ry)
                rcfilter |= getcell(Zone::Column, rx*3+idx, offset);
              // Missing from region outside column
              if (offset%3 != idx)
                crfilter |= getcell(Zone::Region, ridx,     offset);
              // Missing from row outside region
              if (offset/3 != rx)
                rlfilter |= getcell(Zone::Row,    ry*3+idx, offset);
              // Missing from region outside row
              if (offset/3 != idx)
                lrfilter |= getcell(Zone::Region, ridx,     offset);
            }
          Cell rccleared, crcleared, rlcleared, lrcleared;
          for (int offset = 0; offset < 9; ++offset)
            {
              if (offset%3 != idx)
                moving |= rcfilter.filter(getcell(Zone::Region, ridx,     offset), rccleared);
              if (offset/3 != ry)
                moving |= crfilter.filter(getcell(Zone::Column, rx*3+idx, offset), crcleared);
              if (offset/3 != idx)
                moving |= rlfilter.filter(getcell(Zone::Region, ridx,     offset), rlcleared);
              if (offset/3 != rx)
                moving |= lrfilter.filter(getcell(Zone::Row,    ry*3+idx, offset), lrcleared);
            }

          if (rccleared.figures())
            logger << "In region #" << (ridx+1) << ", figures " << rccleared.setfmt() << " can only be from column #" << (rx*3+idx+1) << "\n";
          if (crcleared.figures())
            logger << "In column #" << (rx*3+idx+1) << ", figures " << crcleared.setfmt() << " can only be from region #" << (ridx+1) << "\n";
          if (rlcleared.figures())
            logger << "In region #" << (ridx+1) << ", figures " << rlcleared.setfmt() << " can only be from row #" << (ry*3+idx+1) << "\n";
          if (lrcleared.figures())
            logger << "In row #" << (ry*3+idx+1) << ", figures " << lrcleared.setfmt() << " can only be from region #" << (ridx+1) << "\n";
        }
      if (moving)
        return true;
    }

  return false;
}

void
Sudoku::play( std::ostream& logger, unsigned last_figcount, int _depth )
{
  int step = 0;

  while (rulestep(logger))
    {
      if (int delta = figcount() - last_figcount)
        {
          last_figcount += delta;
          dumpiteration( logger, step++, _depth );
        }
    }

  // Is the game finished ?
  for (int y = 0; y < 9; y++)
    {
      for (int x = 0; x < 9; x++)
        {
          if (cells[y][x].figure() != 0)
            continue;

          // Nope, try brute force resolution
          logger << "Trial and error\n";

          for (int fig = 1; fig <= 9; fig++)
            {
              Cell trial( Cell::Figure{fig} );
              if (cells[y][x] & trial)
                {
                  Sudoku copy( *this );
                  copy.cells[y][x] = trial;
                  try
                    {
                      copy.play(logger, last_figcount, _depth + 1);
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

std::ostream&
Sudoku::Zone::print(std::ostream& sink) const
{
  sink << name();
  return sink;
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
Sudoku::operator=( Sudoku const& _s )
{
  for (int y = 0; y < 9; y++)
    for (int x = 0; x < 9; x++)
      cells[y][x] = _s.cells[y][x];
  return *this;
}

bool
Sudoku::operator!=( Sudoku const& _s ) const
{
  for (int y = 0; y < 9; y++)
    for (int x = 0; x < 9; x++)
      if (cells[y][x] != _s.cells[y][x])
        return true;
  return false;
}

Cell&
Sudoku::getcell( Zone::Code code, int index, int offset )
{
  switch (code)
    {
    default: break;
    case Zone::Column:
      return cells[offset][index];
    case Zone::Region:
      return cells[3*(index/3)+(offset/3)][3*(index%3)+(offset%3)];
    }
  // Zone::Row
  return cells[index][offset];
}



Sudoku::Combinations::Combinations()
{
  struct CellCmp { bool operator () ( Cell const& lhs, Cell const& rhs ) const {
    if (int delta = lhs.popcount() - rhs.popcount())
      return delta < 0;
    return lhs.figures() < rhs.figures();
  } };

  std::set<Cell, CellCmp> build;
  for (unsigned idx = 0; idx < count; ++idx)
    build.insert( Cell( Cell::Figures{idx<<1} ));
  std::copy(build.begin(), build.end(), &items[0] );
}
