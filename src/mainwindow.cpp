/***************************************************************************
 *            mainwindow.cpp
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

#include <stdio.h>
#include <QtWidgets>
#include <QSettings>

#include "mainwindow.h"
#include "aboutbox.h"
#include "configdialog.h"

extern QSettings *settings;

MainWindow::MainWindow()
{
  restoreGeometry(settings->value("windowState", "").toByteArray());

  if(!settings->contains("alwaysOnTop")) {
    showPreferences();
  }

  setWindowTitle("Wordcounter v" VERSION);

  if(settings->value("alwaysOnTop").toBool()) {
    setWindowFlags(Qt::WindowStaysOnTopHint);
  }

  outputWidget = new QPlainTextEdit();

  createActions();
  createMenus();

  dataWidget = new DataWidget(this);
  connect(dataWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(processSelected()));

  addFolderButton = new QPushButton(tr("Add files from directory..."));
  addFolderButton->setIcon(QIcon(":addfolder.png"));
  connect(addFolderButton, SIGNAL(clicked()), this, SLOT(chooseFolder()));

  processSelectedButton = new QPushButton(tr("Process selected"));
  processSelectedButton->setIcon(QIcon(":processall.png"));
  connect(processSelectedButton, SIGNAL(clicked()), this, SLOT(processSelected()));

  removeSelectedButton = new QPushButton(tr("Remove selected"));
  removeSelectedButton->setIcon(QIcon(":delete.png"));
  connect(removeSelectedButton, SIGNAL(clicked()), dataWidget, SLOT(removeSelected()));

  clearListButton = new QPushButton(tr("Clear list"));
  clearListButton->setIcon(QIcon(":delete.png"));
  connect(clearListButton, SIGNAL(clicked()),
	  dataWidget, SLOT(clearAll()));

  progressBar = new QProgressBar(this);
  progressBar->setMinimum(0);

  QVBoxLayout *buttonsLayout = new QVBoxLayout();
  buttonsLayout->addWidget(addFolderButton);
  buttonsLayout->addWidget(removeSelectedButton);
  buttonsLayout->addWidget(clearListButton);
  buttonsLayout->addStretch();
  buttonsLayout->addWidget(processSelectedButton);

  QHBoxLayout *functionsLayout = new QHBoxLayout();
  functionsLayout->addLayout(buttonsLayout);

  QHBoxLayout *dataLayout = new QHBoxLayout();
  dataLayout->addWidget(dataWidget);
  
  QGroupBox *functionsGroup = new QGroupBox(tr("Functions"));
  functionsGroup->setLayout(functionsLayout);

  ocrWidget = new OcrWidget(this);

  QVBoxLayout *processLayout = new QVBoxLayout();
  processLayout->addWidget(ocrWidget);

  QGroupBox *processGroup = new QGroupBox(tr("Analysis and results"));
  processGroup->setLayout(processLayout);

  QGroupBox *dataGroup = new QGroupBox(tr("Files"));
  dataGroup->setLayout(dataLayout);
  
  QHBoxLayout *topLayout = new QHBoxLayout();
  topLayout->addWidget(functionsGroup);
  topLayout->addWidget(dataGroup);
  
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(topLayout);
  layout->addWidget(processGroup);
  layout->addWidget(progressBar);

  setCentralWidget(new QWidget());
  centralWidget()->setLayout(layout);

  show();

  if(settings->value("alwaysMaximize").toBool()) {
    showMaximized();
  }
}

MainWindow::~MainWindow()
{
  settings->setValue("windowState", saveGeometry());
}

void MainWindow::chooseFolder()
{
  QString folder = QFileDialog::getExistingDirectory(this, tr("Choose folder"), ".");

  if(folder != "") {
    QDir::setCurrent(folder);
    addFromFolder(folder);
  }
}

bool MainWindow::addFromFile(const QString &filename)
{
  QListWidgetItem *item = new QListWidgetItem(filename);
  item->setData(Qt::UserRole, filename);
  dataWidget->addItem(item);
  return true;
}

bool MainWindow::addFromFolder(const QString &folder)
{
  qInfo("Now adding files from folder '%s'\n", folder.toStdString().c_str());
  QDir d(folder);
  QList<QFileInfo> fileInfos = d.entryInfoList(QStringList{"*.pdf"});
  for(int i = 0; i < fileInfos.size(); ++i) {
    addFromFile(fileInfos.at(i).absoluteFilePath());
  }
  //printList();
  return true;
}

void MainWindow::processSelected()
{
  disableAll();
  processSelectedButton->setText(tr("Cancel processing"));
  QList<QListWidgetItem *> selectedFiles = dataWidget->selectedItems();
  progressBar->setRange(1, selectedFiles.length());
  progressBar->reset();
  qInfo("Now processing selected files...\n");
  for(const auto &item: selectedFiles) {
    ocrWidget->process(item);
    progressBar->setValue(progressBar->value() + 1);
  }
  processSelectedButton->setText(tr("Process selected"));
  enableAll();
}

void MainWindow::enableAll()
{
  addFolderButton->setEnabled(true);
  removeSelectedButton->setEnabled(true);
  clearListButton->setEnabled(true);
  processSelectedButton->setEnabled(true);
}

void MainWindow::disableAll()
{
  addFolderButton->setEnabled(false);
  removeSelectedButton->setEnabled(false);
  clearListButton->setEnabled(false);
  processSelectedButton->setEnabled(false);
}

void MainWindow::createActions()
{
  quitAct = new QAction("&Quit", this);
  quitAct->setIcon(QIcon(":quit.png"));
  connect(quitAct, SIGNAL(triggered()), qApp, SLOT(quit()));

  preferencesAct = new QAction(tr("Edit &Preferences..."), this);
  preferencesAct->setIcon(QIcon(":preferences.png"));
  connect(preferencesAct, SIGNAL(triggered()), this, SLOT(showPreferences()));

  aboutAct = new QAction(tr("&About..."), this);
  aboutAct->setIcon(QIcon(":about.png"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(showAbout()));
}

void MainWindow::createMenus()
{
  fileMenu = new QMenu(tr("&File"), this);
  fileMenu->addAction(quitAct);

  optionsMenu = new QMenu(tr("&Options"), this);
  optionsMenu->addAction(preferencesAct);

  helpMenu = new QMenu(tr("&Help"), this);
  helpMenu->addAction(aboutAct);

  menuBar = new QMenuBar();
  menuBar->addMenu(fileMenu);
  menuBar->addMenu(optionsMenu);
  menuBar->addMenu(helpMenu);
  
  setMenuBar(menuBar);
  
#ifdef DEBUG  
  qDebug("Created menu...\n");
#endif
}

void MainWindow::showAbout()
{
  // Spawn about window
  AboutBox aboutBox(this);
  aboutBox.exec();
}

void MainWindow::showPreferences()
{
  // Spawn preferences dialog
  ConfigDialog preferences(this);
  preferences.exec();
}
