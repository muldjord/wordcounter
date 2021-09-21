/***************************************************************************
 *            ocrworker.cpp
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
#include <poppler-qt5.h>

#include "ocrworker.h"

OcrWorker::OcrWorker(QSharedPointer<Queue> queue) : queue(queue)
{
  settings = new QSettings("config.ini", QSettings::IniFormat);

  tesser = new tesseract::TessBaseAPI();
  if(tesser->Init(nullptr,
		  settings->value("Tesseract/lang", "dan").toString().toStdString().c_str())) {
    qFatal("Could not initialize tesseract engine!\n");
    exit(1);
  }
}

OcrWorker::~OcrWorker()
{
  delete settings;
  delete tesser;
}

void OcrWorker::run()
{
  while(queue->hasEntry()) {
    QPair<QImage, int> page = queue->takeEntry();
    tesser->SetImage(page.first.constBits(), page.first.width(), page.first.height(), 4, 4 * page.first.width());
    QString finalText = QString::fromUtf8(tesser->GetUTF8Text()).trimmed();
    if(finalText.right(1) == "-") {
      finalText = finalText.left(finalText.length() - 1);
    }
    emit entryReady(finalText, page.second);
  }
  emit allDone();
}
