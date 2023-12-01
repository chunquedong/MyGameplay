#include "base/Base.h"
#include "Control.h"
#include "AbsoluteLayout.h"
#include "Container.h"

namespace mgp
{

static AbsoluteLayout* __instance;

AbsoluteLayout::AbsoluteLayout()
{
}

AbsoluteLayout::~AbsoluteLayout()
{
    __instance = NULL;
}

UPtr<AbsoluteLayout> AbsoluteLayout::create()
{
    if (!__instance)
    {
        __instance = new AbsoluteLayout();
    }
    else
    {
        __instance->addRef();
    }

    return UPtr<AbsoluteLayout>(__instance);
}

Layout::Type AbsoluteLayout::getType()
{
    return Layout::LAYOUT_ABSOLUTE;
}

void AbsoluteLayout::update(const Container* container)
{
    // Nothing to do for absolute layout
}

}