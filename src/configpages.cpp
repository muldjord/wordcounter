/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            configpages.cpp
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

#include "configpages.h"
#include "lineedit.h"
#include "combobox.h"
#include "checkbox.h"
#include "slider.h"

extern QSettings *settings;

GeneralPage::GeneralPage(QWidget *parent) : QWidget(parent)
{
  QPushButton *resetButton = new QPushButton(tr("Reset all to defaults"));

  CheckBox *alwaysOnTopCheckBox = new CheckBox(tr("Always keep window on top"), "General",
                                               "alwaysOnTop", false);
  connect(resetButton, &QPushButton::clicked, alwaysOnTopCheckBox, &CheckBox::resetToDefault);

  CheckBox *alwaysMaximizeCheckBox = new CheckBox(tr("Always maximize window on startup"), "General", "alwaysMaximize", true);
  connect(resetButton, &QPushButton::clicked, alwaysMaximizeCheckBox, &CheckBox::resetToDefault);

  /*
  CheckBox *removeFromListCheckBox = new CheckBox(tr("Remove each item from input list after processing"), "General", "removeFromList", false);
  connect(resetButton, &QPushButton::clicked, removeFromListCheckBox, &CheckBox::resetToDefault);
  */

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(resetButton);
  layout->addWidget(alwaysOnTopCheckBox);
  layout->addWidget(alwaysMaximizeCheckBox);
  layout->addStretch();
  setLayout(layout);
}

MatchingPage::MatchingPage(QWidget *parent) : QWidget(parent)
{
  QPushButton *resetButton = new QPushButton(tr("Reset all to defaults"));

  CheckBox *checkForTextCheckBox = new CheckBox(tr("Use actual document text if any exists before falling back on OCR"), "Matching", "checkForText", true);
  connect(resetButton, &QPushButton::clicked, checkForTextCheckBox, &CheckBox::resetToDefault);

  QLabel *tesseractLangLabel = new QLabel(tr("Tesseract OCR engine language (ex. 'dan' or 'eng'):"));
  LineEdit *tesseractLangLineEdit = new LineEdit("Tesseract", "lang", "dan");
  connect(resetButton, &QPushButton::clicked, tesseractLangLineEdit, &LineEdit::resetToDefault);

  QLabel *maxThreadsLabel = new QLabel(tr("Max number of threads to use when doing OCR processing:"));
  Slider *maxThreadsSlider = new Slider("Tesseract", "maxThreads", 4, 8);
  connect(resetButton, &QPushButton::clicked, maxThreadsSlider, &Slider::resetToDefault);
  
  QLabel *pdfImageDpiLabel = new QLabel(tr("DPI to use when rendering PDF to image for use with OCR:"));
  LineEdit *pdfImageDpiLineEdit = new LineEdit("Matching", "pdfImageDpi", "300");
  connect(resetButton, &QPushButton::clicked, pdfImageDpiLineEdit, &LineEdit::resetToDefault);

  QLabel *minMatchPercentageLabel = new QLabel(tr("Minimum accepted percentage word match:"));
  Slider *minMatchPercentageSlider = new Slider("Matching", "minMatchPercentage", 80, 100, 40);
  connect(resetButton, &QPushButton::clicked, minMatchPercentageSlider, &Slider::resetToDefault);

  QLabel *matchColorLabel = new QLabel(tr("Matched word color in hex (eg. 'ff0000'):"));
  LineEdit *matchColorLineEdit = new LineEdit("Matching", "matchedWordColor", "ff0000");
  connect(resetButton, &QPushButton::clicked, matchColorLineEdit, &LineEdit::resetToDefault);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(resetButton);
  layout->addWidget(pdfImageDpiLabel);
  layout->addWidget(pdfImageDpiLineEdit);
  layout->addWidget(tesseractLangLabel);
  layout->addWidget(tesseractLangLineEdit);
  layout->addWidget(maxThreadsLabel);
  layout->addWidget(maxThreadsSlider);
  //layout->addWidget(checkForTextCheckBox);
  layout->addWidget(minMatchPercentageLabel);
  layout->addWidget(minMatchPercentageSlider);
  layout->addWidget(matchColorLabel);
  layout->addWidget(matchColorLineEdit);
  layout->addStretch();
  setLayout(layout);
}

OutputPage::OutputPage(QWidget *parent) : QWidget(parent)
{
  QPushButton *resetButton = new QPushButton(tr("Reset all to defaults"));

  QLabel *delimiterLabel = new QLabel(tr("Delimiter:"));
  ComboBox *delimiterComboBox = new ComboBox("Output", "delimiter", "tab");
  delimiterComboBox->addConfigItem("Tab", "tab");
  delimiterComboBox->addConfigItem("Semicolon", "semicolon");
  delimiterComboBox->addConfigItem("Comma", "comma");
  delimiterComboBox->setFromConfig();
  connect(resetButton, &QPushButton::clicked, delimiterComboBox, &ComboBox::resetToDefault);

  /*
  QLabel *distanceUnitLabel = new QLabel(tr("Distance unit:"));
  LineEdit *distanceUnitLineEdit = new LineEdit("Output", "distanceUnit", "??m");
  connect(resetButton, &QPushButton::clicked, distanceUnitLineEdit, &LineEdit::resetToDefault);

  QLabel *timeUnitLabel = new QLabel(tr("Time unit:"));
  LineEdit *timeUnitLineEdit = new LineEdit("Output", "timeUnit", "s");
  connect(resetButton, &QPushButton::clicked, timeUnitLineEdit, &LineEdit::resetToDefault);
  */
  
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(resetButton);
  layout->addWidget(delimiterLabel);
  layout->addWidget(delimiterComboBox);
  /*
  layout->addWidget(distanceUnitLabel);
  layout->addWidget(distanceUnitLineEdit);
  layout->addWidget(timeUnitLabel);
  layout->addWidget(timeUnitLineEdit);
  */
  layout->addStretch();
  setLayout(layout);
}
