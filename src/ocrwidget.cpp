/***************************************************************************
 *            ocrwidget.cpp
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
#include <math.h>
#include <QtWidgets>
#include <QSettings>
#include <QEventLoop>
#include <poppler-qt5.h>

#include "ocrwidget.h"
#include "ocrworker.h"

extern QSettings *settings;

OcrWidget::OcrWidget(QWidget *parent) : QWidget(parent)
{
  // Create shared queue with files to process
  queue = QSharedPointer<Queue>(new Queue());

  ocrTextEdit = new QTextEdit();

  resultTable = new QTableWidget();
  resultTable->setColumnCount(2);
  resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  resultTable->setHorizontalHeaderLabels(QStringList({tr("Word"), tr("Matches")}));
  resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  connect(resultTable, &QTableWidget::cellClicked, this, &OcrWidget::wordSelected);

  QFile wordsFile("words.txt");
  if(wordsFile.open(QIODevice::ReadOnly)) {
    while(!wordsFile.atEnd()) {
      QString wordLine = QString(wordsFile.readLine().trimmed());
      QList<QString> searchWord = wordLine.split(";");
      if(searchWord.isEmpty()) {
	continue;
      }
      resultTable->setRowCount(resultTable->rowCount() + 1);
      QTableWidgetItem *item = new QTableWidgetItem(searchWord.first());
      item->setData(Qt::ToolTipRole, wordLine.replace(";", "\n"));
      item->setData(Qt::UserRole, QVariant(searchWord));
      resultTable->setItem(resultTable->rowCount() - 1, 0, item);
    }
    wordsFile.close();
  } else {
    qFatal("Could not open 'words.txt' file for reading!\n");
  }

  progressBar = new QProgressBar(this);
  progressBar->setMinimum(0);

  QHBoxLayout *hLayout = new QHBoxLayout();
  hLayout->addWidget(ocrTextEdit);
  hLayout->addWidget(resultTable);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(hLayout);
  layout->addWidget(progressBar);
  
  setLayout(layout);
}

OcrWidget::~OcrWidget()
{
}

void OcrWidget::process(QListWidgetItem *item)
{
  threads = settings->value("Tesseract/maxThreads", 4).toInt();
  doneThreads = 0;
  ocrTexts.clear();

  currentFileInfo = QFileInfo(item->data(Qt::UserRole).toString());
  Poppler::Document *document = Poppler::Document::load(item->data(Qt::UserRole).toString());

  // Extract text from all PDF pages either directly or using OCR if no text is found
  progressBar->setMaximum(document->numPages());
  progressBar->setFormat(tr("Processing page ") + "%v / " + QString::number(progressBar->maximum()));
  progressBar->setValue(progressBar->minimum());
  for(int a = 0; a < document->numPages(); ++a) {
    queue->append(QPair<QImage, int> (document->page(a)->renderToImage(settings->value("Matching/pdfImageDpi", "300").toInt(), settings->value("Matching/pdfImageDpi", "300").toInt()).convertToFormat(QImage::Format_RGB32), a));
    /* The following is broken after threads were implemented. It will result in out-of-place pages
    if(settings->value("Matching/checkForText", true).toBool() &&
      !document->page(a)->text(QRectF()).isEmpty()) {
      ocrText.append(document->page(a)->text(QRectF()));
    } else {
      queue->append(QPair<QImage, int> (document->page(a)->renderToImage(settings->value("Matching/pdfImageDpi", "300").toInt(), settings->value("Matching/pdfImageDpi", "300").toInt()).convertToFormat(QImage::Format_RGB32), a));
    }
    */
  }
  //progressBar->setValue(progressBar->maximum());

  // Create and run threads
  QList<QThread*> threadList;
  for(int curThread = 1; curThread <= threads; ++curThread) {
    QThread *thread = new QThread;
    OcrWorker *worker = new OcrWorker(queue);
    worker->moveToThread(thread);
    connect(thread, &QThread::started, worker, &OcrWorker::run);
    connect(worker, &OcrWorker::entryReady, this, &OcrWidget::entryReady);
    connect(worker, &OcrWorker::allDone, this, &OcrWidget::checkThreads);
    connect(thread, &QThread::finished, worker, &OcrWorker::deleteLater);
    threadList.append(thread);
    // Do not start more threads if we have less pages than allowed threads
    if(curThread == document->numPages()) {
      threads = document->numPages();
      break;
    }
  }
  delete document;
  // Ready, set, GO! Start all threads
  for(const auto thread: threadList) {
    thread->start();
  }

  QEventLoop waitForDone;
  connect(this, &OcrWidget::allDone, &waitForDone, &QEventLoop::quit);
  waitForDone.exec();
}

