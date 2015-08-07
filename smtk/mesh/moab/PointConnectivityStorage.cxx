//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#include "smtk/mesh/moab/PointConnectivityStorage.h"
#include "smtk/mesh/moab/CellTypeToType.h"
#include "smtk/mesh/moab/Interface.h"

#include "moab/Interface.hpp"

namespace smtk {
namespace mesh {
namespace moab {


//----------------------------------------------------------------------------
PointConnectivityStorage::PointConnectivityStorage(
                         const smtk::mesh::InterfacePtr& interface,
                         const smtk::mesh::HandleRange& cells):
  ConnectivityStartPositions(),
  ConnectivityArraysLengths(),
  ConnectivityVertsPerCell(),
  ConnectivityTypePerCell(),
  NumberOfCells(0),
  NumberOfVerts(0),
  VertConnectivityStorage()
{
  std::size_t cellCount = 0;
  std::size_t vertCount = 0;

  smtk::mesh::HandleRange::const_iterator cells_current = cells.begin();
  smtk::mesh::HandleRange::const_iterator cells_end = cells.end();

  ::moab::Interface* m_iface = smtk::mesh::moab::extract_moab_interface(interface);

  //We allocate VertConnectivityStorage once before we insert any vertices
  //this gaurentees that all of the ConnectivityStartPositions pointers
  //into our storage are valid. Otherwise the realloc's will cause
  //pointer invalidation
  const std::size_t numVerts = cells.num_of_type( ::moab::MBVERTEX );
  this->VertConnectivityStorage.reserve( numVerts );

  //first find all vertices. Vertices are a special case
  //where they are their own connectivity. Moab orders
  //the vertices first so, if cells_current isnt a vertice
  //we have none
  while(cells_current != cells_end &&
        m_iface->type_from_handle(*cells_current) == ::moab::MBVERTEX)
    {

    smtk::mesh::HandleRange::iterator verts_start = cells_current.start_of_block();
    smtk::mesh::HandleRange::iterator verts_end = cells_current.end_of_block();

    const int numVertsPerCell=1;
    const int numCellsInSubRange = std::distance(verts_start,verts_end+1);

    //add to the VertConnectivityStorage the ids of the vertices in this
    //range
    std::copy(verts_start,
              verts_end+1,
              std::back_inserter(this->VertConnectivityStorage));

    smtk::mesh::Handle* connectivity = &(this->VertConnectivityStorage[vertCount]);
    this->ConnectivityStartPositions.push_back(connectivity);
    this->ConnectivityArraysLengths.push_back(numCellsInSubRange);
    this->ConnectivityVertsPerCell.push_back(numVertsPerCell);

    //increment our iterator
    cells_current += static_cast<std::size_t>(numCellsInSubRange);

    //increment our num cells and verts counters
    cellCount += static_cast<std::size_t>(numCellsInSubRange);
    vertCount += static_cast<std::size_t>(numCellsInSubRange * numVertsPerCell);
    }

  while(cells_current != cells_end)
    {
    ::moab::EntityHandle* connectivity;
    int numVertsPerCell=0, numCellsInSubRange=0;

    ::moab::ErrorCode result =
    m_iface->connect_iterate(cells_current,
                             cells.end(),
                             connectivity,
                             numVertsPerCell,
                             numCellsInSubRange);

    if(result == ::moab::MB_SUCCESS)
      {
      this->ConnectivityStartPositions.push_back(connectivity);
      this->ConnectivityArraysLengths.push_back(numCellsInSubRange);
      this->ConnectivityVertsPerCell.push_back(numVertsPerCell);

      //fetch the current cell type for this block
      ::moab::EntityType moabCellType = m_iface->type_from_handle(*cells_current);
      this->ConnectivityTypePerCell.push_back(
        smtk::mesh::moab::moabToSMTKCell( static_cast< int >(moabCellType) ) );

      //increment our iterator
      cells_current += static_cast<std::size_t>(numCellsInSubRange);

      //increment our num cells and verts counters
      cellCount += static_cast<std::size_t>(numCellsInSubRange);
      vertCount += static_cast<std::size_t>(numCellsInSubRange * numVertsPerCell);
      }
    else
      {
      //we shouldn't presume all ids coming in have connectivity. In theory
      //if we are passed spectral elements we will get back no connectivity.
      //the same goes for vertices
  #ifndef NDEBUG
      std::cerr << "Passed range that contained non cell ids" << std::endl;
      std::cerr << "Handle ids " << *cells_current.start_of_block()
                                 << " to "
                                 << *cells_current.end_of_block()
                                 << "are not cell ids" << std::endl;
  #endif

      //increment our iterator to skip all these bad ids
      cells_current = cells_current.end_of_block();
      ++cells_current;
      }
    }

  this->NumberOfCells = cellCount;
  this->NumberOfVerts = vertCount;
}

//----------------------------------------------------------------------------
PointConnectivityStorage::PointConnectivityStorage(
                         const smtk::mesh::InterfacePtr& interface,
                         const smtk::mesh::Handle& cell):
  ConnectivityStartPositions(),
  ConnectivityArraysLengths(),
  ConnectivityVertsPerCell(),
  ConnectivityTypePerCell(),
  NumberOfCells(0),
  NumberOfVerts(0),
  VertConnectivityStorage()
{
  ::moab::Interface* m_iface = smtk::mesh::moab::extract_moab_interface(interface);

  const ::moab::EntityHandle* connectivity;
  int numVertsPerCell=0;
  const int numCellsInSubRange=1; //we are only passed a single cell

  //we have none
  ::moab::EntityType moabCellType = m_iface->type_from_handle(cell);
  if(moabCellType == ::moab::MBVERTEX)
    {
    //add to the VertConnectivityStorage the ids of the vertices in this
    //range
    this->VertConnectivityStorage.resize(1, cell);

    connectivity = &(this->VertConnectivityStorage[0]);
    numVertsPerCell = 1;
    }
  else
    {
    m_iface->get_connectivity(cell, connectivity, numVertsPerCell);
    }

  this->ConnectivityStartPositions.push_back(connectivity);
  this->ConnectivityArraysLengths.push_back(numCellsInSubRange);
  this->ConnectivityVertsPerCell.push_back(numVertsPerCell);

  this->ConnectivityTypePerCell.push_back(
    smtk::mesh::moab::moabToSMTKCell( static_cast< int >(moabCellType) ) );

  this->NumberOfCells = numCellsInSubRange;
  this->NumberOfVerts = numVertsPerCell;
}

//----------------------------------------------------------------------------
void PointConnectivityStorage::initTraversal(
                       smtk::mesh::PointConnectivity::IterationState& state )
{
  state.whichConnectivityVector = 0;
  state.ptrOffsetInVector = 0;
}

//----------------------------------------------------------------------------
bool PointConnectivityStorage::fetchNextCell(
                       smtk::mesh::PointConnectivity::IterationState& state,
                       smtk::mesh::CellType& cellType,
                       int& numPts,
                       const smtk::mesh::Handle* &points)
{
  if(state.whichConnectivityVector >= this->ConnectivityVertsPerCell.size())
    { //we have iterated passed the end of connectivity pointers
    return false;
    }

  const std::size_t index = state.whichConnectivityVector;
  const std::size_t ptr = state.ptrOffsetInVector;

  cellType = this->ConnectivityTypePerCell[ index ];
  numPts = this->ConnectivityVertsPerCell[ index ];
  points = &this->ConnectivityStartPositions[ index ][ptr];

  //now determine if we can safely increment
  const std::size_t currentArrayLength = this->ConnectivityArraysLengths[index] *
                                         this->ConnectivityVertsPerCell[index];

  //check the last position we are accessing with this fetch
  //to properly determine this is the last cell that is valid
  if( ptr + numPts >= currentArrayLength)
    {
    //move to the next vector
    ++state.whichConnectivityVector;
    state.ptrOffsetInVector = 0;
    }
  else
    {
    state.ptrOffsetInVector += numPts;
    }
  return true;
}

//----------------------------------------------------------------------------
bool PointConnectivityStorage::equal( PointConnectivityStorage* other ) const
{
  if( this == other ) { return true;}

  //two quick checks that can confirm two items aren't equal
  if( this->NumberOfCells != other->NumberOfCells )
    { return false; }

  if( this->ConnectivityStartPositions.size() !=
      other->ConnectivityStartPositions.size() )
    { return false; }

  //now we know that both sets have the same number of moab pointers
  //and the total number of cells are the same. So we just have too
  //iterate the handles and the array lengths and confirm they all match.
  //in theory you could have equal start positions but different lengths
  //that both sum up to the same total num cells.

  typedef std::vector< const ::moab::EntityHandle* >::const_iterator hc_it;
  typedef std::vector< int >::const_iterator ic_it;

  const hc_it this_end = this->ConnectivityStartPositions.end();

  hc_it this_ptr = this->ConnectivityStartPositions.begin();
  hc_it other_ptr = other->ConnectivityStartPositions.begin();

  ic_it this_len = this->ConnectivityArraysLengths.begin();
  ic_it other_len = other->ConnectivityArraysLengths.begin();

  while( this_ptr != this_end &&
         *this_ptr == *other_ptr &&
         *this_len == *other_len)
    {
    ++this_ptr;
    ++other_ptr;
    ++this_len;
    ++other_len;
    }

  //if we are at the end that means the collections match
  return this_ptr == this_end;
}

}
}
}
