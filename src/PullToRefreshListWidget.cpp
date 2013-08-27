/*
 * PullToRefreshListWidget.cpp
 *
 *  Created on: Aug 26, 2013
 *      Author: Rex Si
 */

#define DEBUG_LEVEL 0
#define LOG_TAG "PullToRefresh"

#include <com/gaia/common/R.h>
#include <gaia/base/gloger.h>
#include <gaia/base/String.h>
#include <gaia/content/res/Resources.h>
#include <gaia/core/Context.h>
#include <gaia/core/animation/LinearInterpolator.h>
#include <gaia/core/animation/RotateAnimation.h>
#include <gaia/core/event/MotionEvent.h>
#include <gaia/core/util/DisplayMetrics.h>
#include <gaia/core/vision/Gravity.h>
#include <gaia/core/vision/LayoutInflater.h>
#include <gaia/core/vision/MeasureSpec.h>
#include <gaia/graphics/Typeface.h>
#include <gaia/ui/ControllerParams.h>
#include <gaia/ui/ImageWidget.h>
#include <gaia/ui/ProgressBar.h>
#include <gaia/ui/RelativeController.h>
#include <gaia/ui/RelativeControllerParams.h>
#include <gaia/ui/TextWidget.h>
#include "PullToRefresh/R.h"

#include "PullToRefreshListWidget.h"

using namespace gaia::base;
using namespace gaia::content;
using namespace gaia::core;
using namespace gaia::graphics;
using namespace gaia::ui;

PullToRefreshListWidget::PullToRefreshListWidget(Page* page)
        : ListWidget(page), mpOnRefreshListener(NULL), mpOnScrollListener(NULL), mpRefreshController(NULL), mpRefreshText(NULL), mpRefreshImage(
                NULL), mpRefreshProgress(NULL), mpRefreshLastUpdated(NULL), mCurrentScrollState(NULL), mRefreshState(NULL), mpFlipAnimation(NULL), mpReverseFlipAnimation(
                NULL), mRefreshWidgetHeight(0), mRefreshOriginalTopPadding(0), mLastMotionY(0), mBounceHack(false) {
    init(Context(page), NULL);
}

PullToRefreshListWidget::PullToRefreshListWidget(Context* context)
        : ListWidget(context), mpOnRefreshListener(NULL), mpOnScrollListener(NULL), mpRefreshController(NULL), mpRefreshText(NULL), mpRefreshImage(
                NULL), mpRefreshProgress(NULL), mpRefreshLastUpdated(NULL), mCurrentScrollState(NULL), mRefreshState(NULL), mpFlipAnimation(NULL), mpReverseFlipAnimation(
                NULL), mRefreshWidgetHeight(0), mRefreshOriginalTopPadding(0), mLastMotionY(0), mBounceHack(false) {
    init(*context, NULL);
}

PullToRefreshListWidget::PullToRefreshListWidget(Context* context, AttributeSet* attrs)
        : ListWidget(context, attrs), mpOnRefreshListener(NULL), mpOnScrollListener(NULL), mpRefreshController(NULL), mpRefreshText(NULL), mpRefreshImage(
                NULL), mpRefreshProgress(NULL), mpRefreshLastUpdated(NULL), mCurrentScrollState(NULL), mRefreshState(NULL), mpFlipAnimation(
                NULL), mpReverseFlipAnimation(NULL), mRefreshWidgetHeight(0), mRefreshOriginalTopPadding(0), mLastMotionY(0), mBounceHack(false) {
    init(*context, attrs);
}

PullToRefreshListWidget::PullToRefreshListWidget(Page* page, AttributeSet* attrs, int32_t defStyle)
        : ListWidget(page, attrs, defStyle), mpOnRefreshListener(NULL), mpOnScrollListener(NULL), mpRefreshController(NULL), mpRefreshText(
                NULL), mpRefreshImage(NULL), mpRefreshProgress(NULL), mpRefreshLastUpdated(NULL), mCurrentScrollState(NULL), mRefreshState(
                NULL), mpFlipAnimation(NULL), mpReverseFlipAnimation(NULL), mRefreshWidgetHeight(0), mRefreshOriginalTopPadding(0), mLastMotionY(0), mBounceHack(
                false) {
    init(Context(page), attrs);
}

PullToRefreshListWidget::~PullToRefreshListWidget() {
    delete mpFlipAnimation;
    delete mpReverseFlipAnimation;
    delete mpRefreshController;
    delete mpRefreshText;
    delete mpRefreshImage;
    delete mpRefreshProgress;
    delete mpRefreshLastUpdated;
}