void OcrWidget::checkThreads()
{
  QMutexLocker locker(&checkThreadMutex);

  doneThreads++;
  if(doneThreads != threads) {
    return;
  }

  // Sort begin
  std::sort(ocrTexts.begin(), ocrTexts.end(),
	    [=](const QPair<QString, int> a, const QPair<QString, int> b) -> bool {
	      return a.second < b.second;
	    });
  // Sort end

  QString ocrText = "";
  for(const auto &pageText: ocrTexts) {
    ocrText.append(pageText.first);
  }
  
  ocrText = ocrText.replace("\n", "");
  printf("ALL DONE!\n");
  // Segment ocrText and add them to list
  QString chars = "";
  pdfWords.clear();
  bool inWord = ocrText.isEmpty()?false:ocrText.at(0).isLetterOrNumber();
  for(const auto &currentChar: ocrText) {
    if((currentChar.isLetterOrNumber() || currentChar == '-') == !inWord) {
      pdfWords.append(CharData(inWord, chars));
      printf("PDFWORD: '%s', %d\n", chars.toStdString().c_str(), inWord);
      chars.clear();
      inWord = !inWord;
    }
    chars.append(currentChar);
  }
  for(auto &word: pdfWords) {
    if(!word.isWord()) {
      continue;
    }
    bool found = false;
    for(int a = 0; a < resultTable->rowCount(); ++a) {
      for(const auto &variation: resultTable->item(a, 0)->data(Qt::UserRole).toList()) {
	if(wordDifference(word.getChars().toLower(), variation.toString().toLower()) >= settings->value("Matching/minMatchPercentage", "80").toInt()) {
	  word.setMatch(resultTable->item(a, 0)->data(Qt::UserRole).toList().first().toString());
	  printf("WORD: '%s'\n", resultTable->item(a, 0)->data(Qt::UserRole).toList().first().toString().toStdString().c_str());
	  found = true;
	  break;
	}
      }
      if(found) {
	break;
      }
    }
  }

  bool foundAny = false;
  for(int a = 0; a < resultTable->rowCount(); ++a) {
    int numMatches = 0;
    for(const auto &word: pdfWords) {
      if(word.getMatch() == resultTable->item(a, 0)->data(Qt::UserRole).toList().first().toString()) {
	numMatches++;
	foundAny = true;
      }
    }
    QTableWidgetItem *numMatchesItem = new QTableWidgetItem(QString::number(numMatches));
    resultTable->setItem(a, 1, numMatchesItem);
  }

  QString finalHtml = redrawText();
  // Save to disk as HTML with all words marked, but only if single match or more has been found
  if(foundAny) {
    QFile outHtml(currentFileInfo.absolutePath() + "/" + currentFileInfo.baseName() + ".html");
    if(outHtml.open(QIODevice::WriteOnly)) {
      outHtml.write("<!DOCTYPE html>\n");
      outHtml.write("<html>\n");
      outHtml.write("<head>\n");
      outHtml.write("<style>\n\nbody {\nfont-family: Arial, Helvetica, sans-serif;\n}\n\n");
      outHtml.write("table {\nborder-collapse: collapse;\n}\n\n");
      outHtml.write("td, th {\nborder: 1px solid #ddd;\npadding: 5px;\n}\n\n");
      outHtml.write("th {\npadding-top: 12px;\npadding-bottom: 12px;\ntext-align: left;\nbackground-color: #04AA6D;\ncolor: white;\n}\n");
      outHtml.write("</style>\n");
      outHtml.write("</head>\n");
      outHtml.write("<body>\n");
      outHtml.write("<table>\n");
      outHtml.write("<tr>\n");
      outHtml.write("<th>Word</th>\n");
      outHtml.write("<th>Matches</th>\n");
      outHtml.write("</tr>\n");
      for(int row = 0; row < resultTable->rowCount(); ++row) {
	outHtml.write("<tr>\n");
	outHtml.write("<td>");
	outHtml.write(resultTable->item(row, 0)->data(Qt::UserRole).toList().first().toString().toUtf8());
	outHtml.write("</td>\n");
	outHtml.write("<td>");
	outHtml.write(resultTable->item(row, 1)->text().toUtf8());
	outHtml.write("</td>\n");
	outHtml.write("</tr>\n");
      }
      outHtml.write("</table>\n");
      outHtml.write("<h1>Journal text</h1>\n");
      outHtml.write("<p>\n");
      outHtml.write(finalHtml.toUtf8());
      outHtml.write("</p>\n");
      outHtml.write("</body>");
      outHtml.write("</html>");
      outHtml.close();
    }
  }

  emit allDone();
}

