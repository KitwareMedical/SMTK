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
// .NAME AttributeReferenceComponent.h -
// .SECTION Description
// .SECTION See Also

#ifndef __slctk_attribute_AttributeReferenceComponent_h
#define __slctk_attribute_AttributeReferenceComponent_h

#include "AttributeExports.h"
#include "attribute/ValueComponent.h"
#include <vector>

namespace slctk
{
  namespace attribute
  {
    class Attribute;
    class AttributeReferenceComponentDefinition;
    class SLCTKATTRIBUTE_EXPORT AttributeReferenceComponent : public ValueComponent
    {
    public:
      AttributeReferenceComponent(const AttributeReferenceComponentDefinition *def);
      virtual ~AttributeReferenceComponent();
      virtual Component::Type type() const;
      slctk::attribute::Attribute *value() const
      {return this->m_values[0];}
      slctk::attribute::Attribute *value(int element) const
      {return this->m_values[element];}
      bool setValue( slctk::attribute::Attribute *val)
      {return this->setValue(0, val);}
      bool setValue(int element, slctk::attribute::Attribute *val);
      virtual bool setToDefault(int element) const;
      bool appendValue(slctk::attribute::Attribute *val);
      bool removeValue(int element);
      virtual void reset();
      virtual bool setToDefault(int element);
      virtual const std::string &valueAsString(int element, const std::string &format) const;
     
    protected:
      virtual void updateDiscreteValue(int element);
      std::vector<Attribute *>m_values;
    private:
    };
  };
};


#endif /* __slctk_attribute_AttributeReferenceComponent_h */
