/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#pragma once

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <AvgEngine/Base/Camera.h>
#include <AvgEngine/Utils/TweenManager.h>
#include <AvgEngine/EventManager.h>
#include <algorithm>

namespace AvgEngine::Base
{
	/**
	 * \brief A base class for AvgEngine Game Objects
	 */
	class GameObject
	{
	private:
		int lastObjectId = 0;
	public:

		Events::EventManager* eManager = NULL;

		TweenManager* tween{};

		Camera* camera{};

		bool drawn = true;

		bool hitbox = false;

		bool center = false;

		/**
		 * \brief If the transform should base itself on percentages of the display's width/height
		 */
		bool transformRatio = false;

		bool dontDelete = false;
		Render::Rect iTransform = Render::Rect();
		Render::Rect transform = Render::Rect();
		Render::Rect* parentClip = NULL;
		bool clipAll = true;
		Render::Rect clipRect = Render::Rect();

		Render::Rect transformOffset = Render::Rect();
		Render::Rect* parent = NULL;
		Render::Rect* parentI = NULL;

		std::string tag = "object";

		std::vector<GameObject*> Children;

		int id = 0;
		int zIndex = 0;

		bool render = true;

		GameObject(Render::Rect _transform)
		{
			transform = _transform;
		}

		GameObject(float x, float y)
		{
			transform = Render::Rect();
			transform.x = x;
			transform.y = y;
		}

		void SetColor(float r, float g, float b)
		{
			transform.r = r;
			transform.g = g;
			transform.b = b;
		}

		virtual ~GameObject()
		{
			if (!dontDelete)
			{
				for (GameObject* o : Children)
					delete o;
			}
		};

		GameObject() = default;

		virtual void draw();

		virtual void drawTopZIndex()
		{
			for (GameObject* ob : Children)
			{
				// Render objects' draw calls.
				if (ob->render && zIndex + ob->zIndex > zIndex)
				{
					Render::Rect* cr = &clipRect;
					if (parentClip != NULL && (cr->w == 0 && cr->h == 0))
						cr = parentClip;
					if (cr->w != 0 || cr->h != 0)
						ob->parentClip = cr;
					else
						ob->parentClip = NULL;
					ob->camera = camera;
					ob->parent = &transform;
					int oldZ = ob->zIndex;
					ob->zIndex += zIndex;
					ob->draw();
					ob->zIndex = oldZ;
				}
			}
		};

		virtual void setRatio(bool ratio)
		{
			transformRatio = ratio;
		}

		bool operator==(const GameObject& other) {
			return id == other.id;
		}


		virtual void Added()
		{
			
		}

		/**
		 * \brief Creates a copy of an object and puts the copy onto a vector with a given id
		 * \param object Object to copy onto the vector
		 */
		virtual void addObject(GameObject* object)
		{
			object->tween = tween;
			object->id = lastObjectId;
			object->eManager = eManager;
			object->camera = camera;
			object->parent = &transform;
			object->parentI = &iTransform;
			object->Added();
			Children.push_back(object);
			lastObjectId++;
		}

		/**
		 * \brief Removes an object
		 * \param object The object to remove
		 */
		virtual void removeObject(GameObject* object)
		{
			for (GameObject* g : Children)
				if (g->id == object->id)
					Children.erase(std::ranges::remove(Children, g).begin(), Children.end());
		}

		/**
		 * \brief Removes an object
		 * \param id The id of the object to remove
		 */
		virtual void removeObject(int id)
		{
			for (GameObject* g : Children)
				if (g->id == id)
					Children.erase(std::ranges::remove(Children, g).begin(), Children.end());
		}


		virtual void removeAll()
		{
			for (GameObject* o : Children)
				delete o;
			Children.clear();
		}
	};
}

#endif // !GAMEOBJECT_H