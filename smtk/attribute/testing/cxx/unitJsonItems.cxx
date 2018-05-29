//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#include "smtk/PublicPointerDefs.h"

#include "smtk/operation/operators/ReadResource.h"

#include "smtk/model/Manager.h"

#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/Collection.h"
#include "smtk/attribute/Definition.h"
#include "smtk/attribute/DirectoryItemDefinition.h"
#include "smtk/attribute/DoubleItem.h"
#include "smtk/attribute/DoubleItemDefinition.h"
#include "smtk/attribute/FileItem.h"
#include "smtk/attribute/FileItemDefinition.h"
#include "smtk/attribute/GroupItemDefinition.h"
#include "smtk/attribute/IntItem.h"
#include "smtk/attribute/IntItemDefinition.h"
#include "smtk/attribute/Item.h"
#include "smtk/attribute/RefItemDefinition.h"
#include "smtk/attribute/ReferenceItem.h"
#include "smtk/attribute/ReferenceItemDefinition.h"
#include "smtk/attribute/Registrar.h"
#include "smtk/attribute/StringItem.h"
#include "smtk/attribute/StringItemDefinition.h"
#include "smtk/attribute/VoidItemDefinition.h"

#include "smtk/common/Registry.h"

#include "smtk/io/Logger.h"

#include "smtk/operation/Manager.h"
#include "smtk/resource/Manager.h"

#include "smtk/attribute/json/jsonDirectoryItem.h"
#include "smtk/attribute/json/jsonItem.h"
#include "smtk/attribute/json/jsonReferenceItem.h"

#include "smtk/common/testing/cxx/helpers.h"

#include "nlohmann/json.hpp"

using namespace smtk::attribute;
using json = nlohmann::json;

static std::vector<char*> dataArgs;

