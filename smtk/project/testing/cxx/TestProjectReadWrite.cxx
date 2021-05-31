//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/FileItem.h"
#include "smtk/attribute/IntItem.h"
#include "smtk/attribute/ResourceItem.h"

#include "smtk/common/UUIDGenerator.h"
#include "smtk/resource/Component.h"
#include "smtk/resource/DerivedFrom.h"
#include "smtk/resource/Manager.h"

#include "smtk/mesh/core/Resource.h"
#include "smtk/mesh/resource/Registrar.h"

#include "smtk/operation/Manager.h"
#include "smtk/operation/Registrar.h"
#include "smtk/operation/operators/ImportResource.h"
#include "smtk/operation/operators/ReadResource.h"
#include "smtk/operation/operators/WriteResource.h"

#include "smtk/project/Manager.h"
#include "smtk/project/Project.h"
#include "smtk/project/Registrar.h"
#include "smtk/project/operators/Define.h"

#include "smtk/common/testing/cxx/helpers.h"

//force to use filesystem version 3
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

// This test verifies that projects can be serialized to the file system
// and unserialized back. The test project contains an SMTK mesh resource.

namespace
{

//SMTK_DATA_DIR is a define setup by cmake
std::string data_root = SMTK_DATA_DIR;
std::string write_root = SMTK_SCRATCH_DIR;

void cleanup(const std::string& file_path)
{
  //first verify the file exists
  ::boost::filesystem::path path(file_path);
  if (::boost::filesystem::is_regular_file(path))
  {
    //remove the file_path if it exists.
    ::boost::filesystem::remove(path);
  }
}
} // namespace

