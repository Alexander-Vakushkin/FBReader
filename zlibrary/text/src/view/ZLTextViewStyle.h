/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __ZLTEXTVIEWSTYLE_H__
#define __ZLTEXTVIEWSTYLE_H__

#include <ZLTextParagraphCursor.h>

class ZLTextView;
class ZLTextStyle;

class ZLTextViewStyle {

public:
	ZLTextViewStyle(const ZLTextView &view);

	void setTextStyle(const shared_ptr<ZLTextStyle> style, unsigned char bidiLevel);

private:
	void reset() const;

	void applyControl(const ZLTextControlElement &control);
	void applyControl(const ZLTextStyleElement &control);
	void increaseBidiLevel();
	void decreaseBidiLevel();

public:
	void applySingleControl(const ZLTextElement &element);
	void applyControls(const ZLTextWordCursor &begin, const ZLTextWordCursor &end);

	shared_ptr<ZLTextStyle> textStyle() const;
	int elementWidth(const ZLTextElement &element, unsigned int charNumber, const ZLTextStyleEntry::Metrics &metrics) const;
	int elementHeight(const ZLTextElement &element, const ZLTextStyleEntry::Metrics &metrics) const;
	int elementDescent(const ZLTextElement &element) const;

	int wordWidth(const ZLTextWord &word, int start = 0, int length = -1, bool addHyphenationSign = false) const;

	unsigned char bidiLevel() const;

private:
	const ZLTextView &myView;
	mutable shared_ptr<ZLTextStyle> myTextStyle;
	mutable unsigned char myBidiLevel;
	mutable int myWordHeight;
};

inline unsigned char ZLTextViewStyle::bidiLevel() const { return myBidiLevel; }
inline shared_ptr<ZLTextStyle> ZLTextViewStyle::textStyle() const { return myTextStyle; }

#endif /* __ZLTEXTVIEWSTYLE_H__ */