#pragma once
#include "Entity.h"
#include "GameObject.h"


namespace roka
{
	class Scene : public Entity
	{
	public:
		Scene();
		virtual ~Scene();

		virtual void Initialize();
		virtual void Update();
		virtual void LateUpdate();
		virtual void Render();

		void AddGameObject(GameObject* obj) { mGameObjects.push_back(obj); }
		//SceneManager 구현 안해서 임시로 씀.
	    const std::vector<GameObject*>& GetObjects() { return mGameObjects; }
		void RegisterDeleteObject(GameObject* obj) { mDeleteQueue.push(obj); }
		void RegisterDeadItem(GameObject* obj) { mDeadItemQueue.push(obj); }
		void DeadItemErase(GameObject* obj);
		void ActiveItemOnce();
		void DeleteObjects();
		void RemoveGameObject(GameObject* obj);
	private:
		const UINT mObjectMaxSize;
		const UINT mItemMaxCount;
		UINT mCurItemCnt;
		std::vector<GameObject*> mGameObjects;
		std::queue<GameObject*> mDeleteQueue;
		std::queue<GameObject*> mDeadItemQueue;
	
		double mTime;
	};

}

