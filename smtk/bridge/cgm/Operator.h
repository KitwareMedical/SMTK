//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef __smtk_session_cgm_Operator_h
#define __smtk_session_cgm_Operator_h

#include "smtk/bridge/cgm/cgmSMTKExports.h"
#include "smtk/model/Operator.h"

class RefEntity;
class ToolDataUser;

namespace smtk {
  namespace bridge {
    namespace cgm {

class Session;

/**\brief An operator using the CGM kernel.
  *
  * This is a base class for actual CGM operators.
  * It provides convenience methods for accessing CGM-specific data
  * for its subclasses to use internally.
  */
class CGMSMTK_EXPORT Operator : public smtk::model::Operator
{
protected:
  Session* cgmSession();
  ToolDataUser* cgmData(const smtk::model::EntityRef& smtkEntity);
  RefEntity* cgmEntity(const smtk::model::EntityRef& smtkEntity);

  template<typename T>
  T cgmEntityAs(const smtk::model::EntityRef& smtkEntity)
    { return dynamic_cast<T>(this->cgmEntity(smtkEntity)); }
};

    } // namespace cgm
  } // namespace bridge
} // namespace smtk

#endif // __smtk_session_cgm_Operator_h
