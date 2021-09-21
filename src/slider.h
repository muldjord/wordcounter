/***************************************************************************
 *            slider.h
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

#ifndef SLIDER_H
#define SLIDER_H

#include <QSlider>
#include <QLineEdit>

class Slider : public QWidget
{
  Q_OBJECT
    
public:
  Slider(const QString &group, const QString &name, const int &stdValue, const int &maxValue, const int &minValue = 0);
  ~Slider();
  
public slots:
  void resetToDefault();

protected:
  
private slots:
  void saveToConfig();
  void setValueText(int value);

private:
  QSlider *slider;
  QLineEdit *valueText;
  QString name;
  QString group;
  int defaultValue;
};

#endif
