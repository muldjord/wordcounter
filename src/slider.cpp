/***************************************************************************
 *            slider.cpp
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
#include <QHBoxLayout>

#include "slider.h"

extern QSettings *settings;

Slider::Slider(const QString &group, const QString &name, const int &stdValue,
	       const int &maxValue, const int &minValue)
  : group(group), defaultValue(stdValue)
{
  this->name = (group != "General"?group + "/":"") + name;

  if(!settings->contains(name)) {
    settings->setValue(name, stdValue);
  }

  slider = new QSlider(this);
  slider->setOrientation(Qt::Horizontal);
  slider->setMinimum(minValue);
  slider->setMaximum(maxValue);
  slider->setValue(settings->value(this->name, stdValue).toInt());
  connect(slider, &QSlider::sliderReleased, this, &Slider::saveToConfig);
  connect(slider, &QSlider::valueChanged, this, &Slider::setValueText);

  valueText = new QLineEdit(this);
  valueText->setMaximumWidth(25);
  valueText->setText(QString::number(slider->value()));
  
  QHBoxLayout *layout = new QHBoxLayout();
  layout->addWidget(valueText);
  layout->addWidget(slider);
  
  setLayout(layout);
}

Slider::~Slider()
{
}

void Slider::resetToDefault()
{
  slider->setValue(defaultValue);
}

void Slider::saveToConfig()
{
  settings->setValue(name, slider->value());
  qDebug("Key '%s' saved to config with value '%d'\n", name.toStdString().c_str(), slider->value());
}

void Slider::setValueText(int value)
{
  valueText->setText(QString::number(value));
}