void PullToRefreshListWidget::init(const Context& context, AttributeSet* attrs) {
    // Load all of the animations we need in code rather than through XML

    Resources* pRes = context.getResourcesN();
    float density = pRes->getDisplayMetrics().getDensity();
    delete pRes;

    mpFlipAnimation = new RotateAnimation(0, -180, Animation::RELATIVE_TO_SELF, 0.5f, Animation::RELATIVE_TO_SELF, 0.5f);
    mpFlipAnimation->setInterpolator(&LinearInterpolator());
    mpFlipAnimation->setDuration(250);
    mpFlipAnimation->setFillAfter(true);

    mpReverseFlipAnimation = new RotateAnimation(-180, 0, Animation::RELATIVE_TO_SELF, 0.5f, Animation::RELATIVE_TO_SELF, 0.5f);
    mpReverseFlipAnimation->setInterpolator(&LinearInterpolator());
    mpReverseFlipAnimation->setDuration(250);
    mpReverseFlipAnimation->setFillAfter(true);

    mpRefreshController = new RelativeController(&context, attrs);
    mpRefreshController->setPadding(0, (int32_t) (10 * density + 0.5f), 0, (int32_t) (15 * density + 0.5f));
    mpRefreshController->setGravity(Gravity::CENTER);
    mpRefreshController->setOnClickListener(this);
    RelativeControllerParams rcParams(ControllerParams::MATCH_PARENT, ControllerParams::MATCH_PARENT);
    mpRefreshController->setControllerParams(&rcParams);

    mpRefreshText = new TextWidget(&context, attrs);
    mpRefreshText->setId(pull_to_refresh_text);
    mpRefreshText->setText(PullToRefresh::R::string::pull_to_refresh_tap_label);
    mpRefreshText->setTypeface(&Typeface::DEFAULT_BOLD());
    mpRefreshText->setPadding(0, (int32_t) (5 * density + 0.5f), 0, 0);
    mpRefreshText->setGravity(Gravity::CENTER);
    RelativeControllerParams rtParams(ControllerParams::MATCH_PARENT, ControllerParams::WRAP_CONTENT);
    mpRefreshText->setControllerParams(&rtParams);

    mpRefreshImage = new ImageWidget(&context, attrs);
    mpRefreshImage->setId(pull_to_refresh_image);
    mpRefreshImage->setVisibility(Widget::GONE);
    mpRefreshImage->setMinimumHeight(50);
    mpRefreshImage->setImageResource(PullToRefresh::R::drawable::ic_pulltorefresh_arrow);
    RelativeControllerParams riParams(ControllerParams::WRAP_CONTENT, ControllerParams::WRAP_CONTENT);
    riParams.setMargins((int32_t) (30 * density + 0.5f), 0, (int32_t) (20 * density + 0.5f), 0);
    mpRefreshImage->setControllerParams(&riParams);

    mpRefreshProgress = new ProgressBar(&context, attrs, com::gaia::common::R::attr::progressBarStyleSmall);
    mpRefreshProgress->setId(pull_to_refresh_progress);
    mpRefreshProgress->setIndeterminate(true);
    mpRefreshProgress->setVisibility(Widget::GONE);
    RelativeControllerParams rpParams(ControllerParams::WRAP_CONTENT, ControllerParams::WRAP_CONTENT);
    rpParams.setMargins((int32_t) (30 * density + 0.5f), (int32_t) (10 * density + 0.5f), (int32_t) (20 * density + 0.5f), 0);
    mpRefreshProgress->setControllerParams(&rpParams);

    mpRefreshLastUpdated = new TextWidget(&context, attrs);
    mpRefreshLastUpdated->setId(pull_to_refresh_updated_at);
    mpRefreshLastUpdated->setVisibility(Widget::GONE);
    mpRefreshLastUpdated->setGravity(Gravity::CENTER);
    RelativeControllerParams rluParams(ControllerParams::MATCH_PARENT, ControllerParams::WRAP_CONTENT);
    rluParams.setRelation(RelativeControllerParams::BELOW, pull_to_refresh_text);
    mpRefreshLastUpdated->setControllerParams(&rluParams);

    mpRefreshController->addWidget(mpRefreshProgress);
    mpRefreshController->addWidget(mpRefreshImage);
    mpRefreshController->addWidget(mpRefreshText);
    mpRefreshController->addWidget(mpRefreshLastUpdated);

    mRefreshOriginalTopPadding = mpRefreshController->getPaddingTop();

    mRefreshState = TAP_TO_REFRESH;

    addHeaderView(mpRefreshController);

    ListWidget::setOnScrollListener(this);

    measureView(mpRefreshController);
    mRefreshWidgetHeight = mpRefreshController->getMeasuredHeight();
}

