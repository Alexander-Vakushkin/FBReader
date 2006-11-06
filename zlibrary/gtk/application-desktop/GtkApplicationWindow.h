/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __GTKAPPLICATIONWINDOW_H__
#define __GTKAPPLICATIONWINDOW_H__

#include <map>

#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include <ZLApplication.h>

class GtkApplicationWindow : public ZLApplicationWindow { 

public:
	GtkApplicationWindow(ZLApplication *application);
	~GtkApplicationWindow();

private:
	ZLViewWidget *createViewWidget();
	void addToolbarItem(ZLApplication::Toolbar::ItemPtr item);
	void refresh();
	void close();

	bool isFullKeyboardControlSupported() const;
	void grabAllKeys(bool grab);

	bool isFingerTapEventSupported() const;
	bool isMousePresented() const;
	bool isKeyboardPresented() const;

	void setCaption(const std::string &caption) { gtk_window_set_title (myMainWindow, caption.c_str ()); }

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	void setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button);

public:
	void handleKeyEventSlot(GdkEventKey *event);
	void handleScrollEventSlot(GdkEventScroll *event);
	void onGtkButtonPress(GtkWidget *gtkButton);

	GtkWindow *getMainWindow() { return myMainWindow; }

private:
	ZLIntegerRangeOption myWidthOption;
	ZLIntegerRangeOption myHeightOption;

	GtkWindow *myMainWindow;
	GtkWidget *myToolbar;
	GtkWidget *myVBox;

	std::map<const ZLApplication::Toolbar::Item*,GtkWidget*> myButtonToWidget;
	std::map<GtkWidget*,ZLApplication::Toolbar::ItemPtr> myWidgetToButton;

	bool myFullScreen;
};

#endif /* __GTKAPPLICATIONWINDOW_H__ */

// vim:ts=2:sw=2:noet
