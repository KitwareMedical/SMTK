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

#include "smtk/attribute/Manager.h"
#include "smtk/attribute/Definition.h"
#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/RefItemDefinition.h"
#include "smtk/attribute/IntItem.h"
#include "smtk/attribute/IntItemDefinition.h"
#include "smtk/attribute/DoubleItem.h"
#include "smtk/attribute/DoubleItemDefinition.h"
#include "smtk/attribute/DirectoryItemDefinition.h"
#include "smtk/attribute/FileItemDefinition.h"
#include "smtk/attribute/GroupItemDefinition.h"
#include "smtk/attribute/StringItem.h"
#include "smtk/attribute/StringItemDefinition.h"
#include "smtk/attribute/VoidItemDefinition.h"
#include "smtk/model/Item.h" // just needed for enum
#include "smtk/util/AttributeWriter.h"
#include "smtk/util/Logger.h"

#include <iostream>

int main(int argc, char *argv[])
{
  int status = 0;
  {
  if (argc != 3)
    {
    std::cerr << "Usage: " << argv[0] << " FullAttributeFilename InstanceOnlyFileName\n";
    return -1;
    }
  smtk::attribute::Manager manager;
  std::cout << "Manager Created\n";
  // Lets add some analyses
  std::set<std::string> analysis;
  analysis.insert("Flow");
  analysis.insert("General");
  analysis.insert("Time");
  manager.defineAnalysis("CFD Flow", analysis);
  analysis.clear();

  analysis.insert("Flow");
  analysis.insert("Heat");
  analysis.insert("General");
  analysis.insert("Time");
  manager.defineAnalysis("CFD Flow with Heat Transfer", analysis);
  analysis.clear();

  analysis.insert("Constituent");
  analysis.insert("General");
  analysis.insert("Time");
  manager.defineAnalysis("Constituent Transport", analysis);
  analysis.clear();

  double lcolor1[] = {1.0, 1.0, 0.0, 0.1};
  double lcolor2[] = {1.0, 0.0, 1.0, 0.2};
  double lcolor3[] = {0.0, 1.0, 1.0, 0.3};
  manager.addAdvanceLevel(0, "Level 0", lcolor1);
  manager.addAdvanceLevel(1, "Level 1", lcolor2);
  manager.addAdvanceLevel(2, "Level 2", lcolor3);

  // Lets create an attribute to represent an expression
  smtk::attribute::DefinitionPtr expDef = manager.createDefinition("ExpDef");
  expDef->setBriefDescription("Sample Expression");
  expDef->setDetailedDescription("Sample Expression for testing\nThere is not much here!");
  smtk::attribute::StringItemDefinitionPtr eitemdef =
    expDef->addItemDefinition<smtk::attribute::StringItemDefinitionPtr>("Expression String");
  smtk::attribute::StringItemDefinitionPtr eitemdef2 =
    expDef->addItemDefinition<smtk::attribute::StringItemDefinition>("Aux String");
  eitemdef->setDefaultValue("sample");

  smtk::attribute::DefinitionPtr base = manager.createDefinition("BaseDef");
  // Lets add some item definitions
  smtk::attribute::IntItemDefinitionPtr iitemdef =
    base->addItemDefinition<smtk::attribute::IntItemDefinitionPtr>("TEMPORAL");
  iitemdef->setCommonValueLabel("Time");
  iitemdef->addDiscreteValue(0, "Seconds");
  iitemdef->addDiscreteValue(1, "Minutes");
  iitemdef->addDiscreteValue(2, "Hours");
  iitemdef->addDiscreteValue(3, "Days");
  iitemdef->setDefaultDiscreteIndex(0);
  iitemdef->addCategory("Time");
  iitemdef =
    base->addItemDefinition<smtk::attribute::IntItemDefinitionPtr>("IntItem2");
  iitemdef->setDefaultValue(10);
  iitemdef->addCategory("Heat");

  smtk::attribute::DefinitionPtr def1 = manager.createDefinition("Derived1", "BaseDef");
  def1->setAssociationMask(smtk::model::Item::MODEL_DOMAIN); // belongs on model
   // Lets add some item definitions
  smtk::attribute::DoubleItemDefinitionPtr ditemdef =
    def1->addItemDefinition<smtk::attribute::DoubleItemDefinitionPtr>("DoubleItem1");
  // Allow this one to hold an expression
  ditemdef->addCategory("Veg");
  ditemdef->setExpressionDefinition(expDef);
  // Check to make sure we can use expressions
  if (!ditemdef->allowsExpressions())
    {
    std::cout << "ERROR - Item Def does not allow expressions\n";
    status = -1;
    }
  ditemdef =
    def1->addItemDefinition<smtk::attribute::DoubleItemDefinitionPtr>("DoubleItem2");
  ditemdef->setDefaultValue(-35.2);
  ditemdef->setMinRange(-100, true);
  ditemdef->setMaxRange(125.0, false);
  ditemdef->addCategory("Constituent");
  smtk::attribute::VoidItemDefinitionPtr vdef =
    def1->addItemDefinition<smtk::attribute::VoidItemDefinitionPtr>("VoidItem");
  vdef->setIsOptional(true);
  vdef->setLabel("Option 1");


  smtk::attribute::DefinitionPtr def2 = manager.createDefinition("Derived2", "Derived1");
  def2->setAssociationMask(smtk::model::Item::REGION);
   // Lets add some item definitions
  smtk::attribute::StringItemDefinitionPtr sitemdef =
    def2->addItemDefinition<smtk::attribute::StringItemDefinitionPtr>("StringItem1");
  sitemdef->setIsMultiline(true);
  sitemdef->addCategory("Flow");
  sitemdef =
    def2->addItemDefinition<smtk::attribute::StringItemDefinitionPtr>("StringItem2");
  sitemdef->setDefaultValue("Default");
  sitemdef->addCategory("General");
  smtk::attribute::DirectoryItemDefinitionPtr dirdef =
    def2->addItemDefinition<smtk::attribute::DirectoryItemDefinitionPtr>("DirectoryItem");
  dirdef->setShouldExist(true);
  dirdef->setShouldBeRelative(true);
  smtk::attribute::FileItemDefinitionPtr fdef =
    def2->addItemDefinition<smtk::attribute::FileItemDefinitionPtr>("FileItem");
  fdef->setShouldBeRelative(true);
  smtk::attribute::GroupItemDefinitionPtr gdef1, gdef =
    def2->addItemDefinition<smtk::attribute::GroupItemDefinitionPtr>("GroupItem");
  gdef->addItemDefinition<smtk::attribute::FileItemDefinitionPtr>("File1");
  gdef1 = gdef->addItemDefinition<smtk::attribute::GroupItemDefinitionPtr>("SubGroup");
  sitemdef =
    gdef1->addItemDefinition<smtk::attribute::StringItemDefinitionPtr>("GroupString");
  sitemdef->setDefaultValue("Something Cool");
  sitemdef->addCategory("General");
  sitemdef->addCategory("Flow");

  // Add in a Attribute definition with a reference to another attribute
  smtk::attribute::DefinitionPtr attrefdef = manager.createDefinition("AttributeReferenceDef");
  smtk::attribute::RefItemDefinitionPtr aritemdef =
    attrefdef->addItemDefinition<smtk::attribute::RefItemDefinitionPtr>("BaseDefItem");
  aritemdef->setCommonValueLabel("A reference to another attribute");
  aritemdef->setAttributeDefinition(base);

  // Process Categories
  manager.updateCategories();
  // Lets test creating an attribute by passing in the expression definition explicitly
  smtk::attribute::AttributePtr expAtt = manager.createAttribute("Exp1", expDef);
  smtk::attribute::AttributePtr att = manager.createAttribute("testAtt", "Derived2");
  if (!att)
    {
    std::cout << "ERROR: Attribute testAtt not created\n";
    status = -1;
    }

  smtk::attribute::ValueItemPtr vitem;
  smtk::attribute::ItemPtr item;

  // Find the expression enabled item
  item = att->item(2);
  vitem = smtk::dynamic_pointer_cast<smtk::attribute::ValueItem>(item);
  smtk::util::AttributeWriter writer;
  smtk::util::Logger logger;
  if (writer.write(manager, argv[1],logger))
    {
    std::cerr << "Errors encountered creating Attribute File:\n";
    std::cerr << logger.convertToString();
    status = -1;
    }
  // Now repeat but only save the instance section
  writer.includeModelInformation(false);
  writer.includeDefinitions(false);
  writer.includeViews(false);
  if (writer.write(manager, argv[2],logger))
    {
    std::cerr << "Errors encountered creating Instance Only Attribute File:\n";
    std::cerr << logger.convertToString();
    status = -1;
    }

  std::cout << "Manager destroyed\n";
  }
  return status;
}
