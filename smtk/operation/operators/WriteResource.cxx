//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#include "smtk/operation/operators/WriteResource.h"

#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/Definition.h"
#include "smtk/attribute/FileItem.h"
#include "smtk/attribute/IntItem.h"
#include "smtk/attribute/ResourceItem.h"

#include "smtk/io/Logger.h"

#include "smtk/resource/Manager.h"
#include "smtk/resource/Metadata.h"

#include "smtk/operation/Group.h"
#include "smtk/operation/WriteResource_xml.h"
#include "smtk/operation/groups/WriterGroup.h"

#include "nlohmann/json.hpp"

#include <fstream>

using json = nlohmann::json;

namespace smtk
{
namespace operation
{

WriteResource::WriteResource()
{
}

bool WriteResource::ableToOperate()
{
  if (!this->Superclass::ableToOperate())
  {
    return false;
  }

  // To write a resource, we must have an operation manager from which we access
  // specific write operations.
  if (m_manager.expired())
  {
    return false;
  }

  return true;
}

smtk::operation::Operation::Result WriteResource::operateInternal()
{
  auto manager = m_manager.lock();

  auto params = this->parameters();
  auto fileItem = params->findFile("filename");
  auto setFileName = fileItem->isEnabled();
  auto resourceItem = params->findResource("resource");

  if (resourceItem->numberOfValues() < 1)
  {
    smtkErrorMacro(this->log(), "At least one resource must be selected for saving.");
    return this->createResult(smtk::operation::Operation::Outcome::FAILED);
  }

  if (setFileName && resourceItem->numberOfValues() != fileItem->numberOfValues())
  {
    smtkErrorMacro(this->log(), "Number of filenames must match number of resources.");
    return this->createResult(smtk::operation::Operation::Outcome::FAILED);
  }

  smtk::operation::WriterGroup writerGroup(manager);

  int rr = 0;
  for (auto rit = resourceItem->begin(); rit != resourceItem->end(); ++rit, ++rr)
  {
    auto resource = std::dynamic_pointer_cast<smtk::resource::Resource>(*rit);

    smtk::operation::Operation::Ptr writeOperation =
      writerGroup.writerForResource(resource->typeName());

    if (writeOperation == nullptr)
    {
      smtkErrorMacro(
        this->log(), "Could not find writer for type = " << resource->typeName() << ".");
      return this->createResult(smtk::operation::Operation::Outcome::FAILED);
    }

    auto fileName = setFileName ? fileItem->value(rr) : resource->location();

    if (fileName.empty())
    {
      smtkErrorMacro(this->log(), "An empty file name is not allowed.");
      return this->createResult(smtk::operation::Operation::Outcome::FAILED);
    }

    // Set the local writer's filename field, if there is one.
    smtk::attribute::FileItem::Ptr writerFileItem = writeOperation->parameters()->findFile(
      writerGroup.fileItemNameForOperation(writeOperation->index()));

    // A writer may not accept a filename input. If this is the case and a file
    // name is requested by the user, warn and continue.
    if (!writerFileItem && setFileName)
    {
      smtkWarningMacro(this->log(), "File name \""
          << fileName << "\" was provided, but the registered writer for type \""
          << resource->typeName() << "\" does not accept an input file item.");
    }

    // If the writer does accept a filename input, set it.
    if (writerFileItem)
    {
      writerFileItem->setValue(fileName);
    }

    writeOperation->parameters()->findResource("resource")->setValue(resource);

    smtk::operation::Operation::Result writeOperationResult = writeOperation->operate();
    if (writeOperationResult->findInt("outcome")->value() !=
      static_cast<int>(smtk::operation::Operation::Outcome::SUCCEEDED))
    {
      // An error message should already enter the logger from the local
      // operation.
      return this->createResult(smtk::operation::Operation::Outcome::FAILED);
    }

    if (setFileName)
    {
      resource->setLocation(fileName);
    }
  }
  return this->createResult(smtk::operation::Operation::Outcome::SUCCEEDED);
}

const char* WriteResource::xmlDescription() const
{
  return WriteResource_xml;
}

void WriteResource::generateSummary(WriteResource::Result& res)
{
  std::ostringstream msg;
  int outcome = res->findInt("outcome")->value();
  auto resourceItem = this->parameters()->findResource("resource");
  msg << this->parameters()->definition()->label();
  if (outcome == static_cast<int>(smtk::operation::Operation::Outcome::SUCCEEDED))
  {
    msg << ": wrote \"" << resourceItem->value(0)->location() << "\"";
    smtkInfoMacro(this->log(), msg.str());
  }
  else
  {
    msg << ": failed to write \"" << resourceItem->value(0)->location() << "\"";
    smtkErrorMacro(this->log(), msg.str());
  }
}
}
}