/***************************************************************************
 *            mainwindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QCheckBox>
#include <QPushButton>
#include <QProgressBar>
#include <QPlainTextEdit>

#include "datawidget.h"
#include "ocrwidget.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT
    
 public:
  MainWindow();
  ~MainWindow();
  
public slots:
  
protected:
  
signals:

private slots:
  void chooseFolder();
  void processSelected();
  void showAbout();
  void showPreferences();
  
private:
  void createActions();
  void createMenus();
  QAction *quitAct;
  QAction *preferencesAct;
  QAction *aboutAct;
  QMenu *fileMenu;
  QMenu *optionsMenu;
  QMenu *helpMenu;
  QMenuBar *menuBar;
  bool addFromFile(const QString &filename);
  bool addFromFolder(const QString &folder);
  QProgressBar *progressBar;
  //QListWidget *inputList;
  bool processing;
  QPushButton *processSelectedButton;
  QPushButton *addFolderButton;
  QPushButton *removeSelectedButton;
  QPushButton *clearListButton;
  void enableAll();
  void disableAll();
  //QPlainTextEdit *dataWidget;
  DataWidget *dataWidget;
  OcrWidget *ocrWidget;
  QPlainTextEdit *outputWidget;
  
};

#endif
