/***************************************************************************
 *            lineedit.cpp
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

#define DEBUG

#include <stdio.h>
#include <QSettings>

#include "lineedit.h"

extern QSettings *settings;

LineEdit::LineEdit(QString group, QString name, QString stdValue, const bool &text)
  : group(group), defaultValue(stdValue), isText(text)
{
  this->name = (group != "General"?group + "/":"") + name;

  if(!settings->contains(name)) {
    settings->setValue(name, stdValue);
  }
  setText(settings->value(this->name, stdValue).toString());

  connect(this, &QLineEdit::editingFinished, this, &LineEdit::saveToConfig);
  connect(this, &QLineEdit::textChanged, this, &LineEdit::saveToConfig);
}

LineEdit::~LineEdit()
{
}

void LineEdit::resetToDefault()
{
  setText(defaultValue);
}

void LineEdit::saveToConfig()
{
  if(!isText && text().contains(",")) {
    setText(text().replace(",", "."));
  }
  settings->setValue(name, this->text());

  qDebug("Key '%s' saved to config with value '%s'\n", name.toStdString().c_str(), this->text().toStdString().c_str());
}
