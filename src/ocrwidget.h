/***************************************************************************
 *            ocrwidget.h
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

#ifndef OCRWIDGET_H
#define OCRWIDGET_H

#include <QWidget>
#include <QList>
#include <QLabel>
#include <QListWidgetItem>
#include <QTextEdit>
#include <QTableWidget>
#include <QProgressBar>
#include <QMutex>
#include <QFileInfo>

#include "chardata.h"
#include "queue.h"

class OcrWidget : public QWidget
{
  Q_OBJECT
    
public:
  OcrWidget(QWidget *parent);
  ~OcrWidget();
  void process(QListWidgetItem *item);
  QSharedPointer<Queue> queue;
  
private slots:
  void wordSelected(int row, int);
  void entryReady(const QString &pageText, const int &pageNum);
  void checkThreads();

signals:
  void allDone();

private:
  int wordDifference(const QString &s1, const QString &s2);
  QString redrawText(const QString &mark = QString());
  
  QMutex entryMutex;
  QMutex checkThreadMutex;
  int threads = 6;
  int doneThreads = 0;
  QList<QPair<QString, int> > ocrTexts;;
  QProgressBar *progressBar;
  QList<CharData> pdfWords;
  QTextEdit *ocrTextEdit;
  QTableWidget *resultTable;
  QFileInfo currentFileInfo = QFileInfo();
};

#endif
