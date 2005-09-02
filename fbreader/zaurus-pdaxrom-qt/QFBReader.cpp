/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <qapplication.h>
#include <qpixmap.h>
#include <qmenubar.h>

#include <qlayout.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qdialog.h>

#include <abstract/ZLOptions.h>

#include <qt/QViewWidget.h>
#include <qt/QPaintContext.h>

#include "../common/description/BookDescription.h"
#include "../common/fbreader/BookTextView.h"
#include "../common/fbreader/FootnoteView.h"
#include "../common/fbreader/ContentsView.h"
#include "../common/fbreader/CollectionView.h"
#include "QFBReader.h"

static ZLIntegerOption Width("Options", "Width", 350);
static ZLIntegerOption Height("Options", "Height", 350);

QFBReader::QFBReader(const std::string& bookToOpen) : FBReader(new QPaintContext(), bookToOpen) {
	setWFlags(getWFlags() | WStyle_Customize);

	myViewWidget = new QViewWidget(this, this);
	setCentralWidget((QViewWidget*)myViewWidget);

	myKeyBindings[Key_L] = ACTION_SHOW_COLLECTION;
	myKeyBindings[Key_Z] = ACTION_SHOW_LAST_BOOKS;
	myKeyBindings[Key_C] = ACTION_SHOW_CONTENTS;
	myKeyBindings[Key_F] = ACTION_SEARCH;
	myKeyBindings[Key_N] = ACTION_FIND_NEXT;
	myKeyBindings[Key_P] = ACTION_FIND_PREVIOUS;
	myKeyBindings[Key_O] = ACTION_SHOW_OPTIONS;
	myKeyBindings[Key_I] = ACTION_SHOW_BOOK_INFO;
	myKeyBindings[Key_D] = ACTION_SHOW_HIDE_POSITION_INDICATOR;
	myKeyBindings[Key_R] = ACTION_ROTATE_SCREEN;
	myKeyBindings[Key_A] = ACTION_ADD_BOOK;
	myKeyBindings[Key_1] = ACTION_DECREASE_FONT;
	myKeyBindings[Key_2] = ACTION_INCREASE_FONT;
	myKeyBindings[Key_Left] = ACTION_UNDO;
	myKeyBindings[Key_Right] = ACTION_REDO;
	myKeyBindings[Key_Up] = ACTION_LARGE_SCROLL_BACKWARD;
	myKeyBindings[Key_Down] = ACTION_LARGE_SCROLL_FORWARD;
	myKeyBindings[Key_Escape] = ACTION_CANCEL;
	myKeyBindings[Key_Return] = ACTION_TOGGLE_FULLSCREEN;

	createToolbar();
	connect(menuBar(), SIGNAL(activated(int)), this, SLOT(doActionSlot(int)));

	myFullScreen = false;

	resize(Width.value(), Height.value());

	setMode(BOOK_TEXT_MODE);
}

QFBReader::~QFBReader() {
	Width.setValue(width());
	Height.setValue(height());
}

void QFBReader::keyPressEvent(QKeyEvent *event) {
	std::map<int,ActionCode>::const_iterator it = myKeyBindings.find(event->key());
	if (it != myKeyBindings.end()) {
		doAction(it->second);
	}
}

void QFBReader::toggleFullscreenSlot() {
	myFullScreen = !myFullScreen;
	if (myFullScreen) {
		myWasMaximized = isMaximized();
		menuBar()->hide();
		showFullScreen();
	} else {
		menuBar()->show();
		showNormal();
		if (myWasMaximized) {
			showMaximized();
		}
	}
}

bool QFBReader::isFullscreen() const {
	return myFullScreen;
}

void QFBReader::quitSlot() {
	close();
}

void QFBReader::closeEvent(QCloseEvent *event) {
	if (myMode != BOOK_TEXT_MODE) {
		restorePreviousMode();
		event->ignore();
	} else {
		event->accept();
	}
}

void QFBReader::addButton(ActionCode id, const std::string &name) {
	menuBar()->insertItem(QPixmap((ImageDirectory + "/FBReader/" + name + ".png").c_str()), this, SLOT(emptySlot()), 0, id);
}

void QFBReader::setButtonVisible(ActionCode id, bool visible) {
	if (menuBar()->findItem(id) != 0) {
		menuBar()->setItemVisible(id, visible);
	}
}

void QFBReader::setButtonEnabled(ActionCode id, bool enable) {
	if (menuBar()->findItem(id) != 0) {
		menuBar()->setItemEnabled(id, enable);
	}
}

void QFBReader::searchSlot() {
	QDialog findDialog(this, 0, true);
	findDialog.setCaption("Text search");
	findDialog.setSizeGripEnabled(true);
    
	QGridLayout *layout = new QGridLayout(&findDialog, -1, 3, 5, 5);

	QLineEdit *wordToSearch = new QLineEdit(&findDialog);
	wordToSearch->setMinimumWidth(width() / 3);
	wordToSearch->setText(QString::fromUtf8(SearchPatternOption.value().c_str()));
	layout->addMultiCellWidget(wordToSearch, 0, 0, 0, 2);

	QCheckBox *ignoreCase = new QCheckBox("&Ignore case", &findDialog, 0);
	ignoreCase->setChecked(SearchIgnoreCaseOption.value());
	layout->addMultiCellWidget(ignoreCase, 1, 1, 0, 2);

	QCheckBox *wholeText = new QCheckBox("In w&hole text", &findDialog, 0);
	wholeText->setChecked(SearchInWholeTextOption.value());
	layout->addMultiCellWidget(wholeText, 2, 2, 0, 2);

	QCheckBox *backward = new QCheckBox("&Backward", &findDialog, 0);
	backward->setChecked(SearchBackwardOption.value());
	layout->addMultiCellWidget(backward, 3, 3, 0, 2);

	QCheckBox *thisSectionOnly = ((TextView*)myViewWidget->view())->hasMultiSectionModel() ?
		new QCheckBox("&This section only", &findDialog, 0) : 0;
	if (thisSectionOnly != 0) {
		thisSectionOnly->setChecked(SearchThisSectionOnlyOption.value());
		layout->addMultiCellWidget(thisSectionOnly, 4, 4, 0, 2);
	}

	QPushButton *b = new QPushButton("&Go!", &findDialog);
	layout->addWidget(b, 5, 1);
	b->setDefault(true);
	connect(b, SIGNAL(clicked()), &findDialog, SLOT(accept()));

	if (findDialog.exec()) {
		QString qPattern = wordToSearch->text().stripWhiteSpace();
		std::string pattern = (const char*)qPattern.utf8();
		SearchPatternOption.setValue(pattern);
		SearchIgnoreCaseOption.setValue(ignoreCase->isChecked());
		SearchInWholeTextOption.setValue(wholeText->isChecked());
		SearchBackwardOption.setValue(backward->isChecked());
		if (thisSectionOnly != 0) {
			SearchThisSectionOnlyOption.setValue(thisSectionOnly->isChecked());
		}
		((TextView*)myViewWidget->view())->search(
			pattern, ignoreCase->isChecked(), wholeText->isChecked(), backward->isChecked(), SearchThisSectionOnlyOption.value()
		);
	}
}

void QFBReader::setWindowCaption(const std::string &caption) {
	QString qCaption = QString::fromUtf8(caption.c_str());
	if (qCaption.length() > 60) {
		qCaption = qCaption.left(57) + "...";
	}
	setCaption(qCaption);
}

void QFBReader::doActionSlot(int buttonNumber) {
	doAction((ActionCode)buttonNumber);
}
