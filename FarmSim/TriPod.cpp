#include "TriPod.h"

TriPod::TriPod(IVehicle *vehicle, string fname, bool FrontTriPod)
{
	m_tractor = true;
	m_vehicle = vehicle;
	m_triPodDimms = new TriPodDimms;
	m_isLifted = false;
	m_attachedDevice = NULL;
	m_attachedTrailer = NULL;
	m_jointTopPodDevice = NULL;
	m_jointBottomLeftPodDevice = NULL;
	m_jointBottomRightPodDevice = NULL;
	m_jointTrailer = NULL;

	string topPodModel;
	string bottomPodModel;

	m_triPodDimms->getVariableList()->push_back(new CfgVariable(V_STRING, &topPodModel, getVarName(topPodModel)));
	m_triPodDimms->getVariableList()->push_back(new CfgVariable(V_STRING, &bottomPodModel, getVarName(bottomPodModel)));

	CfgLoader(fname, m_triPodDimms->getVariableList());

	m_objTop = new Surface(topPodModel, new Material(MT_DEFAULT, "wood.jpg"), Vec3(0, 0, 0));
	m_objBottom = new Surface(bottomPodModel, new Material(MT_DEFAULT, "wood.jpg"), Vec3(0, 0, 0));

	NxVec3 actorTopPos = NxVec3(m_vehicle->getVehicleController()->getVehicleParams()->posTriPodBack) + NxVec3(m_triPodDimms->posTopPodTractorConnector);
	NxVec3 actorBottomPos = NxVec3(m_vehicle->getVehicleController()->getVehicleParams()->posTriPodBack) + NxVec3(m_triPodDimms->posBottomPodTractorConnector);

	NxVec3 actorTopDimm = NxVec3(m_objTop->boundingBox.getWidth()/2, m_objTop->boundingBox.getHeight()/2, m_objTop->boundingBox.getDepth()/2);
	NxVec3 actorBottomDimm = NxVec3(m_objBottom->boundingBox.getWidth()/2, m_objBottom->boundingBox.getHeight()/2, m_objBottom->boundingBox.getDepth()/2);


	/*m_actorTop = core.dynamics->createBox(actorTopPos, actorTopDimm, 1);
	m_actorBottom = core.dynamics->createBox(actorBottomPos, actorBottomDimm, 1);*/
	m_actorTop = core.dynamics->createBox(actorTopPos, NxVec3(D3DXVec3Length(&Vec3(m_triPodDimms->posTopPodTractorConnector - m_triPodDimms->posTopPodDeviceConnector))/2, 0.1f, 0.1f), 500);
	m_actorBottom = core.dynamics->createBox(actorBottomPos, NxVec3(D3DXVec3Length(&Vec3(m_triPodDimms->posBottomPodTractorConnector - m_triPodDimms->posBottomPodDeviceConnector))/2, 0.1f, m_triPodDimms->width / 2), 500);
	/*m_triPodDimms->posTopPodDeviceConnector.x -= 0.2f;
	m_triPodDimms->posBottomPodDeviceConnector.x -= 0.2f;*/

	/*SetActorCollisionGroup(m_actorTop, GROUP_COLLIDABLE_NON_PUSHABLE);
	SetActorCollisionGroup(m_actorBottom, GROUP_COLLIDABLE_NON_PUSHABLE);*/
	SetActorCollisionGroup(m_actorTop, GROUP_COLLIDABLE_NON_PUSHABLE);
	SetActorCollisionGroup(m_actorBottom, GROUP_COLLIDABLE_NON_PUSHABLE);

	core.game->getWorld()->addToWorld(m_objTop, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objBottom, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);

	m_frontPod = FrontTriPod;

	if(!m_frontPod)
	{
		float highLimit = D3DX_PI * 0.12f;
		float lowLimit = D3DX_PI * -0.1f;
		m_jointMotorTop = new NxMotorDesc(0, NX_MAX_REAL, 0);
		m_jointMotorBottom = new NxMotorDesc(0, NX_MAX_REAL, 0);

		Vec3 newPosTop = m_vehicle->getVehicleController()->getBackJointPoint() + (m_triPodDimms->posTopPod) - m_triPodDimms->posTopPodTractorConnector;
		Vec3 newPosBottom = m_vehicle->getVehicleController()->getBackJointPoint() + (m_triPodDimms->posBottomPod) - m_triPodDimms->posBottomPodTractorConnector;

		Vec3 moveTop = m_vehicle->getVehicleController()->getBackJointPoint() + (m_triPodDimms->posTopPod);// - m_triPodDimms->posTopPodTractorConnector;
		Vec3 moveBottom = m_vehicle->getVehicleController()->getBackJointPoint() + (m_triPodDimms->posBottomPod);// - m_triPodDimms->posBottomPodTractorConnector;
		/*Vec3 moveTop = Vec3(actorTopPos.x, actorTopPos.y, actorTopPos.z) - m_vehicle->getVehicleController()->getBackJointPoint();
		Vec3 moveBottom = Vec3(actorBottomPos.x, actorBottomPos.y, actorBottomPos.z) - m_vehicle->getVehicleController()->getBackJointPoint();*/
		/*m_actorTop->setGlobalPosition(m_actorTop->getGlobalPosition() + NxVec3(moveTop));
		m_actorBottom->setGlobalPosition(m_actorBottom->getGlobalPosition() + NxVec3(moveBottom));*/
		m_actorTop->setGlobalPosition(NxVec3(newPosTop));
		m_actorBottom->setGlobalPosition(NxVec3(newPosBottom));


		NxRevoluteJointDesc revDescTop;
		revDescTop.actor[0] = m_vehicle->getVehicleController()->getActor();
		revDescTop.actor[1] = m_actorTop;
		revDescTop.localNormal[0] = NxVec3(0, 1, 0);
		revDescTop.localNormal[1] = NxVec3(0, 1, 0);
		revDescTop.limit.high.value = highLimit;
		revDescTop.limit.low.value = lowLimit;
		revDescTop.setGlobalAxis(NxVec3(0, 0, 1)); //The direction of the axis the bodies revolve around.
		revDescTop.setGlobalAnchor(NxVec3(moveTop)); //Reference point that the axis passes through.
		revDescTop.projectionMode = NX_JPM_POINT_MINDIST;
		revDescTop.projectionDistance = 0.1f;

		revDescTop.motor = *m_jointMotorTop;
		revDescTop.flags |= NX_RJF_MOTOR_ENABLED | NX_RJF_LIMIT_ENABLED;

		m_jointTop = (NxRevoluteJoint*)core.dynamics->getScene()->createJoint(revDescTop);

		/*NxRevoluteJointDesc revDescBottom;
		revDescBottom.actor[0] = m_vehicle->getVehicleController()->getActor();
		revDescBottom.actor[1] = m_actorBottom;
		revDescBottom.localNormal[0] = NxVec3(0, 1, 0);
		revDescBottom.localNormal[1] = NxVec3(0, 1, 0);
		revDescBottom.limit.high.value = highLimit;
		revDescBottom.limit.low.value = lowLimit;
		revDescBottom.setGlobalAxis(NxVec3(0, 0, 1)); //The direction of the axis the bodies revolve around.
		revDescBottom.setGlobalAnchor(NxVec3(moveBottom)); //Reference point that the axis passes through.

		revDescBottom.motor = *m_jointMotorBottom;
		revDescBottom.flags |= NX_RJF_MOTOR_ENABLED | NX_RJF_LIMIT_ENABLED;

		//m_jointBottom = (NxRevoluteJoint*)core.dynamics->getScene()->createJoint(revDescBottom);*/

		Vec3 leftPos = Vec3(0, 0, m_triPodDimms->width/2);
		Mat temp;
		m_actorTop->getGlobalPose().getColumnMajor44((NxF32*)&temp);
		temp._41 = 0;
		temp._42 = 0;
		temp._43 = 0;
		D3DXVec3TransformCoord(&leftPos, &leftPos, &temp);





		NxRevoluteJointDesc revDescBottom;
		revDescBottom.actor[0] = m_vehicle->getVehicleController()->getActor();
		revDescBottom.actor[1] = m_actorBottom;
		revDescBottom.localNormal[0] = NxVec3(0, 1, 0);
		revDescBottom.localNormal[1] = NxVec3(0, 1, 0);
		revDescBottom.limit.high.value = highLimit;
		revDescBottom.limit.low.value = lowLimit;
		revDescBottom.setGlobalAxis(NxVec3(0, 0, 1)); //The direction of the axis the bodies revolve around.
		revDescBottom.setGlobalAnchor(NxVec3(moveBottom + leftPos)); //Reference point that the axis passes through.
		revDescBottom.projectionMode = NX_JPM_POINT_MINDIST;
		revDescBottom.projectionDistance = 0.1f;

		revDescBottom.motor = *m_jointMotorBottom;
		revDescBottom.flags |= NX_RJF_MOTOR_ENABLED | NX_RJF_LIMIT_ENABLED;

		m_jointBottomLeft = (NxRevoluteJoint*)core.dynamics->getScene()->createJoint(revDescBottom);







		NxRevoluteJointDesc revDescBottom1;
		revDescBottom1.actor[0] = m_vehicle->getVehicleController()->getActor();
		revDescBottom1.actor[1] = m_actorBottom;
		revDescBottom1.localNormal[0] = NxVec3(0, 1, 0);
		revDescBottom1.localNormal[1] = NxVec3(0, 1, 0);
		revDescBottom1.setGlobalAxis(NxVec3(0, 0, 1)); //The direction of the axis the bodies revolve around.
		revDescBottom1.setGlobalAnchor(NxVec3(moveBottom - leftPos));
		revDescBottom1.limit.high.value = highLimit;
		revDescBottom1.limit.low.value = lowLimit;
		revDescBottom1.projectionMode = NX_JPM_POINT_MINDIST;
		revDescBottom1.projectionDistance = 0.1f;

		revDescBottom1.motor = *m_jointMotorBottom;
		revDescBottom1.flags |= NX_RJF_MOTOR_ENABLED | NX_RJF_LIMIT_ENABLED;


		m_jointBottomRight = (NxRevoluteJoint*)core.dynamics->getScene()->createJoint(revDescBottom1);
	}
	else
	{
		Vec3 switchVec;
		switchVec = m_triPodDimms->posBottomPodDeviceConnector;
		m_triPodDimms->posBottomPodDeviceConnector = m_triPodDimms->posBottomPodTractorConnector;
		m_triPodDimms->posBottomPodTractorConnector = switchVec;

		switchVec = m_triPodDimms->posTopPodDeviceConnector;
		m_triPodDimms->posTopPodDeviceConnector = m_triPodDimms->posTopPodTractorConnector;
		m_triPodDimms->posTopPodTractorConnector = switchVec;

		float highLimit = D3DX_PI * 0.12f;
		float lowLimit = D3DX_PI * -0.1f;
		m_jointMotorTop = new NxMotorDesc(0, NX_MAX_REAL, 0);
		m_jointMotorBottom = new NxMotorDesc(0, NX_MAX_REAL, 0);

		Vec3 newPosTop = m_vehicle->getVehicleController()->getFrontJointPoint() + (m_triPodDimms->posTopPod) - m_triPodDimms->posTopPodTractorConnector;
		Vec3 newPosBottom = m_vehicle->getVehicleController()->getFrontJointPoint() + (m_triPodDimms->posBottomPod) - m_triPodDimms->posBottomPodTractorConnector;

		Vec3 moveTop = m_vehicle->getVehicleController()->getFrontJointPoint() + (m_triPodDimms->posTopPod);// - m_triPodDimms->posTopPodTractorConnector;
		Vec3 moveBottom = m_vehicle->getVehicleController()->getFrontJointPoint() + (m_triPodDimms->posBottomPod);// - m_triPodDimms->posBottomPodTractorConnector;
		/*Vec3 moveTop = Vec3(actorTopPos.x, actorTopPos.y, actorTopPos.z) - m_vehicle->getVehicleController()->getBackJointPoint();
		Vec3 moveBottom = Vec3(actorBottomPos.x, actorBottomPos.y, actorBottomPos.z) - m_vehicle->getVehicleController()->getBackJointPoint();*/
		/*m_actorTop->setGlobalPosition(m_actorTop->getGlobalPosition() + NxVec3(moveTop));
		m_actorBottom->setGlobalPosition(m_actorBottom->getGlobalPosition() + NxVec3(moveBottom));*/
		m_actorTop->setGlobalPosition(NxVec3(newPosTop));
		m_actorBottom->setGlobalPosition(NxVec3(newPosBottom));


		NxRevoluteJointDesc revDescTop;
		revDescTop.actor[0] = m_vehicle->getVehicleController()->getActor();
		revDescTop.actor[1] = m_actorTop;
		revDescTop.localNormal[0] = NxVec3(0, 1, 0);
		revDescTop.localNormal[1] = NxVec3(0, 1, 0);
		revDescTop.limit.high.value = highLimit;
		revDescTop.limit.low.value = lowLimit;
		revDescTop.setGlobalAxis(NxVec3(0, 0, -1)); //The direction of the axis the bodies revolve around.
		revDescTop.setGlobalAnchor(NxVec3(moveTop)); //Reference point that the axis passes through.
		revDescTop.projectionMode = NX_JPM_POINT_MINDIST;
		revDescTop.projectionDistance = 0.1f;

		revDescTop.motor = *m_jointMotorTop;
		revDescTop.flags |= NX_RJF_MOTOR_ENABLED | NX_RJF_LIMIT_ENABLED;

		m_jointTop = (NxRevoluteJoint*)core.dynamics->getScene()->createJoint(revDescTop);



		Vec3 leftPos = Vec3(0, 0, m_triPodDimms->width/2);
		Mat temp;
		m_actorTop->getGlobalPose().getColumnMajor44((NxF32*)&temp);
		temp._41 = 0;
		temp._42 = 0;
		temp._43 = 0;
		D3DXVec3TransformCoord(&leftPos, &leftPos, &temp);





		NxRevoluteJointDesc revDescBottom;
		revDescBottom.actor[0] = m_vehicle->getVehicleController()->getActor();
		revDescBottom.actor[1] = m_actorBottom;
		revDescBottom.localNormal[0] = NxVec3(0, 1, 0);
		revDescBottom.localNormal[1] = NxVec3(0, 1, 0);
		revDescBottom.limit.high.value = highLimit;
		revDescBottom.limit.low.value = lowLimit;
		revDescBottom.setGlobalAxis(NxVec3(0, 0, -1)); //The direction of the axis the bodies revolve around.
		revDescBottom.setGlobalAnchor(NxVec3(moveBottom + leftPos)); //Reference point that the axis passes through.
		revDescBottom.projectionMode = NX_JPM_POINT_MINDIST;
		revDescBottom.projectionDistance = 0.1f;

		revDescBottom.motor = *m_jointMotorBottom;
		revDescBottom.flags |= NX_RJF_MOTOR_ENABLED | NX_RJF_LIMIT_ENABLED;

		m_jointBottomLeft = (NxRevoluteJoint*)core.dynamics->getScene()->createJoint(revDescBottom);







		NxRevoluteJointDesc revDescBottom1;
		revDescBottom1.actor[0] = m_vehicle->getVehicleController()->getActor();
		revDescBottom1.actor[1] = m_actorBottom;
		revDescBottom1.localNormal[0] = NxVec3(0, 1, 0);
		revDescBottom1.localNormal[1] = NxVec3(0, 1, 0);
		revDescBottom1.setGlobalAxis(NxVec3(0, 0, -1)); //The direction of the axis the bodies revolve around.
		revDescBottom1.setGlobalAnchor(NxVec3(moveBottom - leftPos));
		revDescBottom1.limit.high.value = highLimit;
		revDescBottom1.limit.low.value = lowLimit;
		revDescBottom1.projectionMode = NX_JPM_POINT_MINDIST;
		revDescBottom1.projectionDistance = 0.1f;

		revDescBottom1.motor = *m_jointMotorBottom;
		revDescBottom1.flags |= NX_RJF_MOTOR_ENABLED | NX_RJF_LIMIT_ENABLED;


		m_jointBottomRight = (NxRevoluteJoint*)core.dynamics->getScene()->createJoint(revDescBottom1);
	}
	lift();
	update();
}

