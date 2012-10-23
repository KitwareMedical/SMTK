/*=========================================================================

Copyright (c) 1998-2012 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved. No part of this software may be reproduced, distributed,
or modified, in any form or by any means, without permission in writing from
Kitware Inc.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO
PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================*/


#include "attribute/VoidItemDefinition.h"
#include "attribute/Attribute.h"
#include "attribute/VoidItem.h"

using namespace slctk::attribute;

//----------------------------------------------------------------------------
VoidItemDefinition::
VoidItemDefinition(const std::string &myName):
  ItemDefinition(myName)
{
}

//----------------------------------------------------------------------------
VoidItemDefinition::~VoidItemDefinition()
{
}
//----------------------------------------------------------------------------
slctk::AttributeItemPtr 
VoidItemDefinition::buildItem(Attribute *owningAttribute,
                              int itemPosition) const
{
  return slctk::AttributeItemPtr(new VoidItem(owningAttribute,
                                              itemPosition));
}
//----------------------------------------------------------------------------
slctk::AttributeItemPtr 
VoidItemDefinition::buildItem(Item *owningItem,
                              int itemPosition,
                              int subGroupPosition) const
{
  return slctk::AttributeItemPtr(new VoidItem(owningItem,
                                              itemPosition,
                                              subGroupPosition));
}
//----------------------------------------------------------------------------
Item::Type VoidItemDefinition::type() const
{
  return Item::VOID;
}
//----------------------------------------------------------------------------
