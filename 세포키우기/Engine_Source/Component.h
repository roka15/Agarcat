#pragma once
#include "Entity.h"
namespace roka
{
	class Component:public Entity
	{
	public:
		Component();
		virtual ~Component();

		virtual void Initialize();
		virtual void Update();
		virtual void LateUpdate();
		virtual void Render();

	private:
		const roka::enums::EComponentType mType;
	};
}


