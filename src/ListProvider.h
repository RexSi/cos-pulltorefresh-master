/*
 * ListProvider.h
 *
 *  Created on: Jul 24, 2013
 *      Author: Rex Si
 */

#pragma once

#include <gaia/ui/AbsListItemProvider.h>

class ListProvider: public gaia::ui::AbsListItemProvider {
public:
    explicit ListProvider(gaia::core::Page* page);
    virtual ~ListProvider();
    virtual gaia::core::Widget *createItemWidget(int32_t position, gaia::core::Widget *pParent);
    virtual gaia::core::Widget *getItemWidget(int32_t position, gaia::core::Widget *pItemWidget, gaia::core::Widget *pParent);
    virtual int32_t getItemWidgetType(int32_t position) const;
    virtual int32_t getWidgetTypeCount() const;
    virtual bool hasStableIds() const;
    virtual int32_t getCount() const;
    virtual bool isEmpty() const;
    virtual int64_t getItemId(int32_t position) const;
    virtual bool areAllItemsEnabled() const;
    virtual bool isEnabled(int32_t position) const;

    enum {
        ID_TEXTWIDGET,
        ID_BUTTON,
    };
private:
    gaia::core::Page* mpPage;
    gaia::base::Vector<gaia::base::String> mListData;
    gaia::base::Vector<gaia::core::Widget*> mWidgetPool;
};

