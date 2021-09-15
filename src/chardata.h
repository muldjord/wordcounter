/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            chardata.h
 *
 *  Wed Sep 8 12:00:00 CEST 2021
 *  Copyright 2021 Lars Bisballe Jensen
 *  larsbjensen@gmail.com
 ****************************************************************************/
/*
 *  This file is part of Wordcounter.
 *
 *  Wordcounter is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Wordcounter is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Wordcounter; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#ifndef __CHARDATA_H__
#define __CHARDATA_H__

#include <QObject>

class CharData : public QObject
{
Q_OBJECT

public:
  CharData(const CharData &data);
  CharData(const bool &isWord,
           const QString &chars);
  bool isWord() const;
  bool setChars(const QString &value);
  bool setMatch(const QString &value);
  QString getChars() const;
  QString getMatch() const;
  
private:
  bool word = false;
  QString chars = QString();
  QString match = QString();
};

#endif // __CHARDATA_H__
