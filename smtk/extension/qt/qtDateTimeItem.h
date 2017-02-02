//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
// .NAME qtDateTimeItem - UI components for attribute DateTimeItem
// .SECTION Description
// .SECTION See Also
// qtItem

#ifndef __smtk_extension_qtDateTimeItem_h
#define __smtk_extension_qtDateTimeItem_h

#include "smtk/extension/qt/qtItem.h"
#include "smtk/extension/qt/Exports.h"
#include <QString>

class QAction;
class QDateTime;
class QDateTimeEdit;

namespace smtk
{
  namespace extension
  {
    class SMTKQTEXT_EXPORT qtDateTimeItem : public qtItem
    {
      Q_OBJECT

    public:
      qtDateTimeItem(smtk::attribute::DateTimeItemPtr, QWidget* p,
        qtBaseView* bview, Qt::Orientation enumOrient = Qt::Horizontal);
      virtual ~qtDateTimeItem();
      virtual void setLabelVisible(bool);

    public slots:
      void setOutputOptional(int);

    signals:

    protected slots:
      virtual void updateItemData();
      virtual void onChildWidgetSizeChanged();
      virtual void onAddNewValue();
      virtual void onRemoveValue();

      void onDateTimeChanged(const QDateTime& newValue);

      // Time zone menu actions
      void onTimeZoneUnset();
      void onTimeZoneUTC();
      void onTimeZoneRegion();

      // Time zone dialog actions
      void onRegionSelected();

    protected:
      smtk::attribute::DateTimeItemPtr datetimeItem();
      virtual void createWidget();
      QWidget *createDateTimeWidget(int elementIdx);
      virtual void loadInputValues();
      virtual void updateUI();
      virtual void addInputEditor(int i);
      virtual void updateExtensibleState();
      virtual void clearChildWidgets();
      void updateBackground(QDateTimeEdit *dtEdit, bool valid);
      void updateTimeZoneMenu(QAction *selectedAction = NULL);

      void setTimeZone(std::size_t element, const QString& region);
      void setTimeZoneToUTC(std::size_t element);

    private:
      class qtDateTimeItemInternals;
      qtDateTimeItemInternals *Internals;
    }; // class qDateTimeItem
  }; // namespace attribute
}; // namespace smtk

#endif
