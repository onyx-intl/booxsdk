#ifndef RENDER_POLICY_H
#define RENDER_POLICY_H

#include "onyx/base/base.h"
#include "onyx/ui/ui.h"

namespace vbf
{

// Render requests
// First parameter is the page number and the second is the priority
#ifdef _WINDOWS
typedef stdext::hash_map<int, int>  RenderRequests;
#else
typedef unordered_map<int, int>  RenderRequests;
#endif

typedef RenderRequests::iterator RenderRequestsIter;

class RenderPolicy
{
public:
    RenderPolicy();
    virtual ~RenderPolicy();
    virtual void getRenderRequests(const int current_page,
                                   const int previous_page,
                                   const int total,
                                   QVector<int> & result) = 0;

    void setPrerender(bool yes) { need_prerender_ = yes; }
    bool needPrerender() const { return need_prerender_; }
    bool isRenderingPage(int page_number);
    const RenderRequests & renderRequests() const { return requests_; }
    int getPriority(int page_number);

protected:
    void updateRequests(const QVector<int> & sources);

private:
    bool need_prerender_;
    RenderRequests requests_;

    // Image render requests list should be thread-safe
    QMutex mutex_;
};

};
#endif
