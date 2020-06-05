#include "GameSystemScene.h"
#include <IRigidBody.h>
#include <algorithm>
#include <StringHelpers.h>
#include <EngineFolders.h>
//FOR AI PROJECT 03
#include <BMPImage.h>
#include <IRender.h>
#include <ISystemAI.h>
#include <BellEngineID.h>

void GameSystemScene::UpdateScene1Rules()
{
	if (!currentScene) return;

	if (currentScene->GetName() == "Scene1")
	{
		
	}
}

void GameSystemScene::InitializeScene01(Scene* scene1)
{
	if (scene1->GetName() == "Scene1")
	{
		GetScene01Objects(scene1);
		CompleteScene01(scene1);
	}
}

Scene* GameSystemScene::GetDepositScene()
{
	return this->deposit_scene;
}

void GameSystemScene::PutSphere(Entity* where)
{
	if (!where) return;
	
	Entity* sphere = deposit_scene->FindEntityByName("SPHERE");
	if (sphere)
	{
		sphere->GetComponent<IRigidbody*>()->SetPosition(where->GetComponent<IRigidbody*>()->GetPosition());
	}
}

void GameSystemScene::BindOnExternalMessage(std::function<void(const std::string&, const double&, const unsigned int& renderAs)> fn)
{
	if (!fn) return;

	this->onExternalMessage = std::bind(fn, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

void GameSystemScene::GetScene01Objects(Scene* scene1)
{
	if (deposit_scene && scene1)
	{
		Entity* thescentShip = scene1->FindEntityByName("THESCENT_SHIP");
		Entity* exitCube = scene1->FindEntityByName("EXIT_CUBE");
		Entity* bossBody = scene1->FindEntityByName("ENEMY_BOSS_BODY");
		Entity* bossTop = scene1->FindEntityByName("ENEMY_BOSS_TURRET_TOP");
		Entity* bossBottom = scene1->FindEntityByName("ENEMY_BOSS_TURRET_BOTTOM");

		Entity* corridorZ_0 = scene1->FindEntityByName("PLANE_10x10_CORRIDOR1_DOWN");
		Entity* corridorZ_1 = scene1->FindEntityByName("PLANE_10x10_CORRIDOR1_UP");
		Entity* corridorZ_2 = scene1->FindEntityByName("PLANE_10x10_CORRIDOR1_WALL");		
		Entity* mediumroom_0 = scene1->FindEntityByName("PLANE_10x10_ROOM_LEFT");
		Entity* mediumroom_1 = scene1->FindEntityByName("PLANE_10x10_ROOM_RIGHT");
		Entity* mediumroom_2 = scene1->FindEntityByName("PLANE_10x10_ROOM_BACK");
		Entity* mediumroom_3 = scene1->FindEntityByName("PLANE_100x20_PANEL_WALL01");
		Entity* glass_panel = scene1->FindEntityByName("PLANE_20x20_GLASS_FRONT");
		Entity* sphere = scene1->FindEntityByName("SPHERE");
		Entity* projectile_blaster = scene1->FindEntityByName("PROJECTILE_BLASTER");
		Entity* plane20_20_panelwall01 = scene1->FindEntityByName("PLANE_20x20_PANEL_WALL01");

		scene1->Remove(thescentShip, false);
		scene1->Remove(exitCube, false);
		scene1->Remove(bossBody, false);
		scene1->Remove(bossTop, false);
		scene1->Remove(bossBottom, false);
		scene1->Remove(corridorZ_0, false);
		scene1->Remove(corridorZ_1, false);
		scene1->Remove(corridorZ_2, false);		
		scene1->Remove(mediumroom_0, false);
		scene1->Remove(mediumroom_1, false);
		scene1->Remove(mediumroom_2, false);
		scene1->Remove(mediumroom_3, false);
		scene1->Remove(glass_panel, false);
		scene1->Remove(sphere, false);
		scene1->Remove(projectile_blaster, false);
		scene1->Remove(plane20_20_panelwall01, false);

		deposit_scene->Append(thescentShip);
		deposit_scene->Append(exitCube);
		deposit_scene->Append(bossBody);
		deposit_scene->Append(bossTop);
		deposit_scene->Append(bossBottom);
		deposit_scene->Append(corridorZ_0);
		deposit_scene->Append(corridorZ_1);
		deposit_scene->Append(corridorZ_2);		
		deposit_scene->Append(mediumroom_0);
		deposit_scene->Append(mediumroom_1);
		deposit_scene->Append(mediumroom_2);
		deposit_scene->Append(mediumroom_3);
		deposit_scene->Append(glass_panel);
		deposit_scene->Append(sphere);
		deposit_scene->Append(projectile_blaster);
		deposit_scene->Append(plane20_20_panelwall01);
	}
}

void GameSystemScene::CompleteScene01(Scene* scene1)
{
	if (!scene1) return;

	Entity* corridor1[4], * plane20_20_panelwindow01, *plane20_20_panelwall01, *PLANE_100x20_PANEL_WALL01;
	corridor1[0] = deposit_scene->FindEntityByName("PLANE_10x10_CORRIDOR1_DOWN");
	corridor1[1] = deposit_scene->FindEntityByName("PLANE_10x10_CORRIDOR1_UP");
	corridor1[2] = deposit_scene->FindEntityByName("PLANE_10x10_CORRIDOR1_WALL");
		
	PLANE_100x20_PANEL_WALL01 = deposit_scene->FindEntityByName("PLANE_100x20_PANEL_WALL01");
	plane20_20_panelwindow01 = deposit_scene->FindEntityByName("PLANE_20x20_GLASS_FRONT");
	plane20_20_panelwall01 = deposit_scene->FindEntityByName("PLANE_20x20_PANEL_WALL01");

	std::vector<Entity*> clones;

	/*First corridor*/	
	{
		/*Left*/
		Entity* clone = (Entity*)corridor1[2]->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(8.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, 90.0f, 0.0f));
				rb->SetPosition(Vector3D(-10.0f, 10.0f, 70.0f));
				rb->SetScale(Vector3D(8.0f, 1.0f, 1.0f));
			}

			clones.push_back(clone);
		}

		clone = (Entity*)corridor1[2]->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(8.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, -90.0f, 0.0f));
				rb->SetPosition(Vector3D(10.0f, 10.0f, 70.0f));
				rb->SetScale(Vector3D(8.0f, 1.0f, 1.0f));
			}

			clones.push_back(clone);
		}

		clone = (Entity*)corridor1[0]->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(1.0f, 8.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(0.0f, 0.0f, 0.0f));
				rb->SetPosition(Vector3D(0.0f, 0.0f, 70.0f));
				rb->SetScale(Vector3D(1.0f, 1.0f, 8.0f));
			}

			clones.push_back(clone);
		}

		clone = (Entity*)corridor1[1]->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(1.0f, 8.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(180.0f, 0.0f, 0.0f));
				rb->SetPosition(Vector3D(0.0f, 20.0f, 70.0f));
				rb->SetScale(Vector3D(1.0f, 1.0f, 8.0f));
			}

			clones.push_back(clone);
		}
	}

	/*Second corridor*/	
	{
		/*Left*/
		Entity* clone = (Entity*)corridor1[2]->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(4.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, 180.0f, 0.0f));
				rb->SetPosition(Vector3D(-90.0f, 10.0f, -90.0f));
				rb->SetScale(Vector3D(4.0f, 1.0f, 1.0f));
			}

			clones.push_back(clone);
		}

		clone = (Entity*)corridor1[2]->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(4.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, 0.0f, 0.0f));
				rb->SetPosition(Vector3D(-90.0f, 10.0f, -110.0f));
				rb->SetScale(Vector3D(4.0f, 1.0f, 1.0f));
			}

			clones.push_back(clone);
		}

		clone = (Entity*)corridor1[0]->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(1.0f, 4.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(0.0f, 90.0f, 0.0f));
				rb->SetPosition(Vector3D(-90.0f, 0.0f, -100.0f));
				rb->SetScale(Vector3D(1.0f, 1.0f, 4.0f));
			}

			clones.push_back(clone);
		}

		clone = (Entity*)corridor1[1]->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(1.0f, 4.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(180.0f, 90.0f, 0.0f));
				rb->SetPosition(Vector3D(-90.0f, 20.0f, -100.0f));
				rb->SetScale(Vector3D(1.0f, 1.0f, 4.0f));
			}

			clones.push_back(clone);
		}
	}
	
	/*Third corridor*/
	{
		/*Left*/
		Entity* clone = (Entity*)corridor1[2]->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(4.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, 180.0f, 0.0f));
				rb->SetPosition(Vector3D(90.0f, 10.0f, -90.0f));
				rb->SetScale(Vector3D(4.0f, 1.0f, 1.0f));
			}

			clones.push_back(clone);
		}

		clone = (Entity*)corridor1[2]->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(4.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, 0.0f, 0.0f));
				rb->SetPosition(Vector3D(90.0f, 10.0f, -110.0f));
				rb->SetScale(Vector3D(4.0f, 1.0f, 1.0f));
			}

			clones.push_back(clone);
		}

		clone = (Entity*)corridor1[0]->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(1.0f, 4.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(0.0f, 90.0f, 0.0f));
				rb->SetPosition(Vector3D(90.0f, 0.0f, -100.0f));
				rb->SetScale(Vector3D(1.0f, 1.0f, 4.0f));
			}

			clones.push_back(clone);
		}

		clone = (Entity*)corridor1[1]->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(1.0f, 4.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(180.0f, 90.0f, 0.0f));
				rb->SetPosition(Vector3D(90.0f, 20.0f, -100.0f));
				rb->SetScale(Vector3D(1.0f, 1.0f, 4.0f));
			}

			clones.push_back(clone);
		}
	}

	/*Key room left walls*/
	{
		Entity* clone = (Entity*)PLANE_100x20_PANEL_WALL01->Clone();
		if (clone)
		{
			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, 90.0f, 0.0f));

				Vector3D currPos = Vector3D(-180.0, 10.0, -100.0);				
				currPos.x -= 50.0f;
				rb->SetPosition(currPos);
			}

			clones.push_back(clone);
		}
	}
		
	/*Key room right walls*/
	{
		Entity* clone = (Entity*)PLANE_100x20_PANEL_WALL01->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(2.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, -90.0f, 0.0f));
				rb->SetScale(Vector3D(0.4f, 1.0f, 1.0f));

				Vector3D currPos = Vector3D(-180.0, 10.0, -70.0);
				currPos.x += 50.0f;
				rb->SetPosition(currPos);
			}

			clones.push_back(clone);
		}

		clone = (Entity*)PLANE_100x20_PANEL_WALL01->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(2.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, -90.0f, 0.0f));
				rb->SetScale(Vector3D(0.4f, 1.0f, 1.0f));

				Vector3D currPos = Vector3D(-180.0, 10.0, -130.0f);
				currPos.x += 50.0f;
				rb->SetPosition(currPos);
			}

			clones.push_back(clone);
		}
	}

	/*Key room back*/
	{
		Entity* clone = (Entity*)PLANE_100x20_PANEL_WALL01->Clone();
		if (clone)
		{
			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, 180.0f, 0.0f));
				rb->SetPosition(Vector3D(-180.0, 10.0, -50.0));
			}

			clones.push_back(clone);
		}
	}
		
	/*Key room front glass*/
	{
		Entity* clone = (Entity*)plane20_20_panelwindow01->Clone();
		if (clone)
		{
			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, 0.0f, 0.0f));
				rb->SetPosition(Vector3D(-180.0, 10.0, -150.0));
			}

			clones.push_back(clone);
		}
	}

	/*Rescue room right walls*/
	{
		Entity* clone = (Entity*)PLANE_100x20_PANEL_WALL01->Clone();
		if (clone)
		{
			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, -90.0f, 0.0f));

				Vector3D currPos = Vector3D(180.0, 10.0, -100.0);
				currPos.x += 50.0f;
				rb->SetPosition(currPos);
			}

			clones.push_back(clone);
		}
	}
		
	/*Rescue room left walls*/
	{
		Entity* clone = (Entity*)PLANE_100x20_PANEL_WALL01->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(2.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, 90.0f, 0.0f));
				rb->SetScale(Vector3D(0.4f, 1.0f, 1.0f));

				Vector3D currPos = Vector3D(180.0, 10.0, -70.0);
				currPos.x -= 50.0f;
				rb->SetPosition(currPos);
			}

			clones.push_back(clone);
		}

		clone = (Entity*)PLANE_100x20_PANEL_WALL01->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(2.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, 90.0f, 0.0f));
				rb->SetScale(Vector3D(0.4f, 1.0f, 1.0f));

				Vector3D currPos = Vector3D(180.0, 10.0, -130.0f);
				currPos.x -= 50.0f;
				rb->SetPosition(currPos);
			}

			clones.push_back(clone);
		}
	}

	/*Rescue room back*/
	{
		Entity* clone = (Entity*)PLANE_100x20_PANEL_WALL01->Clone();
		if (clone)
		{
			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, 180.0f, 0.0f));
				rb->SetPosition(Vector3D(180.0, 10.0, -50.0));
			}

			clones.push_back(clone);
		}
	}

	/*Rescue room front glass*/
	{
		Entity* clone = (Entity*)plane20_20_panelwindow01->Clone();
		if (clone)
		{
			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, 0.0f, 0.0f));
				rb->SetPosition(Vector3D(180.0, 10.0, -150.0));
			}

			clones.push_back(clone);
		}
	}

	/*Main room left*/
	{
		Entity* clone = (Entity*)plane20_20_panelwall01->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(10.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, 90.0f, 0.0f));
				rb->SetScale(Vector3D(10.0f, 1.0f, 1.0f));

				Vector3D currPos = Vector3D(-50.0f, 10.0, -210.0);
				rb->SetPosition(currPos);
			}

			clones.push_back(clone);
		}

		clone = (Entity*)plane20_20_panelwall01->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(4.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetPosition(Vector3D(-50.0f, 10.0, -50.0));
				rb->SetRotation(Vector3D(90.0f, 90.0f, 0.0f));
				rb->SetScale(Vector3D(4.0f, 1.0f, 1.0f));
			}

			clones.push_back(clone);
		}
	}
	
	/*Main room right*/
	{
		Entity* clone = (Entity*)plane20_20_panelwall01->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(10.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetRotation(Vector3D(90.0f, -90.0f, 0.0f));
				rb->SetScale(Vector3D(10.0f, 1.0f, 1.0f));

				Vector3D currPos = Vector3D(50.0f, 10.0, -210.0);
				rb->SetPosition(currPos);
			}

			clones.push_back(clone);
		}

		clone = (Entity*)plane20_20_panelwall01->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(4.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetPosition(Vector3D(50.0f, 10.0, -50.0));
				rb->SetRotation(Vector3D(90.0f, -90.0f, 0.0f));
				rb->SetScale(Vector3D(4.0f, 1.0f, 1.0f));
			}

			clones.push_back(clone);
		}
	}

	/*Main room back*/
	{
		Entity* clone = (Entity*)plane20_20_panelwall01->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(2.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetPosition(Vector3D(-30.0f, 10.0, -10.0));
				rb->SetRotation(Vector3D(90.0f, 180.0f, 0.0f));
				rb->SetScale(Vector3D(2.0f, 1.0f, 1.0f));
			}

			clones.push_back(clone);
		}

		clone = (Entity*)plane20_20_panelwall01->Clone();
		if (clone)
		{
			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				ShaderFeature* texturize = vr->GetShaderFeature("TEXTURIZE");
				if (texturize)
				{
					texturize->parameters.SetValue("TILING", StringHelpers::Vec2ToStr(Vector2D(2.0f, 1.0f)));
				}
			}

			/*Shift along Z*/
			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				rb->SetPosition(Vector3D(30.0f, 10.0, -10.0));
				rb->SetRotation(Vector3D(90.0f, 180.0f, 0.0f));
				rb->SetScale(Vector3D(2.0f, 1.0f, 1.0f));
			}

			clones.push_back(clone);
		}
	}

	/*Adds cloned items to scene*/
	for (Entity* clone : clones) scene1->Append(clone);
}

