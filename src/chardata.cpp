/***************************************************************************
 *            chardata.cpp
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

#include "chardata.h"

CharData::CharData(const CharData &data) : QObject()
{
  word = data.word;
  chars = data.chars;
  match = data.match;
}

CharData::CharData(const bool &word, const QString &chars) :
  word(word), chars(chars)
{
}

// Setter functions
bool CharData::setChars(const QString &value)
{
  chars = value;
  return true;
}

bool CharData::setMatch(const QString &value)
{
  match = value;
  return true;
}

// Getter functions
bool CharData::isWord() const
{
  return word;
}

QString CharData::getChars() const
{
  return chars;
}

QString CharData::getMatch() const
{
  return match;
}
