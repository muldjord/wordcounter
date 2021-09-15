/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            configdialog.cpp
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

#include <QtWidgets>

#include "configdialog.h"

ConfigDialog::ConfigDialog(QWidget *parent) : QDialog(parent)
{
  setFixedSize(800, 750);
  contentsWidget = new QListWidget;
  contentsWidget->setViewMode(QListView::IconMode);
  contentsWidget->setIconSize(QSize(64, 64));
  contentsWidget->setMovement(QListView::Static);
  contentsWidget->setMaximumWidth(128);
  contentsWidget->setMinimumWidth(128);
  contentsWidget->setSpacing(12);

  generalPage = new GeneralPage();
  matchingPage = new MatchingPage();
  outputPage = new OutputPage();
  
  pagesWidget = new QStackedWidget;
  pagesWidget->addWidget(generalPage);
  pagesWidget->addWidget(matchingPage);
  pagesWidget->addWidget(outputPage);

  QPushButton *okButton = new QPushButton(tr("Save"));

  createIcons();
  contentsWidget->setCurrentRow(0);

  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

  QHBoxLayout *horizontalLayout = new QHBoxLayout;
  horizontalLayout->addWidget(contentsWidget);
  horizontalLayout->addWidget(pagesWidget, 1);

  QHBoxLayout *buttonsLayout = new QHBoxLayout;
  buttonsLayout->addStretch(1);
  buttonsLayout->addWidget(okButton);

  QVBoxLayout *generalLayout = new QVBoxLayout;
  generalLayout->addLayout(horizontalLayout);
  generalLayout->addLayout(buttonsLayout);
  setLayout(generalLayout);

  setWindowTitle(tr("Preferences"));
}

void ConfigDialog::createIcons()
{
  QListWidgetItem *generalButton = new QListWidgetItem(contentsWidget);
  generalButton->setIcon(QIcon(":generalconfig.png"));
  generalButton->setText(tr("General"));
  generalButton->setTextAlignment(Qt::AlignHCenter);
  generalButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

  QListWidgetItem *matchingButton = new QListWidgetItem(contentsWidget);
  matchingButton->setIcon(QIcon(":matchingconfig.png"));
  matchingButton->setText(tr("Matching"));
  matchingButton->setTextAlignment(Qt::AlignHCenter);
  matchingButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

  QListWidgetItem *outputButton = new QListWidgetItem(contentsWidget);
  outputButton->setIcon(QIcon(":outputconfig.png"));
  outputButton->setText(tr("Output"));
  outputButton->setTextAlignment(Qt::AlignHCenter);
  outputButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

  connect(contentsWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
          this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
}

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
  if (!current)
    current = previous;

  pagesWidget->setCurrentIndex(contentsWidget->row(current));
}
