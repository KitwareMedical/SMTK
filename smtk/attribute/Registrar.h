//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef __smtk_attribute_Registrar_h
#define __smtk_attribute_Registrar_h

#include "smtk/CoreExports.h"

#include "smtk/attribute/AssociationRuleManager.h"
#include "smtk/attribute/EvaluatorManager.h"
#include "smtk/common/Managers.h"
#include "smtk/operation/Manager.h"
#include "smtk/resource/Manager.h"

namespace smtk
{
namespace attribute
{
class SMTKCORE_EXPORT Registrar
{
public:
  static void registerTo(const smtk::common::Managers::Ptr&);
  static void unregisterFrom(const smtk::common::Managers::Ptr&);

  static void registerTo(const smtk::attribute::AssociationRuleManager::Ptr&);
  static void unregisterFrom(const smtk::attribute::AssociationRuleManager::Ptr&);

  static void registerTo(const smtk::operation::Manager::Ptr&);
  static void unregisterFrom(const smtk::operation::Manager::Ptr&);

  static void registerTo(const smtk::resource::Manager::Ptr&);
  static void unregisterFrom(const smtk::resource::Manager::Ptr&);

  static void registerTo(const smtk::attribute::EvaluatorManager::Ptr&);
  static void unregisterFrom(const smtk::attribute::EvaluatorManager::Ptr&);
};
}
}

#endif