int unitJsonItems(int argc, char* argv[])
{
  // Load in a model file so we can associate model entities to test attributes.
  if (argc < 2)
  {
    std::string testFile;
    testFile = SMTK_DATA_DIR;
    testFile += "/model/2d/smtk/epic-trex-drummer.smtk";
    dataArgs.push_back(strdup(argv[0]));
    dataArgs.push_back(strdup(testFile.c_str()));
    dataArgs.push_back(nullptr);
    argc = 2;
    argv = &dataArgs[0];
  }
  auto rsrcMgr = smtk::resource::Manager::create();
  auto operMgr = smtk::operation::Manager::create();

  auto registry = smtk::common::Registry<smtk::attribute::Registrar, smtk::resource::Manager,
    smtk::operation::Manager>(rsrcMgr, operMgr);

  smtk::resource::ResourceArray rsrcs;
  for (int i = 1; i < argc; i++)
  {
    auto rdr = operMgr->create<smtk::operation::ReadResource>();
    rdr->parameters()->findFile("filename")->setValue(argv[i]);
    rdr->operate();
  }

  // Reference childrenItemsTest
  smtk::attribute::CollectionPtr sysptr = rsrcMgr->create<smtk::attribute::Collection>();
  smtk::attribute::Collection& collection(*sysptr.get());
  std::cout << "Collection Created\n";

  // Lets create an attribute to represent an expression
  smtk::attribute::DefinitionPtr expDef = collection.createDefinition("ExpDef");
  expDef->setBriefDescription("Sample Expression");
  expDef->setDetailedDescription("Sample Expression for testing\nThere is not much here!");
  // StringItemDefinition
  smtk::attribute::StringItemDefinitionPtr eitemdef =
    expDef->addItemDefinition<smtk::attribute::StringItemDefinitionPtr>("Expression String");
  eitemdef->setDefaultValue("sample");
  eitemdef->setIsOptional(true);
  smtk::attribute::StringItemDefinitionPtr eitemdef2 =
    expDef->addItemDefinition<smtk::attribute::StringItemDefinition>("Aux String");
  eitemdef2->setDefaultValue("sample2");
  // DirectoryItemDefinition
  smtk::attribute::DirectoryItemDefinitionPtr dirDef =
    expDef->addItemDefinition<smtk::attribute::DirectoryItemDefinition>("Directory Item");
  dirDef->setIsOptional(true);
  // ReferenceItemDefinition
  auto refDef = expDef->addItemDefinition<smtk::attribute::ReferenceItemDefinition>("Faces");
  refDef->setIsOptional(true);
  refDef->setIsExtensible(true);
  refDef->setNumberOfRequiredValues(0);
  refDef->setMaxNumberOfValues(0);
  refDef->setAcceptsEntries("smtk::model::Manager", "face", true);

  smtk::attribute::DefinitionPtr base = collection.createDefinition("BaseDef");
  // Lets add some item definitions
  smtk::attribute::IntItemDefinitionPtr iitemdef =
    base->addItemDefinition<smtk::attribute::IntItemDefinitionPtr>("TEMPORAL");
  iitemdef->setLabel("Time");
  iitemdef->addDiscreteValue(0, "Seconds");
  iitemdef->addDiscreteValue(1, "Minutes");
  iitemdef->addDiscreteValue(2, "Hours");
  iitemdef->addDiscreteValue(3, "Days");
  iitemdef->setDefaultDiscreteIndex(0);
  iitemdef->addCategory("Time");
  iitemdef = base->addItemDefinition<smtk::attribute::IntItemDefinitionPtr>("IntItem2");
  iitemdef->setDefaultValue(10);
  iitemdef->addCategory("Heat");

  // Lets test creating an attribute by passing in the expression definition explicitly
  smtk::attribute::AttributePtr expAtt1 = collection.createAttribute("Exp1", expDef);

  /**********************       item          ********************/
  smtk::attribute::ItemPtr expressionString1 = expAtt1->find("Expression String");
  json jsonExpSToJ = expressionString1;
  expressionString1->setIsEnabled(true);
  json jsonExpSToJFalse = expressionString1;
  smtk::attribute::from_json(jsonExpSToJ, expressionString1);
  json jsonExpSFromJ = expressionString1;
  // After deserialization, json should match the orignal state and differ with the changed state
  test(jsonExpSFromJ == jsonExpSToJ, "Failed to serialize and deserialize "
                                     "item");
  test(jsonExpSFromJ != jsonExpSToJFalse, "Failed to serialize and deserialize "
                                          "item");

  // TODO: Add unit test for other items
  /**********************       directoryItem          ********************/
  smtk::attribute::DirectoryItemPtr dir =
    smtk::dynamic_pointer_cast<smtk::attribute::DirectoryItem>(expAtt1->find("Directory Item"));
  json DirToJ = dir;
  dir->setIsEnabled(true);
  json DirFromJ = dir;
  smtk::attribute::from_json(DirFromJ, dir);
  json DirToJModified = dir;
  // After deserialization, json should match the orignal state and differ with the changed state
  test(DirFromJ == DirToJModified, "Failed to serialize and deserialize "
                                   "DirectoryItem");
  test(DirToJ != DirToJModified, "Failed to serialize and deserialize "
                                 "DirectoryItem");

  /**********************       referenceItem          ********************/
  auto refItm = smtk::dynamic_pointer_cast<smtk::attribute::ReferenceItem>(expAtt1->find("Faces"));
  json jsonRefItm1 = refItm;
  refItm->setIsEnabled(true);
  refItm->setNumberOfValues(2);
  auto basicRsrc = rsrcMgr->get(argv[1]);
  auto modelRsrc = std::dynamic_pointer_cast<smtk::model::Manager>(basicRsrc);
  auto allFaces =
    modelRsrc->entitiesMatchingFlagsAs<smtk::model::EntityRefArray>(smtk::model::FACE);
  std::cout << "Model " << modelRsrc->id().toString() << " has " << allFaces.size() << " faces\n";
  for (std::size_t i = 0; i < refItm->numberOfValues(); ++i)
  {
    refItm->setObjectValue(i, allFaces[i].component());
  }
  json jsonRefItm2 = refItm;
  smtk::attribute::from_json(jsonRefItm2, refItm);
  json jsonRefItm3 = refItm;
  std::cout << "\n\nBefore\n"
            << jsonRefItm1.dump(2) << "\n\nAfter\n"
            << jsonRefItm2.dump(2) << "\n\n";
  // After deserialization, json should match the orignal state and differ with the changed state
  test(jsonRefItm3 == jsonRefItm2, "Failed to serialize and deserialize ReferenceItem");
  test(jsonRefItm1 != jsonRefItm2, "Failed to serialize and deserialize ReferenceItem");

  while (!dataArgs.empty())
  {
    char* val = dataArgs.back();
    if (val)
    {
      free(val);
    }
    dataArgs.pop_back();
  }
  return 0;
}