int OcrWidget::wordDifference(const QString &s1, const QString &s2) 
{
  if(s1.isEmpty() || s2.isEmpty()) {
    return -1;
  }
  
  const std::size_t len1 = s1.length(), len2 = s2.length();
  std::vector<unsigned int> col(len2 + 1), prevCol(len2 + 1);
  
  for(unsigned int i = 0; i < prevCol.size(); i++)
    prevCol[i] = i;
  for(unsigned int i = 0; i < len1; i++) {
    col[0] = i + 1;
    for (unsigned int j = 0; j < len2; j++)
      // note that std::min({arg1, arg2, arg3}) works only in C++11,
      // for C++98 use std::min(std::min(arg1, arg2), arg3)
      col[j + 1] = std::min({ prevCol[1 + j] + 1, col[j] + 1, prevCol[j] + (s1[i] == s2[j] ? 0 : 1) });
    col.swap(prevCol);
  }
  unsigned int distance = prevCol[len2];

  int percentage = 0;
  if(s1.length() > s2.length()) {
    percentage = (int)(100.0 / (double)s1.length() * ((double)s1.length() - (double)distance));
  } else {
    percentage = (int)(100.0 / (double)s2.length() * ((double)s2.length() - (double)distance));
  }

  return percentage;
}

void OcrWidget::wordSelected(int row, int)
{
  redrawText(resultTable->item(row, 0)->data(Qt::UserRole).toList().first().toString());
}

QString OcrWidget::redrawText(const QString &mark)
{
  QString combined = "";
  if(mark.isEmpty()) {
  for(const auto &word: pdfWords) {
    bool colorize = false;
    for(int row = 0; row < resultTable->rowCount(); ++row) {
      if(word.getMatch() ==
	 resultTable->item(row, 0)->data(Qt::UserRole).toList().first().toString()) {
	combined.append("<span style=\"color:#" + settings->value("Matching/matchedWordColor", "ff0000").toString() + "\">");
	colorize = true;
      }
    }
    combined.append(word.getChars().toHtmlEscaped());
    if(colorize) {
      combined.append("</span>");
    }
  }
  } else {
  for(const auto &word: pdfWords) {
    bool colorize = false;
    if(word.getMatch() == mark) {
      combined.append("<span style=\"color:#" + settings->value("Matching/matchedWordColor", "ff0000").toString() + "\">");
      colorize = true;
    }
    combined.append(word.getChars().toHtmlEscaped());
    if(colorize) {
      combined.append("</span>");
    }
  }
  }
  ocrTextEdit->setHtml(combined);
  return combined;
}

void OcrWidget::entryReady(const QString &pageText, const int &pageNum)
{
  QMutexLocker locker(&entryMutex);
  progressBar->setValue(progressBar->value() + 1);
  ocrTexts.append(QPair<QString, int> (pageText, pageNum));
}
