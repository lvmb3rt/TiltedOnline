#pragma once

#if TP_SKYRIM

#include <Games/Primitives.h>

#include <Games/Skyrim/Havok/BShkbAnimationGraph.h>

struct BSFixedString;

struct BSAnimationGraphManager
{
    virtual ~BSAnimationGraphManager();
    virtual void sub_1(void* apUnk1);

    void Release()
    {
        if (InterlockedDecrement(&refCount) == 0)
            this->~BSAnimationGraphManager();
    }

    volatile LONG refCount;
    void* pad_ptrs[6];
    BSTSmallArray<BShkbAnimationGraph> animationGraphs; // 40 - 20
    void* pad_ptrs2[8];
    BSRecursiveLock lock; // 98 - 4C
    void* unkPtrAfterLock; // A0 - 58

#if TP_PLATFORM_32
    void* unkPtrOldrim;
#endif

    uint32_t animationGraphIndex; // A8 - 5C

    void DumpAnimationVariables();
    uint32_t ReSendEvent(BSFixedString* apEventName);
};


#if TP_PLATFORM_64
static_assert(offsetof(BSAnimationGraphManager, animationGraphs) == 0x40);
static_assert(offsetof(BSAnimationGraphManager, lock) == 0x98);
static_assert(offsetof(BSAnimationGraphManager, animationGraphIndex) == 0xA8);
#else
static_assert(offsetof(BSAnimationGraphManager, animationGraphs) == 0x20);
static_assert(offsetof(BSAnimationGraphManager, lock) == 0x4C);
static_assert(offsetof(BSAnimationGraphManager, animationGraphIndex) == 0x5C);
#endif

#endif
