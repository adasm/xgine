#include "TriggerReport.h"

void TriggerReport::onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status)
{
	if(status & NX_TRIGGER_ON_ENTER)
	{
		NxActor& triggerActor = triggerShape.getActor();
		NxActor& actor = otherShape.getActor();
		AdditionalData *data = (AdditionalData*)actor.userData;
		AdditionalData *dataTrigger = (AdditionalData*)triggerActor.userData;
		if(dataTrigger && data)
		{
			//if(dataTrigger->dataType == DT_TRAFFIC_ACTOR)
			{
				if(data->dataType != DT_STATIC && data->dataType != DT_UNDEFINED && data->data != dataTrigger->data)
					((TrafficActor*)dataTrigger->data)->triggerIncrease();
			}
		}

		// Actor entered the trigger region defined by triggerActor. Do something(eg explode, shoot at etc)...
	}
	if(status & NX_TRIGGER_ON_LEAVE)
	{
		NxActor& triggerActor = triggerShape.getActor();
		NxActor& actor = otherShape.getActor();
		AdditionalData *data = (AdditionalData*)actor.userData;
		AdditionalData *dataTrigger = (AdditionalData*)triggerActor.userData;
		if(dataTrigger && data)
		{
			//if(dataTrigger->dataType == DT_TRAFFIC_ACTOR)
			{
				if(data->dataType != DT_STATIC && data->dataType != DT_UNDEFINED && data->data != dataTrigger->data)
					((TrafficActor*)dataTrigger->data)->triggerDecrease();
			}
		}

		// Actor left the trigger region defined by triggerActor. Do something(eg stop shooting at them)...
	}
	if(status & NX_TRIGGER_ON_STAY)
	{
		NxActor& triggerActor = triggerShape.getActor();
		NxActor& actor = otherShape.getActor();
		AdditionalData *data = (AdditionalData*)actor.userData;
		AdditionalData *dataTrigger = (AdditionalData*)triggerActor.userData;
		if(dataTrigger && data)
		{
			//if(dataTrigger->dataType == DT_TRAFFIC_ACTOR)
			{
				if(data->dataType != DT_STATIC && data->dataType != DT_UNDEFINED && data->data != dataTrigger->data)
					((TrafficActor*)dataTrigger->data)->triggerInCollision();
			}
		}

		// Actor left the trigger region defined by triggerActor. Do something(eg stop shooting at them)...
	}
}