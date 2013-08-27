/*
 * PullToRefreshListWidget.h
 *
 *  Created on: Aug 26, 2013
 *      Author: Rex Si
 */

#pragma once

#include <gaia/core/event/OnClickListener.h>
#include <gaia/core/event/OnTouchListener.h>
#include <gaia/ui/ListWidget.h>

namespace gaia {
namespace core {
class LayoutInflater;
class RotateAnimation;
}
namespace ui {
class ImageWidget;
class TextWidget;
class ProgressBar;
class RelativeController;
}
}
class OnRefreshListener;
class PullToRefreshListWidget: public gaia::ui::ListWidget,
        public gaia::ui::AbsListWidgetOnScrollListener,
        public gaia::core::OnClickListener {
public:
    explicit PullToRefreshListWidget(gaia::core::Page* page);
    PullToRefreshListWidget(gaia::core::Context* context);
    PullToRefreshListWidget(gaia::core::Context* context, gaia::core::AttributeSet* attrs);
    PullToRefreshListWidget(gaia::core::Page* page, gaia::core::AttributeSet* attrs, int32_t defStyle);
    virtual ~PullToRefreshListWidget();

    virtual void setProvider(gaia::ui::IListItemProvider *pProvider);
    virtual void setOnScrollListener(gaia::ui::AbsListWidgetOnScrollListener *l);
    virtual bool dispatchTouchEvent(const gaia::core::MotionEvent& event);
    virtual bool onTouchEvent(gaia::core::MotionEvent *event);

    virtual void onScroll(gaia::ui::AbsListWidget* view, int32_t firstVisibleItem, int32_t visibleItemCount, int32_t totalItemCount);
    virtual void onScrollStateChanged(gaia::ui::AbsListWidget* view, int32_t scrollState);

    /**
     * Register a callback to be invoked when this list should be refreshed.
     *
     * @param onRefreshListener The callback to run.
     */
    void setOnRefreshListener(OnRefreshListener* onRefreshListener);

    /**
     * Set a text to represent when the list was last updated.
     * @param lastUpdated Last updated at.
     */
    void setLastUpdated(const gaia::base::String& lastUpdated);

    void prepareForRefresh();

    void onRefresh();
    /**
     * Resets the list to a normal state after a refresh.
     * @param lastUpdated Last updated at.
     */
    void onRefreshComplete(const gaia::base::String& lastUpdated);

    /**
     * Resets the list to a normal state after a refresh.
     */
    void onRefreshComplete();

protected:
    virtual void onAttachedToWindow();

private:
    void init(const gaia::core::Context& context, gaia::core::AttributeSet* attrs);
    void applyHeaderPadding(const gaia::core::MotionEvent& ev);

    /**
     * Sets the header padding back to original size.
     */
    void resetHeaderPadding();

    /**
     * Resets the header to the original state.
     */
    void resetHeader();

    void measureView(gaia::core::Widget* child);

    /**
     * Invoked when the refresh view is clicked on. This is mainly used when
     * there's only a few items in the list and it's not possible to drag the
     * list.
     */
    virtual void onClick(gaia::core::Widget* v);

private:
    enum {
        pull_to_refresh_progress = 0,
        pull_to_refresh_image,
        pull_to_refresh_text,
        pull_to_refresh_updated_at,
        TAP_TO_REFRESH = 11,
        PULL_TO_REFRESH,
        RELEASE_TO_REFRESH,
        REFRESHING
    };

    OnRefreshListener* mpOnRefreshListener;

    /**
     * Listener that will receive notifications every time the list scrolls.
     */
    gaia::ui::AbsListWidgetOnScrollListener* mpOnScrollListener;
    //    gaia::core::Context* mpContext;

    gaia::ui::RelativeController* mpRefreshController;
    gaia::ui::TextWidget* mpRefreshText;
    gaia::ui::ImageWidget* mpRefreshImage;
    gaia::ui::ProgressBar* mpRefreshProgress;
    gaia::ui::TextWidget* mpRefreshLastUpdated;

    int32_t mCurrentScrollState;
    int32_t mRefreshState;

    gaia::core::RotateAnimation* mpFlipAnimation;
    gaia::core::RotateAnimation* mpReverseFlipAnimation;

    int32_t mRefreshWidgetHeight;
    int32_t mRefreshOriginalTopPadding;
    int32_t mLastMotionY;

    bool mBounceHack;
};

/**
 * Interface definition for a callback to be invoked when list should be
 * refreshed.
 */
class OnRefreshListener {
public:
    OnRefreshListener();
    virtual ~OnRefreshListener();

    /**
     * Called when the list should be refreshed.
     * <p>
     * A call to {@link PullToRefreshListWidget #onRefreshComplete()} is
     * expected to indicate that the refresh has completed.
     */
    virtual void onRefresh() = 0;
};