int TestProjectReadWrite(int /*unused*/, char** const /*unused*/)
{
  // Create a resource manager
  smtk::resource::Manager::Ptr resourceManager = smtk::resource::Manager::create();

  {
    smtk::mesh::Registrar::registerTo(resourceManager);
    smtk::project::Registrar::registerTo(resourceManager);
  }

  // Create an operation manager
  smtk::operation::Manager::Ptr operationManager = smtk::operation::Manager::create();

  {
    smtk::mesh::Registrar::registerTo(operationManager);
    smtk::operation::Registrar::registerTo(operationManager);
  }

  // Register the resource manager to the operation manager (newly created
  // resources will be automatically registered to the resource manager).
  operationManager->registerResourceManager(resourceManager);

  // Create a project manager
  smtk::project::ManagerPtr projectManager =
    smtk::project::Manager::create(resourceManager, operationManager);

  {
    smtk::project::Registrar::registerTo(projectManager);
  }

  // Register a new project type
  projectManager->registerProject("foo");

  // Create a project and write it to disk.
  std::string projectLocation;
  std::size_t numberOfMeshes;
  {
    smtk::project::Project::Ptr project = projectManager->create("foo");
    if (!project)
    {
      std::cerr << "Failed to create a project\n";
      return 1;
    }

    {
      // Create an import operator
      smtk::operation::ImportResource::Ptr importOp =
        operationManager->create<smtk::operation::ImportResource>();
      if (!importOp)
      {
        std::cerr << "No import operator\n";
        return 1;
      }

      // Set the file path
      std::string importFilePath(data_root);
      importFilePath += "/model/3d/exodus/SimpleReactorCore/SimpleReactorCore.exo";
      importOp->parameters()->findFile("filename")->setValue(importFilePath);

      // Execute the operation
      smtk::operation::Operation::Result importOpResult = importOp->operate();

      // Test for success
      if (
        importOpResult->findInt("outcome")->value() !=
        static_cast<int>(smtk::operation::Operation::Outcome::SUCCEEDED))
      {
        std::cerr << "Import operation failed\n";
        return 1;
      }

      // Add the mesh resource to the project
      smtk::attribute::ResourceItemPtr resourceItem =
        std::dynamic_pointer_cast<smtk::attribute::ResourceItem>(
          importOpResult->findResource("resource"));

      project->resources().add(
        std::dynamic_pointer_cast<smtk::mesh::Resource>(resourceItem->value()), "my mesh");
    }

    if (project->resources().size() != 1)
    {
      std::cerr << "Failed to add a resource to the project\n";
      return 1;
    }

    {
      smtk::mesh::Resource::Ptr myMesh =
        project->resources().getByRole<smtk::mesh::Resource>("my mesh");
      numberOfMeshes = myMesh->meshes().size();
    }

    {
      // Create a write operator
      smtk::operation::WriteResource::Ptr writeOp =
        operationManager->create<smtk::operation::WriteResource>();

      if (!writeOp)
      {
        std::cerr << "No write operator\n";
        return 1;
      }

      // Set the file path
      projectLocation = write_root + "/" + smtk::common::UUID::random().toString() + ".smtk";

      writeOp->parameters()->associate(project);
      writeOp->parameters()->findFile("filename")->setIsEnabled(true);
      writeOp->parameters()->findFile("filename")->setValue(projectLocation);

      if (!writeOp->ableToOperate())
      {
        std::cerr << "Write operation unable to operate\n";
        return 1;
      }

      // Execute the operation
      smtk::operation::Operation::Result writeOpResult = writeOp->operate();

      // Test for success
      if (
        writeOpResult->findInt("outcome")->value() !=
        static_cast<int>(smtk::operation::Operation::Outcome::SUCCEEDED))
      {
        std::cerr << "Write operation failed\n";
        std::cerr << writeOp->log().convertToString();
        return 1;
      }
    }

    projectManager->remove(project);
  }

  // Read the project
  smtk::project::Project::Ptr project;
  {
    // Create a read operator
    smtk::operation::ReadResource::Ptr readOp =
      operationManager->create<smtk::operation::ReadResource>();

    if (!readOp)
    {
      std::cerr << "No read operator\n";
      return 1;
    }

    readOp->parameters()->findFile("filename")->setValue(projectLocation);

    if (!readOp->ableToOperate())
    {
      std::cerr << "Read operation unable to operate\n";
      return 1;
    }

    // Execute the operation
    smtk::operation::Operation::Result readOpResult = readOp->operate();

    // Test for success
    if (
      readOpResult->findInt("outcome")->value() !=
      static_cast<int>(smtk::operation::Operation::Outcome::SUCCEEDED))
    {
      std::cerr << "Read operation failed\n";
      std::cerr << readOp->log().convertToString();
      return 1;
    }

    project = readOpResult->findResource("resource")->valueAs<smtk::project::Project>();

    if (!project)
    {
      std::cerr << "Resulting project is invalid\n";
      return 1;
    }

    if (!project->clean())
    {
      std::cerr << "Resulting project is marked modified\n";
      return 1;
    }
  }

  smtk::mesh::Resource::Ptr myMesh =
    project->resources().getByRole<smtk::mesh::Resource>("my mesh");

  if (!myMesh)
  {
    std::cerr << "Resulting project does not contain mesh resource\n";
    return 1;
  }

  if (myMesh->meshes().size() != numberOfMeshes)
  {
    std::cerr << "Resulting project's mesh resource was incorrectly transcribed\n";
    return 1;
  }

  // Fail (and not crash) when project type isn't registered
  projectManager->unregisterProject("foo");
  {
    smtk::operation::ReadResource::Ptr readOp =
      operationManager->create<smtk::operation::ReadResource>();
    readOp->parameters()->findFile("filename")->setValue(projectLocation);
    smtk::operation::Operation::Result readOpResult = readOp->operate();
    if (
      readOpResult->findInt("outcome")->value() !=
      static_cast<int>(smtk::operation::Operation::Outcome::FAILED))
    {
      std::cerr << "Read operation should have failed\n";
      return 1;
    }
    std::cout << readOp->log().convertToString();
  }

  cleanup(projectLocation);

  return 0;
}