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
#include <poppler-qt5.h>

#include "ocrwidget.h"

extern QSettings *settings;

OcrWidget::OcrWidget(QWidget *parent) : QWidget(parent)
{
  QFile wordsFile("words.txt");
  if(wordsFile.open(QIODevice::ReadOnly)) {
    while(!wordsFile.atEnd()) {
      searchWords.append(QString(wordsFile.readLine().trimmed()).split(';'));
    }
    wordsFile.close();
  } else {
    qFatal("Could not open 'words.txt' file for reading!\n");
  }
  tesser = new tesseract::TessBaseAPI();
  ocrTextEdit = new QTextEdit();
  resultTable = new QTableWidget();
  resultTable->setColumnCount(2);
  resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  resultTable->setHorizontalHeaderLabels(QStringList({tr("Word"), tr("Matches")}));
  resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  connect(resultTable, &QTableWidget::cellClicked, this, &OcrWidget::wordSelected);
  QHBoxLayout *layout = new QHBoxLayout();
  layout->addWidget(ocrTextEdit);
  layout->addWidget(resultTable);

  setLayout(layout);
}

OcrWidget::~OcrWidget()
{
  delete tesser;
}

bool OcrWidget::process(QListWidgetItem *item)
{
  if(tesser->Init(nullptr,
		  settings->value("Tesseract/lang", "eng").toString().toStdString().c_str())) {
    qFatal("Could not initialize tesseract engine!\n");
    return false;
  }

  QString ocrText = "";
  Poppler::Document *document = Poppler::Document::load(item->data(Qt::UserRole).toString());

  // Extract text from all PDF pages either directly or using OCR if no text is found
  for(int a = 0; a < document->numPages(); ++a) {
    if(settings->value("Matching/checkForText", true).toBool() &&
       !document->page(a)->text(QRectF()).isEmpty()) {
      ocrText.append(document->page(a)->text(QRectF()));
    } else {
      QImage image =
	document->page(a)->renderToImage(settings->value("Matching/pdfImageDpi", "300").toInt()
					 , settings->value("Matching/pdfImageDpi", "300").toInt()
					 ).convertToFormat(QImage::Format_RGB32);
      tesser->SetImage(image.constBits(), image.width(), image.height(), 4, 4 * image.width());
      ocrText.append(tesser->GetUTF8Text());
    }
  }
  delete document;

  // Segment ocrText and add them to list
  QString chars = "";
  pdfWords.clear();
  bool inWord = ocrText.isEmpty()?false:ocrText.at(0).isLetterOrNumber();
  ocrText = ocrText.toHtmlEscaped();
  for(const auto &currentChar: ocrText) {
    if(currentChar.isLetterOrNumber() == !inWord) {
      pdfWords.append(CharData(inWord, chars));
      printf("FOUND: '%s', %d\n", chars.toStdString().c_str(), inWord);
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
    for(const auto &searchWord: searchWords) {
      for(const auto &variation: searchWord) {
	if(wordDifference(word.getChars(), variation) >= settings->value("Matching/minMatchPercentage", "80").toInt()) {
	  word.setMatch(searchWord.first());
	  found = true;
	  break;
	}
      }
      if(found) {
	break;
      }
    }
  }

  resultTable->setRowCount(searchWords.count());
  for(int a = 0; a < searchWords.count(); ++a) {
    QTableWidgetItem *searchWordItem = new QTableWidgetItem(searchWords.at(a).first());
    int numMatches = 0;
    for(const auto &word: pdfWords) {
      if(word.getMatch() == searchWords.at(a).first()) {
	numMatches++;
      }
    }
    QTableWidgetItem *numMatchesItem = new QTableWidgetItem(QString::number(numMatches));
    resultTable->setItem(a, 0, searchWordItem);
    resultTable->setItem(a, 1, numMatchesItem);
  }

  redrawText();
  
  return true;
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
    percentage = (int)(100.0 / (double)s1.length() *
			((double)s1.length() - (double)distance));
  } else {
    percentage = (int)(100.0 / (double)s2.length() *
			((double)s2.length() - (double)distance));
  }

  return percentage;
}

void OcrWidget::wordSelected(int row, int column)
{
  redrawText(searchWords.at(row).first());
}

void OcrWidget::redrawText(const QString &mark)
{
  QString combined = "";
  for(const auto &word: pdfWords) {
    if(!mark.isEmpty() && word.getMatch() == mark) {
      combined.append("<span style=\"color:blue\">");
    }
    combined.append(word.getChars());
    if(!mark.isEmpty() && word.getMatch() == mark) {
      combined.append("</span>");
    }
  }
  ocrTextEdit->setHtml(combined);
}
