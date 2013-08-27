/*
 * ListProvider.cpp
 *
 *  Created on: Jul 24, 2013
 *      Author: Rex Si
 */

#include <gaia/base/String.h>
#include <gaia/base/Vector.h>
#include <gaia/core/vision/Widget.h>
#include <gaia/ui/Button.h>
#include <gaia/ui/RelativeController.h>
#include <gaia/ui/RelativeControllerParams.h>
#include <gaia/ui/TextWidget.h>

#include "ListProvider.h"

using namespace gaia::base;
using namespace gaia::core;
using namespace gaia::ui;

ListProvider::ListProvider(Page* page): mpPage(page) {
    for (size_t i = 0; i < 20; i++) {
        mListData.append(String::valueOf(i << 10));
    }
}

ListProvider::~ListProvider() {
}

Widget *ListProvider::createItemWidget(int32_t position, Widget *pParent) {

    RelativeController* lc = new RelativeController(mpPage);
    mWidgetPool.append(lc);
    RelativeControllerParams params(ControllerParams::MATCH_PARENT, 150);
    lc->setControllerParams(&params);

    TextWidget* tw = new TextWidget(mpPage);
    mWidgetPool.append(tw);
    RelativeControllerParams twParams(ControllerParams::WRAP_CONTENT, ControllerParams::WRAP_CONTENT);
    twParams.setRelation(RelativeControllerParams::ALIGN_PARENT_LEFT);
    twParams.setRelation(RelativeControllerParams::CENTER_VERTICAL);
    tw->setControllerParams(&twParams);
    tw->setId(ID_TEXTWIDGET);
    lc->addWidget(tw);

    Button* bt = new Button(mpPage);
    mWidgetPool.append(bt);
    RelativeControllerParams btParams(ControllerParams::WRAP_CONTENT, ControllerParams::WRAP_CONTENT);
    btParams.setRelation(RelativeControllerParams::ALIGN_PARENT_RIGHT);
    btParams.setRelation(RelativeControllerParams::CENTER_VERTICAL);
    bt->setControllerParams(&btParams);
    bt->setId(ID_BUTTON);
    bt->setFocusable(false);
    lc->addWidget(bt);

    return lc;
}

Widget *ListProvider::getItemWidget(int32_t position, Widget *pItemWidget, Widget *pParent) {
    TextWidget* tw = dynamic_cast<TextWidget*> (pItemWidget->findWidgetById(ID_TEXTWIDGET));
    tw->setText(mListData.itemAt(position));

    Button* bt = dynamic_cast<Button*> (pItemWidget->findWidgetById(ID_BUTTON));
    bt->setText(mListData.itemAt(position));

    return pItemWidget;
}

int32_t ListProvider::getItemWidgetType(int32_t position) const {
    return 1;
}

int32_t ListProvider::getWidgetTypeCount() const {
    return 1;
}

bool ListProvider::hasStableIds() const {
    return true;
}

int32_t ListProvider::getCount() const {
    return mListData.size();
}

bool ListProvider::isEmpty() const {
    return mListData.size() <= 0;
}

int64_t ListProvider::getItemId(int32_t position) const {
    return position;
}

bool ListProvider::areAllItemsEnabled() const {
    return true;
}

bool ListProvider::isEnabled(int32_t position) const {
    return true;
}
