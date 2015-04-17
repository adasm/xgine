#pragma once
#include "Headers.h"

class TriggerReport : public NxUserTriggerReport
{
    virtual void onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status);
};