void TriPod::update()
{
	m_actorTop->getGlobalPose().getColumnMajor44((NxF32*)&m_matTop);
	m_actorBottom->getGlobalPose().getColumnMajor44((NxF32*)&m_matBottom);

	///Check if tripod is not broken
	Vec3 postTransTop;
	Vec3 postTransBottom;
	D3DXVec3TransformCoord(&postTransBottom, &m_triPodDimms->posBottomPodDeviceConnector, &m_matBottom);
	D3DXVec3TransformCoord(&postTransTop, &m_triPodDimms->posTopPodDeviceConnector, &m_matTop);
	if(postTransTop.y <= postTransBottom.y)
		reset();


	if(m_frontPod)
	{
		Mat rot;
		D3DXMatrixRotationY(&rot, D3DX_PI);
		m_matTop = rot * m_matTop;
		m_matBottom = rot * m_matBottom;
	}
	if(m_objTop)
	{
		m_objTop->boundingBox.setTransform(Vec3(m_matTop._41, m_matTop._42, m_matTop._43));
		m_objTop->world = m_matTop;
		m_objTop->position = Vec3(m_matTop._41, m_matTop._42, m_matTop._43);
	}

	if(m_objBottom)
	{
		m_objBottom->boundingBox.setTransform(Vec3(m_matBottom._41, m_matBottom._42, m_matBottom._43));
		m_objBottom->world = m_matBottom;
		m_objBottom->position = Vec3(m_matBottom._41, m_matBottom._42, m_matBottom._43);
	}
}

