//============================================================================
// Copyright [2013] <COS>
// Name        : MainPage.h
// Author      : Rex Si
// Description : Example Application of COS Project
//============================================================================

#pragma once
#include <gaia/core/Page.h>

class PullToRefreshListWidget;
class Refresh;
class MainPageSample: public gaia::core::Page, public OnRefreshListener {
public:
    MainPageSample();
    ~MainPageSample();
    void onRefresh();

protected:
    virtual void onInit(gaia::core::Persistence* const p);
    virtual void onTear();

private:
    PullToRefreshListWidget* mpList;
    gaia::ui::AbsListItemProvider* mpProvider;
};