GameSystemScene::GameSystemScene()
{
	this->isUpdateComplete = false;
	this->isActive = true;
	this->initializedScenePtrs = false;

	this->currentScene = nullptr;
	this->deposit_scene = nullptr;
}

GameSystemScene::~GameSystemScene()
{
	//Dispose();
}

const bool GameSystemScene::IsUpdateComplete()
{
	return this->isUpdateComplete;
}

const bool GameSystemScene::Initialize(ParameterContainer& input)
{
	this->deposit_scene = new Scene();


	Parameter p;
	bool rc = false;

	//rc = input.FindParameterByName("aisystem", p);
	//if (!rc) return false;
	//systemAI = (ISystemAI*)p.valuePtr;
	//if (!systemAI) return false;

	return true;
}

const bool GameSystemScene::IsActive()
{
	return this->isActive;
}

void GameSystemScene::Activate(const bool& value)
{
	this->isActive = value;
}

void GameSystemScene::SetScene(Scene* scene)
{
	currentScene = scene;	
}

void GameSystemScene::Update(const FrameTime& frameTime)
{
	if (!this->isActive) return;
	this->isUpdateComplete = false;

	SetFrameTime(frameTime);

	this->isUpdateComplete = true;
}

void GameSystemScene::Dispose()
{
	DisposePointer(deposit_scene);
}