void TriPod::lift()
{
	m_isLifted = true;
	m_jointMotorTop->velTarget = 0.5f;
	m_jointMotorBottom->velTarget = 0.5f;
	m_jointTop->setMotor(*m_jointMotorTop);
	m_jointBottomLeft->setMotor(*m_jointMotorBottom);
	m_jointBottomRight->setMotor(*m_jointMotorBottom);
}

void TriPod::low()
{
	m_isLifted = false;
	m_jointMotorTop->velTarget = -0.5f;
	m_jointMotorBottom->velTarget = -0.5f;
	m_jointTop->setMotor(*m_jointMotorTop);
	m_jointBottomLeft->setMotor(*m_jointMotorBottom);
	m_jointBottomRight->setMotor(*m_jointMotorBottom);
}

bool TriPod::isLifted()
{
	return m_isLifted;
}

AttachType TriPod::isAttached()
{
	if(m_attachedDevice)
		return AT_DEVICE;
	if(m_attachedTrailer)
		return AT_TRAILER;
	return AT_NONE;
}

void TriPod::attach(IAgriDevice *device)
{
	if(m_attachedTrailer)
		return;
	if(m_attachedDevice)
		return;

	m_attachedDevice = device;
	Vec3 transformedDeviceConnectorTop;
	Vec3 transformedDeviceConnectorBottom;
	Vec3 transformedTriPodPos;
	Vec3 diff = m_triPodDimms->posTopPodDeviceConnector - m_triPodDimms->posTopPodTractorConnector;
	Vec3 transDiff;
	D3DXVec3TransformCoord(&transDiff, &diff, &m_matTop);

	if(m_frontPod)
	{
		D3DXVec3TransformCoord(&transformedDeviceConnectorTop, &m_triPodDimms->posTopPodTractorConnector, &m_matTop);
		D3DXVec3TransformCoord(&transformedDeviceConnectorBottom, &m_triPodDimms->posBottomPodTractorConnector, &m_matBottom);
	}
	else
	{
		D3DXVec3TransformCoord(&transformedDeviceConnectorTop, &m_triPodDimms->posTopPodDeviceConnector, &m_matTop);
		D3DXVec3TransformCoord(&transformedDeviceConnectorBottom, &m_triPodDimms->posBottomPodDeviceConnector, &m_matBottom);
	}

	NxMat34 pose = m_vehicle->getVehicleController()->getActor()->getGlobalPose();
	if(m_frontPod)
	{
		Mat mat;
		D3DXMatrixRotationY(&mat, D3DX_PI);
		NxMat33 mat33;
		NxQuat quat;
		pose.M.toQuat(quat);
		quat.normalize();
		NxReal angle;
		NxVec3 axis;
		quat.getAngleAxis(angle, axis);
		axis.x *= -1;
		axis.z *= -1;
		//quat.w *= -1;
		quat.fromAngleAxis(angle, axis);
		pose.M.fromQuat(quat);
		pose = NxMat34(NxMat33(NxVec3(mat._11, mat._12, mat._13), NxVec3(mat._21, mat._22, mat._23), NxVec3(mat._31, mat._32, mat._33)), NxVec3(0, 0, 0)) * pose;
	}
	
	NxVec3 trans = NxVec3(m_vehicle->getVehicleController()->getForwardVec() * -10);
	pose.t.x = pose.t.x + trans.x;
	pose.t.y = pose.t.y + trans.y;
	pose.t.z = pose.t.z + trans.z;
	m_attachedDevice->getActor()->setGlobalPose(pose);
	m_attachedDevice->update();
	transformedTriPodPos = transformedDeviceConnectorTop - m_triPodDimms->posTopPod;

	Vec3 move = transformedTriPodPos - m_attachedDevice->getTriPodPos();
	m_attachedDevice->getActor()->setGlobalPosition(m_attachedDevice->getActor()->getGlobalPosition() + NxVec3(move));
	m_attachedDevice->update();

	//NxRevoluteJointDesc revDescTop;
	//revDescTop.actor[0] = m_actorTop;
	//revDescTop.actor[1] = m_attachedDevice->getActor();
	//revDescTop.localNormal[0] = NxVec3(0, 1, 0);
	//revDescTop.localNormal[1] = NxVec3(0, 1, 0);
	//revDescTop.setGlobalAxis(NxVec3(0, 0, 1)); //The direction of the axis the bodies revolve around.
	//revDescTop.setGlobalAnchor(NxVec3(m_attachedDevice->getTriPodPos() + m_triPodDimms->posTopPod)); //Reference point that the axis passes through.

	//m_jointTopPodDevice = (NxRevoluteJoint*)core.dynamics->getScene()->createJoint(revDescTop);



	Vec3 leftPos = Vec3(0, 0, m_triPodDimms->width/2);
	Mat temp;
	m_actorTop->getGlobalPose().getColumnMajor44((NxF32*)&temp);
	temp._41 = 0;
	temp._42 = 0;
	temp._43 = 0;
	D3DXVec3TransformCoord(&leftPos, &leftPos, &temp);

	//NxRevoluteJointDesc revDescBottomLeft;
	//revDescBottomLeft.actor[0] = m_actorBottom;
	//revDescBottomLeft.actor[1] = m_attachedDevice->getActor();
	//revDescBottomLeft.localNormal[0] = NxVec3(0, 1, 0);
	//revDescBottomLeft.localNormal[1] = NxVec3(0, 1, 0);
	//revDescBottomLeft.setGlobalAxis(NxVec3(0, 0, 1)); //The direction of the axis the bodies revolve around.
	//revDescBottomLeft.setGlobalAnchor(NxVec3(m_attachedDevice->getTriPodPos() + m_triPodDimms->posBottomPod + leftPos)); //Reference point that the axis passes through.

	//m_jointBottomLeftPodDevice = (NxRevoluteJoint*)core.dynamics->getScene()->createJoint(revDescBottomLeft);

	//NxRevoluteJointDesc revDescBottomRight;
	//revDescBottomRight.actor[0] = m_actorBottom;
	//revDescBottomRight.actor[1] = m_attachedDevice->getActor();
	//revDescBottomRight.localNormal[0] = NxVec3(0, 1, 0);
	//revDescBottomRight.localNormal[1] = NxVec3(0, 1, 0);
	//revDescBottomRight.setGlobalAxis(NxVec3(0, 0, 1)); //The direction of the axis the bodies revolve around.
	//revDescBottomRight.setGlobalAnchor(NxVec3(m_attachedDevice->getTriPodPos() + m_triPodDimms->posBottomPod - leftPos)); //Reference point that the axis passes through.

	//m_jointBottomRightPodDevice = (NxRevoluteJoint*)core.dynamics->getScene()->createJoint(revDescBottomRight);

	//if(m_f
	NxD6JointDesc d6Desc;
	d6Desc.actor[0] = m_actorTop;
	d6Desc.actor[1] = m_attachedDevice->getActor();
	d6Desc.localNormal[0] = NxVec3(0, 1, 0);
	d6Desc.localNormal[1] = NxVec3(0, 1, 0);
	d6Desc.setGlobalAxis(NxVec3(1, 0, 0)); //The direction of the axis the bodies revolve around.
	d6Desc.setGlobalAnchor(NxVec3(m_attachedDevice->getTriPodPos() + m_triPodDimms->posTopPod));
	d6Desc.twistMotion = NX_D6JOINT_MOTION_FREE;
	d6Desc.swing1Motion = NX_D6JOINT_MOTION_FREE;
	d6Desc.swing2Motion = NX_D6JOINT_MOTION_FREE;
	d6Desc.xMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.yMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.zMotion = NX_D6JOINT_MOTION_LOCKED;
    d6Desc.projectionMode = NX_JPM_NONE;
	d6Desc.projectionMode = NX_JPM_POINT_MINDIST;
	d6Desc.projectionDistance = 0.01f;
	m_jointTopPodDevice = (NxD6Joint*)core.dynamics->getScene()->createJoint(d6Desc);



	NxD6JointDesc d6Desc1;
	d6Desc1.actor[0] = m_actorBottom;
	d6Desc1.actor[1] = m_attachedDevice->getActor();
	d6Desc1.localNormal[0] = NxVec3(0, 1, 0);
	d6Desc1.localNormal[1] = NxVec3(0, 1, 0);
	d6Desc1.setGlobalAxis(NxVec3(1, 0, 0)); //The direction of the axis the bodies revolve around.
	d6Desc1.setGlobalAnchor(NxVec3((m_attachedDevice->getTriPodPos() + m_triPodDimms->posBottomPod) + leftPos));
	d6Desc1.twistMotion = NX_D6JOINT_MOTION_FREE;
	d6Desc1.swing1Motion = NX_D6JOINT_MOTION_FREE;
	d6Desc1.swing2Motion = NX_D6JOINT_MOTION_FREE;
	d6Desc1.xMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc1.yMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc1.zMotion = NX_D6JOINT_MOTION_LOCKED;
    d6Desc1.projectionMode = NX_JPM_NONE;
	d6Desc1.projectionMode = NX_JPM_POINT_MINDIST;
	d6Desc1.projectionDistance = 0.01f;
	m_jointBottomLeftPodDevice = (NxD6Joint*)core.dynamics->getScene()->createJoint(d6Desc1);

	NxD6JointDesc d6Desc2;
	d6Desc2.actor[0] = m_actorBottom;
	d6Desc2.actor[1] = m_attachedDevice->getActor();
	d6Desc2.localNormal[0] = NxVec3(0, 1, 0);
	d6Desc2.localNormal[1] = NxVec3(0, 1, 0);
	d6Desc2.setGlobalAxis(NxVec3(1, 0, 0)); //The direction of the axis the bodies revolve around.
	d6Desc2.setGlobalAnchor(NxVec3((m_attachedDevice->getTriPodPos() + m_triPodDimms->posBottomPod) - leftPos));
	d6Desc2.twistMotion = NX_D6JOINT_MOTION_FREE;
	d6Desc2.swing1Motion = NX_D6JOINT_MOTION_FREE;
	d6Desc2.swing2Motion = NX_D6JOINT_MOTION_FREE;
	d6Desc2.xMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc2.yMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc2.zMotion = NX_D6JOINT_MOTION_LOCKED;
    d6Desc2.projectionMode = NX_JPM_NONE;
	d6Desc2.projectionMode = NX_JPM_POINT_MINDIST;
	d6Desc2.projectionDistance = 0.01f;
	m_jointBottomRightPodDevice = (NxD6Joint*)core.dynamics->getScene()->createJoint(d6Desc2);


	return;
}

