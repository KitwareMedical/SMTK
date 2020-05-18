//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#include "smtk/io/Logger.h"

#include "smtk/session/opencascade/Resource.h"

namespace smtk
{
namespace session
{
namespace opencascade
{

Resource::Resource(const smtk::common::UUID& uid, smtk::resource::ManagerPtr manager)
  : Superclass(uid, manager)
{
}

Resource::Resource(smtk::resource::ManagerPtr manager)
  : Superclass(manager)
{
}

void Resource::setSession(const Session::Ptr& session)
{
  m_session = session->shared_from_this();
}

} // namespace opencascade
} // namespace session
} // namespace smtk