void PullToRefreshListWidget::onAttachedToWindow() {
    ListWidget::onAttachedToWindow();
    setSelection(1);
}

void PullToRefreshListWidget::setProvider(IListItemProvider *pProvider) {
    ListWidget::setProvider(pProvider);
    setSelection(1);
}

void PullToRefreshListWidget::setOnScrollListener(AbsListWidgetOnScrollListener *l) {
    mpOnScrollListener = l;
}

void PullToRefreshListWidget::setOnRefreshListener(OnRefreshListener* onRefreshListener) {
    mpOnRefreshListener = onRefreshListener;
}

void PullToRefreshListWidget::setLastUpdated(const String& lastUpdated) {
    if (!lastUpdated.isEmpty()) {
        mpRefreshLastUpdated->setVisibility(Widget::VISIBLE);
        mpRefreshLastUpdated->setText(lastUpdated);
    } else {
        mpRefreshLastUpdated->setVisibility(Widget::GONE);
    }
}

bool PullToRefreshListWidget::dispatchTouchEvent(const gaia::core::MotionEvent& event) {
    onTouchEvent(&event);
    return ListWidget::dispatchTouchEvent(event);
}

bool PullToRefreshListWidget::onTouchEvent(MotionEvent* event) {
    const int32_t y = (int32_t) event->getY();
    mBounceHack = false;

    switch (event->getAction()) {
    case MotionEvent::ACTION_UP:
        if (!isVerticalScrollBarHidden()) {
            setVerticalScrollBarEnabled(true);
        }
        if (getFirstVisiblePosition() == 0 && mRefreshState != REFRESHING) {
            if ((mpRefreshController->getBottom() >= mRefreshWidgetHeight || mpRefreshController->getTop() >= 0)
                    && mRefreshState == RELEASE_TO_REFRESH) {
                // Initiate the refresh
                mRefreshState = REFRESHING;
                prepareForRefresh();
                onRefresh();
            } else if (mpRefreshController->getBottom() < mRefreshWidgetHeight || mpRefreshController->getTop() <= 0) {
                // Abort refresh and scroll down below the refresh Widget
                resetHeader();
                setSelection(1);
            }
        }
        break;
    case MotionEvent::ACTION_DOWN:
        mLastMotionY = y;
        break;
    case MotionEvent::ACTION_MOVE:
        applyHeaderPadding(*event);
        break;
    }
    return ListWidget::onTouchEvent(event);
}

void PullToRefreshListWidget::applyHeaderPadding(const MotionEvent& ev) {
    int32_t pointerCount = ev.getHistorySize();
    for (int32_t p = 0; p < pointerCount; p++) {
        if (mRefreshState == RELEASE_TO_REFRESH) {
            if (isVerticalScrollBarHidden()) {
                setVerticalScrollBarEnabled(false);
            }
            int32_t historicalY = (int32_t) ev.getHistoricalY(p);
            int32_t topPadding = (int32_t) (((historicalY - mLastMotionY) - mRefreshWidgetHeight) / 1.7);

            mpRefreshController->setPadding(mpRefreshController->getPaddingLeft(), topPadding, mpRefreshController->getPaddingRight(),
                    mpRefreshController->getPaddingBottom());
        }
    }
}

void PullToRefreshListWidget::resetHeaderPadding() {
    mpRefreshController->setPadding(mpRefreshController->getPaddingLeft(), mRefreshOriginalTopPadding, mpRefreshController->getPaddingRight(),
            mpRefreshController->getPaddingBottom());
}

void PullToRefreshListWidget::resetHeader() {
    if (mRefreshState != TAP_TO_REFRESH) {
        mRefreshState = TAP_TO_REFRESH;

        resetHeaderPadding();

        // Set refresh Widget text to the pull label
        mpRefreshText->setText(PullToRefresh::R::string::pull_to_refresh_tap_label);
        // Replace refresh drawable with arrow drawable
        mpRefreshImage->setImageResource(PullToRefresh::R::drawable::ic_pulltorefresh_arrow);
        // Clear the full rotation animation
        mpRefreshImage->clearAnimation();
        // Hide progress bar and arrow.
        mpRefreshImage->setVisibility(Widget::GONE);
        mpRefreshProgress->setVisibility(Widget::GONE);
    }
}