void TriPod::attach(ITrailer *trailer)
{
	if(m_attachedTrailer)
		return;
	if(m_attachedDevice)
		return;

	m_attachedTrailer = trailer;
	Vec3 transformedDeviceConnectorTop;
	Vec3 transformedDeviceConnectorBottom;
	Vec3 transformedTriPodPos;
	Vec3 diff = m_triPodDimms->posTopPodDeviceConnector - m_triPodDimms->posTopPodTractorConnector;
	Vec3 transDiff;
	D3DXVec3TransformCoord(&transDiff, &diff, &m_matTop);

	D3DXVec3TransformCoord(&transformedDeviceConnectorTop, &m_triPodDimms->posTopPodTractorConnector, &m_matTop);
	D3DXVec3TransformCoord(&transformedDeviceConnectorBottom, &m_triPodDimms->posBottomPodTractorConnector, &m_matBottom);
	D3DXVec3TransformCoord(&transformedDeviceConnectorTop, &m_triPodDimms->posTopPodDeviceConnector, &m_matTop);
	D3DXVec3TransformCoord(&transformedDeviceConnectorBottom, &m_triPodDimms->posBottomPodDeviceConnector, &m_matBottom);

	NxMat34 pose = m_vehicle->getVehicleController()->getActor()->getGlobalPose();

	NxVec3 trans = NxVec3(m_vehicle->getVehicleController()->getForwardVec() * -10);
	pose.t.x = pose.t.x + trans.x;
	pose.t.y = pose.t.y + trans.y;
	pose.t.z = pose.t.z + trans.z;
	m_attachedTrailer->getWheelSet()->getActor()->setGlobalPose(pose);
	m_attachedTrailer->update();
	transformedTriPodPos = transformedDeviceConnectorTop - m_triPodDimms->posTopPod;

	Vec3 move = transformedTriPodPos - m_attachedTrailer->getWheelSet()->getConnectorPos();
	m_attachedTrailer->getWheelSet()->getActor()->setGlobalPosition(m_attachedTrailer->getWheelSet()->getActor()->getGlobalPosition() + NxVec3(move));
	m_attachedTrailer->update();


	NxD6JointDesc d6Desc;
	d6Desc.actor[0] = m_vehicle->getVehicleController()->getActor();//m_actorTop;
	d6Desc.actor[1] = m_attachedTrailer->getWheelSet()->getActor();
	d6Desc.localNormal[0] = NxVec3(0, 1, 0);
	d6Desc.localNormal[1] = NxVec3(0, 1, 0);
	d6Desc.setGlobalAxis(NxVec3(1, 0, 0)); //The direction of the axis the bodies revolve around.
	d6Desc.setGlobalAnchor(NxVec3(m_attachedTrailer->getWheelSet()->getConnectorPos() + m_triPodDimms->posTopPod));
	d6Desc.twistMotion = NX_D6JOINT_MOTION_FREE;
	d6Desc.swing1Motion = NX_D6JOINT_MOTION_FREE;
	d6Desc.swing2Motion = NX_D6JOINT_MOTION_FREE;
	d6Desc.xMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.yMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.zMotion = NX_D6JOINT_MOTION_LOCKED;
    d6Desc.projectionMode = NX_JPM_NONE;
	m_jointTrailer = (NxD6Joint*)core.dynamics->getScene()->createJoint(d6Desc);
	return;
}

