/***************************************************************************
 *            datawidget.cpp
 *
 *  Wed Sep 8 12:00:00 CEST 2021
 *  Copyright 2021 Lars Bisballe Jensen
 *  larsbjensen@gmail.com
 ****************************************************************************/
/*
 *  This file is part of wordcounter.
 *
 *  wordcounter is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  wordcounter is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with wordcounter; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#define DEBUG

#include <QtWidgets>
#include <QSettings>

#include "datawidget.h"

extern QSettings *settings;

DataWidget::DataWidget(QWidget *parent) : QListWidget(parent)
{
  setSelectionMode(QAbstractItemView::ExtendedSelection);
}

DataWidget::~DataWidget()
{
}

void DataWidget::clearAll()
{
  clear();
  qInfo("dataWidget cleared!\n");
}

void DataWidget::removeSelected()
{
  qInfo("Removing selected...\n");
  QList<QListWidgetItem *> selectedFiles = selectedItems();
  foreach(QListWidgetItem *item, selectedFiles) {
    delete item;
  }
}
