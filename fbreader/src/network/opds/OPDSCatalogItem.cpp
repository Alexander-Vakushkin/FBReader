/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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

#include <ZLExecutionData.h>
#include <ZLNetworkManager.h>

#include "OPDSCatalogItem.h"
#include "OPDSLink.h"
#include "OPDSXMLParser.h"
#include "NetworkOPDSFeedReader.h"

#include "../NetworkOperationData.h"

OPDSCatalogItem::OPDSCatalogItem(
	const OPDSLink &link,
	const std::string &title,
	const std::string &summary,
	const UrlInfoCollection &urlByType,
	AccessibilityType accessibility,
	int flags
) : NetworkCatalogItem(link, title, summary, urlByType, accessibility, flags), myData(Link) {
}

class OPDSCatalogItemScope : public ZLExecutionScope {
public:
	shared_ptr<ZLExecutionData> networkData;
	shared_ptr<ZLExecutionData::Listener> listener;
};


std::string OPDSCatalogItem::loadChildren(NetworkItem::List &children, shared_ptr<ZLExecutionData::Listener> listener) {

//	NetworkOperationData data(Link);

//	shared_ptr<ZLExecutionData> networkData = ((OPDSLink&)Link).createNetworkData(getCatalogUrl(), data);

//	while (!networkData.isNull()) {
//		networkData->setListener(listener);
//		std::string error = ZLNetworkManager::Instance().perform(networkData);
//		if (!error.empty()) {
//			return error;
//		}

//		children.insert(children.end(), data.Items.begin(), data.Items.end());
//		networkData = data.resume();
//	}


	shared_ptr<ZLExecutionData> networkData = ((OPDSLink&)Link).createNetworkData(getCatalogUrl(), myData);

	OPDSCatalogItemScope *scope = new OPDSCatalogItemScope;
	scope->networkData = networkData;
	scope->listener = listener;

	if(!networkData.isNull()) {
		networkData->setListener(ZLExecutionUtil::createListener(scope, this, &OPDSCatalogItem::onLoadedChildren));
		ZLNetworkManager::Instance().perform(networkData);
	}
	return "";
}

void OPDSCatalogItem::onLoadedChildren(ZLExecutionScope &s, std::string error) {
	OPDSCatalogItemScope &scope = static_cast<OPDSCatalogItemScope&>(s);
	if (!error.empty()) {
		myData.clear();
		scope.listener->finished(error);
		return;
	}

	scope.networkData = myData.resume();
	if (!scope.networkData.isNull()) {
		scope.networkData->setListener(ZLExecutionUtil::createListener(&scope, this, &OPDSCatalogItem::onLoadedChildren));
		ZLNetworkManager::Instance().perform(scope.networkData);
	} else {
		myData.clear();
		scope.listener->finished();
	}
}