void TriPod::detach()
{
	if(m_jointTopPodDevice)
		core.dynamics->getScene()->releaseJoint(*m_jointTopPodDevice);
	if(m_jointBottomLeftPodDevice)
		core.dynamics->getScene()->releaseJoint(*m_jointBottomLeftPodDevice);
	if(m_jointBottomRightPodDevice)
		core.dynamics->getScene()->releaseJoint(*m_jointBottomRightPodDevice);
	if(m_jointTrailer)
		core.dynamics->getScene()->releaseJoint(*m_jointTrailer);
	m_jointTrailer = NULL;
	m_jointTopPodDevice = NULL;
	m_jointBottomLeftPodDevice = NULL;
	m_jointBottomRightPodDevice = NULL;
	m_attachedDevice = NULL;
	m_attachedTrailer = NULL;
	return;
}

void *TriPod::getAttach()
{
	if(m_attachedTrailer)
		return (void*)m_attachedTrailer;
	if(m_attachedDevice)
		return (void*)m_attachedDevice;
	return NULL;
}

void TriPod::reset()
{
	/*if(m_attachedDevice)
		m_attachedDevice->detach();
	if(m_attachedTrailer)
		m_attachedTrailer->detach();
	detach();

	if(m_jointTop)
		core.dynamics->getScene()->releaseJoint(*m_jointTop);
	if(m_jointBottomLeft)
		core.dynamics->getScene()->releaseJoint(*m_jointBottomLeft);
	if(m_jointBottomRight)
		core.dynamics->getScene()->releaseJoint(*m_jointBottomRight);
	m_jointTop = NULL;
	m_jointBottomLeft = NULL;
	m_jointBottomRight = NULL;*/
}