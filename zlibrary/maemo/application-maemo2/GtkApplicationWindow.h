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
#include <stack>

#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include <hildon-widgets/hildon-program.h>
#include <libosso.h>

#include <ZLApplication.h>

class GtkApplicationWindow : public ZLApplicationWindow { 

public:
	GtkApplicationWindow(ZLApplication *application);
	~GtkApplicationWindow();

private:
	ZLViewWidget *createViewWidget();
	void addToolbarItem(ZLApplication::Toolbar::ItemPtr item);

	class MenuBuilder : public ZLApplication::MenuVisitor {

	public:
		MenuBuilder(GtkApplicationWindow &window);

	private:
		void processSubmenuBeforeItems(ZLApplication::Menubar::Submenu &submenu);
		void processSubmenuAfterItems(ZLApplication::Menubar::Submenu &submenu);
		void processItem(ZLApplication::Menubar::PlainItem &item);
		void processSepartor(ZLApplication::Menubar::Separator &separator);

	private:
		GtkApplicationWindow &myWindow;
		std::stack<GtkMenu*> myMenuStack;
	};
	void initMenu();
	void refresh();
	void close();

	void setCaption(const std::string &caption) { gtk_window_set_title(GTK_WINDOW(myWindow), caption.c_str()); }

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	bool isFullKeyboardControlSupported() const;
	void grabAllKeys(bool grab);

	bool isFingerTapEventSupported() const;
	bool isMousePresented() const;
	bool isKeyboardPresented() const;

	void setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button);

public:
	void handleKeyEventSlot(GdkEventKey*);
	HildonWindow *getMainWindow() const { return myWindow; }

public:
	class ToolbarButton {

	public:
		void press(bool state);

	private:
		ToolbarButton(ZLApplication::Toolbar::ButtonItem &buttonItem, GtkApplicationWindow &window);
		void forcePress(bool state);
		GtkToolItem *toolItem() const { return myToolItem; }

	private:
		ZLApplication::Toolbar::ButtonItem &myButtonItem;
		GtkApplicationWindow &myWindow;
		shared_ptr<ZLApplication::Action> myAction;

		GtkToolItem *myToolItem;
		GtkWidget *myEventBox;
		GtkImage *myCurrentImage;
		GtkImage *myReleasedImage;
		GtkImage *myPressedImage;

	friend class GtkApplicationWindow;
	};

private:
	HildonProgram *myProgram;
	HildonWindow *myWindow;
	GtkToolbar *myToolbar;
	GtkMenu *myMenu;

	bool myFullScreen;

	std::map<ZLApplication::Toolbar::ItemPtr,GtkToolItem*> myToolItems;
	std::map<int,GtkMenuItem*> myMenuItems;
	std::map<const ZLApplication::Toolbar::ButtonItem*,ToolbarButton*> myToolbarButtons;

friend class MenuBuilder;
friend class ToolbarButton;
};

#endif /* __GTKAPPLICATIONWINDOW_H__ */

// vim:ts=2:sw=2:noet
