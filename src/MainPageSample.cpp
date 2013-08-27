//============================================================================
// Copyright [2013] <COS>
// Name        : MainPage.cpp
// Author      : Rex Si
// Description : Example Application of COS Project
//============================================================================
#define DEBUG_LEVEL 0
#define LOG_TAG "PullToRefresh"

#include <gaia/base/gloger.h>
#include <gaia/base/String.h>
#include <gaia/core/Context.h>
#include <gaia/core/Persistence.h>
#include <gaia/core/vision/Scene.h>
#include <gaia/locale/Date.h>
#include <gaia/ui/LinearController.h>
#include <gaia/ui/TextWidget.h>
#include "PullToRefresh/R.h"

#include "ListProvider.h"
#include "PullToRefreshListWidget.h"
#include "MainPageSample.h"

using namespace gaia::base;
using namespace gaia::core;
using namespace gaia::locale;
using namespace gaia::ui;

MainPageSample::MainPageSample()
        : mpList(NULL),
                mpProvider(NULL){
}

MainPageSample::~MainPageSample() {
}

void MainPageSample::onInit(Persistence* const p) {
    GLOGENTRY(LOG_TAG);
    Scene* pScene = Scene::SceneFactory(this);

    mpProvider = new ListProvider(this);

    mpList = new PullToRefreshListWidget(this);
    mpList->setProvider(mpProvider);
    mpList->setOnRefreshListener(this);

    pScene->attachController(mpList);
    pScene->setupSceneLayout();
}

void MainPageSample::onTear() {
    GLOGENTRY(LOG_TAG);
    delete mpList;
    delete mpProvider;
    getSceneSingleton()->SceneDestroy();
}

void MainPageSample::onRefresh() {
    GLOGENTRY(LOG_TAG);
//    mpList->onRefreshComplete(String::format("last update: %s", Date().toLocaleString().string()));
}


template class Export<MainPageSample, Page> ;

