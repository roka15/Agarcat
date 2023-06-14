#include "Scene.h"
#include "RokaRectangle.h"
#include "RokaEllipse.h"
#include "RokaTime.h"
roka::Scene::Scene() :mObjectMaxSize(3), mItemMaxCount(mObjectMaxSize * 5)
{
}

roka::Scene::~Scene()
{
}

void roka::Scene::Initialize()
{
	for (int i = 0; i < mObjectMaxSize; i++)
	{
		GameObject* obj = new RokaRectangle(EGameObjectType::Monster);
		obj->Initialize();
		AddGameObject(obj);
	}

	for (int i = 0; i < mItemMaxCount; i++)
	{
		mCurItemCnt++;
		GameObject* obj = new RokaRectangle(EGameObjectType::Item);
		obj->Initialize();
		AddGameObject(obj);
	}

	GameObject* player = new RokaEllipse(EGameObjectType::Player);
	player->Initialize();
	AddGameObject(player);
}

void roka::Scene::Update()
{
	mTime += Time::DeltaTime();
	if ((UINT)mTime % 5 == 0)
	{
		ActiveItemOnce();
	}

	for (GameObject* gameObj : mGameObjects)
	{
		gameObj->Update();
	}

}

void roka::Scene::LateUpdate()
{
}

void roka::Scene::Render()
{
	for (GameObject* gameObj : mGameObjects)
	{
		gameObj->Render();
	}
}

void roka::Scene::DeadItemErase(GameObject* obj)
{
	RemoveGameObject(obj);
	mCurItemCnt--;
}

void roka::Scene::ActiveItemOnce()
{
	if (mDeadItemQueue.empty() == true)
		return;
	GameObject* obj = mDeadItemQueue.front();
	mDeadItemQueue.pop();
	obj->Initialize();
	AddGameObject(obj);
	mCurItemCnt++;
}

void roka::Scene::DeleteObjects()
{
	while (mDeleteQueue.empty() == false)
	{
		GameObject* obj = mDeleteQueue.front();
		mDeleteQueue.pop();
		RemoveGameObject(obj);

		delete obj;
	}
}

void roka::Scene::RemoveGameObject(GameObject* obj)
{
	for (int i = 0; i < mGameObjects.size(); i++)
	{
		if (mGameObjects[i] == obj)
		{
			mGameObjects.erase(mGameObjects.begin() + i);
			break;
		}
	}
}
