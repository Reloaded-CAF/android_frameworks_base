/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <private/hwui/WebViewFunctor.h>
#include <renderthread/RenderProxy.h>

#include <utils/LightRefBase.h>
#include <mutex>
#include <vector>

namespace android::uirenderer {

class WebViewFunctorManager;

class WebViewFunctor {
public:
    WebViewFunctor(void* data, const WebViewFunctorCallbacks& callbacks, RenderMode functorMode);
    ~WebViewFunctor();

    class Handle : public LightRefBase<Handle> {
    public:
        ~Handle() { renderthread::RenderProxy::destroyFunctor(id()); }

        int id() const { return mReference.id(); }

        void sync(const WebViewSyncData& syncData) const { mReference.sync(syncData); }

        void drawGl(const DrawGlInfo& drawInfo) const { mReference.drawGl(drawInfo); }

    private:
        friend class WebViewFunctor;

        Handle(WebViewFunctor& ref) : mReference(ref) {}

        WebViewFunctor& mReference;
    };

    int id() const { return mFunctor; }
    void sync(const WebViewSyncData& syncData) const;
    void drawGl(const DrawGlInfo& drawInfo);
    void destroyContext();

    sp<Handle> createHandle() {
        LOG_ALWAYS_FATAL_IF(mCreatedHandle);
        mCreatedHandle = true;
        return sp<Handle>{new Handle(*this)};
    }

private:
    WebViewFunctorCallbacks mCallbacks;
    void* const mData;
    int mFunctor;
    RenderMode mMode;
    bool mHasContext = false;
    bool mCreatedHandle = false;
};

class WebViewFunctorManager {
public:
    static WebViewFunctorManager& instance();

    int createFunctor(void* data, const WebViewFunctorCallbacks& callbacks, RenderMode functorMode);
    void releaseFunctor(int functor);
    void onContextDestroyed();
    void destroyFunctor(int functor);

    sp<WebViewFunctor::Handle> handleFor(int functor);

private:
    WebViewFunctorManager() = default;
    ~WebViewFunctorManager() = default;

    std::mutex mLock;
    std::vector<std::unique_ptr<WebViewFunctor>> mFunctors;
    std::vector<sp<WebViewFunctor::Handle>> mActiveFunctors;
};

}  // namespace android::uirenderer
