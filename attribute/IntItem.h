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
// .NAME IntItem.h -
// .SECTION Description
// .SECTION See Also

#ifndef __slctk_attribute_IntItem_h
#define __slctk_attribute_IntItem_h

#include "attribute/ValueItemTemplate.h"
#include "AttributeExports.h"

namespace slctk
{
  namespace attribute
  {
    class Attribute;
    class IntItemDefinition;
    class SLCTKATTRIBUTE_EXPORT IntItem :
      public ValueItemTemplate<int>
    {
      friend class IntItemDefinition; 
    public:
      IntItem(Attribute *owningAttribute, int itemPosition);
      IntItem(Item *owningItem, int myPosition, int mySubGroupPosition);
      virtual ~IntItem();
      virtual Item::Type type() const;
    protected:
      
    private:

    };
  };
};

#endif /* __slctk_attribute_IntItem_h */