void PullToRefreshListWidget::measureView(Widget* child) {
    ControllerParams* p = child->getControllerParams();
    if (p == NULL) {
        p = new ControllerParams(ControllerParams::MATCH_PARENT, ControllerParams::WRAP_CONTENT);
    }

    int32_t childWidthSpec = WidgetController::getChildMeasureSpec(0, 0 + 0, p->getWidth());
    int32_t lpHeight = p->getHeight();
    int32_t childHeightSpec;
    if (lpHeight > 0) {
        childHeightSpec = MeasureSpec::makeMeasureSpec(lpHeight, MeasureSpec::EXACTLY);
    } else {
        childHeightSpec = MeasureSpec::makeMeasureSpec(0, MeasureSpec::UNSPECIFIED);
    }
    child->measure(childWidthSpec, childHeightSpec);
}

void PullToRefreshListWidget::onScroll(AbsListWidget* Widget, int32_t firstVisibleItem, int32_t visibleItemCount, int32_t totalItemCount) {
// When the refresh Widget is completely visible, change the text to say
// "Release to refresh..." and flip the arrow drawable.
    if (mCurrentScrollState == SCROLL_STATE_TOUCH_SCROLL && mRefreshState != REFRESHING) {
        if (firstVisibleItem == 0) {
            mpRefreshImage->setVisibility(Widget::VISIBLE);
            if ((mpRefreshController->getBottom() >= mRefreshWidgetHeight + 20 || mpRefreshController->getTop() >= 0)
                    && mRefreshState != RELEASE_TO_REFRESH) {
                mpRefreshText->setText(PullToRefresh::R::string::pull_to_refresh_release_label);
                mpRefreshImage->clearAnimation();
                mpRefreshImage->startAnimation(mpFlipAnimation);
                mRefreshState = RELEASE_TO_REFRESH;
            } else if (mpRefreshController->getBottom() < mRefreshWidgetHeight + 20 && mRefreshState != PULL_TO_REFRESH) {
                mpRefreshText->setText(PullToRefresh::R::string::pull_to_refresh_pull_label);
                if (mRefreshState != TAP_TO_REFRESH) {
                    mpRefreshImage->clearAnimation();
                    mpRefreshImage->startAnimation(mpReverseFlipAnimation);
                }
                mRefreshState = PULL_TO_REFRESH;
            }
        } else {
            mpRefreshImage->setVisibility(Widget::GONE);
            resetHeader();
        }
    } else if (mCurrentScrollState == SCROLL_STATE_FLING && firstVisibleItem == 0 && mRefreshState != REFRESHING) {
        setSelection(1);
        mBounceHack = true;
    } else if (mBounceHack && mCurrentScrollState == SCROLL_STATE_FLING) {
        setSelection(1);
    }

    if (mpOnScrollListener != NULL) {
        mpOnScrollListener->onScroll(Widget, firstVisibleItem, visibleItemCount, totalItemCount);
    }
}

void PullToRefreshListWidget::onScrollStateChanged(AbsListWidget* Widget, int32_t scrollState) {
    mCurrentScrollState = scrollState;

    if (mCurrentScrollState == SCROLL_STATE_IDLE) {
        mBounceHack = false;
    }

    if (mpOnScrollListener != NULL) {
        mpOnScrollListener->onScrollStateChanged(Widget, scrollState);
    }
}

void PullToRefreshListWidget::prepareForRefresh() {
    resetHeaderPadding();

    mpRefreshImage->setVisibility(Widget::GONE);
// We need this hack, otherwise it will keep the previous drawable.
    mpRefreshImage->setImageDrawer(NULL);
    mpRefreshProgress->setVisibility(Widget::VISIBLE);

// Set refresh Widget text to the refreshing label
    mpRefreshText->setText(PullToRefresh::R::string::pull_to_refresh_refreshing_label);

    mRefreshState = REFRESHING;
}

void PullToRefreshListWidget::onRefresh() {
    if (mpOnRefreshListener != NULL) {
        mpOnRefreshListener->onRefresh();
    }
}

void PullToRefreshListWidget::onRefreshComplete(const String& lastUpdated) {
    setLastUpdated(lastUpdated);
    onRefreshComplete();
}

void PullToRefreshListWidget::onRefreshComplete() {
    resetHeader();

// If refresh Widget is visible when loading completes, scroll down to the next item.
    if (mpRefreshController->getBottom() > 0) {
        invalidateViews();
        setSelection(1);
    }
}

void PullToRefreshListWidget::onClick(Widget* v) {
    if (mRefreshState != REFRESHING) {
        prepareForRefresh();
        onRefresh();
    }
}

OnRefreshListener::OnRefreshListener() {
}

OnRefreshListener::~OnRefreshListener() {
}
