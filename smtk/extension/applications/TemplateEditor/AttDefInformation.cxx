//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#include <QModelIndex>

#include "smtk/attribute/Definition.h"
#include "smtk/attribute/GroupItemDefinition.h"
#include "smtk/attribute/ItemDefinition.h"

#include "AttDefDataModel.h"
#include "AttDefInformation.h"
#include "ItemDefDialog.h"
#include "ItemDefinitionHelper.h"
#include "ItemDefinitionsDataModel.h"
#include "ui_AttDefInformation.h"

// -----------------------------------------------------------------------------
AttDefInformation::AttDefInformation(QWidget* parent)
  : QWidget(parent)
  , Ui(new Ui::AttDefInformation)
  , InheritedItemDefModel(new ItemDefinitionsDataModel(this))
  , OwnedItemDefModel(new ItemDefinitionsDataModel(this))
{
  this->Ui->setupUi(this);

  connect(this->Ui->pbApplyDef, SIGNAL(clicked()), this, SLOT(onSaveAttDef()));
  connect(this->Ui->pbAddItemDef, SIGNAL(clicked()), this, SLOT(onAddItemDef()));
  connect(this->Ui->pbDeleteItemDef, SIGNAL(clicked()), this, SLOT(onRemoveItemDef()));

  this->Ui->tvInheritedItems->setExpandsOnDoubleClick(false);
  this->Ui->tvOwnedItems->setExpandsOnDoubleClick(false);
  connect(this->Ui->tvInheritedItems, SIGNAL(doubleClicked(const QModelIndex&)), this,
    SLOT(showInheritedItemDetails(const QModelIndex&)));
  connect(this->Ui->tvOwnedItems, SIGNAL(doubleClicked(const QModelIndex&)), this,
    SLOT(showOwnedItemDetails(const QModelIndex&)));
}

// -----------------------------------------------------------------------------
AttDefInformation::~AttDefInformation() = default;

// -----------------------------------------------------------------------------
void AttDefInformation::onAttDefChanged(
  const QModelIndex& currentDef, const QModelIndex& previousDef)
{
  this->updateAttDefData(currentDef);
  this->updateInheritedItemDef();
  this->updateOwnedItemDef();
}

// -----------------------------------------------------------------------------
void AttDefInformation::updateAttDefData(const QModelIndex& currentDef)
{
  AttDefDataModel const* model = qobject_cast<AttDefDataModel const*>(currentDef.model());
  this->CurrentAttDef = model->get(currentDef);

  auto baseDef = this->CurrentAttDef->baseDefinition();
  QString baseDefStr = baseDef ? QString::fromStdString(baseDef->type()) : "";

  this->Ui->laType->setText(QString::fromStdString(this->CurrentAttDef->type()));
  this->Ui->laBaseType->setText(baseDefStr);
  this->Ui->leLabel->setText(QString::fromStdString(this->CurrentAttDef->label()));
  this->Ui->cbUnique->setChecked(this->CurrentAttDef->isUnique());
  this->Ui->cbAbstract->setChecked(this->CurrentAttDef->isAbstract());
}

// -----------------------------------------------------------------------------
void AttDefInformation::updateOwnedItemDef()
{
  delete this->OwnedItemDefModel;
  this->OwnedItemDefModel = new ItemDefinitionsDataModel(this);

  this->OwnedItemDefModel->appendBranchToRoot(this->CurrentAttDef);
  this->Ui->tvOwnedItems->setModel(this->OwnedItemDefModel);
  this->Ui->tvOwnedItems->setColumnWidth(0, 250);
  this->Ui->tvOwnedItems->setColumnHidden(2, true);
  this->Ui->tvOwnedItems->expandAll();
}

// -----------------------------------------------------------------------------
void AttDefInformation::updateInheritedItemDef()
{
  delete this->InheritedItemDefModel;
  this->InheritedItemDefModel = new ItemDefinitionsDataModel(this);

  // Add inherited ItemDefinitions from each parent type recursively
  std::function<void(smtk::attribute::DefinitionPtr&)> recursiveAdd;
  recursiveAdd = [&recursiveAdd, this](smtk::attribute::DefinitionPtr& def) {
    smtk::attribute::DefinitionPtr baseDef = def->baseDefinition();
    if (baseDef)
    {
      this->InheritedItemDefModel->appendBranchToRoot(baseDef);
      recursiveAdd(baseDef);
    }
  };
  recursiveAdd(this->CurrentAttDef);

  this->Ui->tvInheritedItems->setModel(this->InheritedItemDefModel);
  this->Ui->tvInheritedItems->setColumnWidth(0, 250);
  this->Ui->tvInheritedItems->expandAll();
}

// -----------------------------------------------------------------------------
void AttDefInformation::onSaveAttDef()
{
  this->CurrentAttDef->setLabel(this->Ui->leLabel->text().toStdString());
  this->CurrentAttDef->setIsUnique(this->Ui->cbUnique->isChecked());
  this->CurrentAttDef->setIsAbstract(this->Ui->cbAbstract->isChecked());
  emit systemChanged(true);
}

// -----------------------------------------------------------------------------
void AttDefInformation::onAddItemDef()
{
  ItemDefDialog dialog(this);
  dialog.setAttDef(this->CurrentAttDef);
  if (dialog.exec() == QDialog::Accepted)
  {
    // Get data input from the user and add current selection specifics.
    auto props = dialog.getInputValues();
    props.Definition = this->CurrentAttDef;

    QItemSelectionModel* sm = this->Ui->tvOwnedItems->selectionModel();
    const auto currentIndex = sm->currentIndex();
    props.ParentIndex = currentIndex.parent();

    this->OwnedItemDefModel->insert(props);
    emit systemChanged(true);
  }
}

// -----------------------------------------------------------------------------
void AttDefInformation::onRemoveItemDef()
{
  QItemSelectionModel* sm = this->Ui->tvOwnedItems->selectionModel();
  this->OwnedItemDefModel->remove(sm->currentIndex(), this->CurrentAttDef);
  emit systemChanged(true);
}

// -----------------------------------------------------------------------------
void AttDefInformation::showInheritedItemDetails(const QModelIndex& index)
{
  ItemDefinitionsDataModel const* model =
    qobject_cast<ItemDefinitionsDataModel const*>(index.model());
  auto const& itemDef = model->get(index);

  ItemDefDialog dialog(this);
  dialog.setItemDef(itemDef);
  dialog.setEditMode(ItemDefDialog::EditMode::SHOW);
  dialog.exec();
}

// -----------------------------------------------------------------------------
void AttDefInformation::showOwnedItemDetails(const QModelIndex& index)
{
  ItemDefinitionsDataModel const* model =
    qobject_cast<ItemDefinitionsDataModel const*>(index.model());
  auto const& itemDef = model->get(index);

  ItemDefDialog dialog(this);
  dialog.setItemDef(itemDef);
  dialog.setAttDef(this->CurrentAttDef);
  dialog.setEditMode(ItemDefDialog::EditMode::EDIT);
  if (dialog.exec() == QDialog::Accepted)
  {
    // TODO update values depending on its concrete implementation.
    // This should probably be done in the Model class.
    QItemSelectionModel* sm = this->Ui->tvOwnedItems->selectionModel();
    const auto currentIndex = sm->currentIndex();
    auto itemDef = this->OwnedItemDefModel->get(currentIndex);

    auto props = dialog.getInputValues();
    itemDef->setLabel(props.Label);
    itemDef->setVersion(props.Version);
    emit systemChanged(true);
  }
}
