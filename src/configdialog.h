/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            configdialog.h
 *
 *  Wed Sep 8 12:00:00 CEST 2021
 *  Copyright 2021 Lars Bisballe Jensen
 *  larsbjensen@gmail.com
 ****************************************************************************/
/*
 *  This file is part of PulseReg2.
 *
 *  PulseReg2 is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  PulseReg2 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PulseReg2; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#ifndef LENSREG_CONFIGDIALOG_H
#define LENSREG_CONFIGDIALOG_H

#include "configpages.h"

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>

class ConfigDialog : public QDialog
{
  Q_OBJECT

public:
  ConfigDialog(QWidget *parent);

public slots:
  void changePage(QListWidgetItem *current, QListWidgetItem *previous);
  
private:
  void createIcons();
  QListWidget *contentsWidget;
  QStackedWidget *pagesWidget;
  GeneralPage *generalPage;
  MatchingPage *matchingPage;
  OutputPage *outputPage;
};

#endif/*__LENSREG_CONFIGDIALOG_H__*/
