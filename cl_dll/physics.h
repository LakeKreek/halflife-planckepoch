#pragma once

typedef struct
{
	float x, y, z;
}Vector3;


typedef struct PhsicsAPI_s
{
	/*

	*/
	void(_stdcall* Test)();

	/*
	<summary>
				Init physics system
				if the struct layout is different from default layout, that will throw a fatal error.
				</summary><param name="pStudioRenderer">the address of StudioModelRenderer's first field. (m_clTime)</param><param name="lastFieldAddress">&gt;the address of StudioModelRenderer's last field. (m_plighttransform)</param><param name="engineStudioAPI">pIEngineStudio</param>
	*/
	void(_stdcall* InitSystem)(void* pStudioRenderer, void* lastFieldAddress, void* engineStudioAPI);

	/*
	<summary>
				Load map geomitry collider.
				</summary><param name="mapName"></param>
	*/
	void(_stdcall* ChangeLevel)(const char* mapName);

	/*
	<summary>
				��ͼ���䣬�������ã���������Ϸ�ж�̬�����ĸ���CollisionObjects
				cs��ÿһ�ֽ������Ե���
				</summary>
	*/
	void(_stdcall* LevelReset)();

	/*
	<summary>
				Physics world update
				</summary><param name="delta"></param>
	*/
	void(_stdcall* Update)(float delta);

	/*

	*/
	void(_stdcall* Pause)();

	/*

	*/
	void(_stdcall* Resume)();

	/*
	<summary>
				Close physics system and release physics resources.
				</summary>
	*/
	void(_stdcall* ShotDown)();

	/*
	<summary>
				Show configration form.
				Using cvar to call this is recommended.
				</summary>
	*/
	void(_stdcall* ShowConfigForm)();

	/*

	*/
	void(_stdcall* CreateRagdollController)(int entityId, const char* modelName);

	/*

	*/
	void(_stdcall* CreateRagdollControllerIndex)(int entityId, int index);

	/*

	*/
	void(_stdcall* CreateRagdollControllerHeader)(int entityId, void* hdr);

	/*

	*/
	void(_stdcall* StartRagdoll)(int entityId);

	/*

	*/
	void(_stdcall* StopRagdoll)(int entityId);

	/*

	*/
	void(_stdcall* SetupBonesPhysically)(int entityId);

	/*

	*/
	void(_stdcall* ChangeOwner)(int oldEntity, int newEntity);

	/*

	*/
	void(_stdcall* SetVelocity)(int entityId, Vector3* v);

	/*

	*/
	void(_stdcall* DisposeRagdollController)(int entityId);

	/*
	<summary>
				Set an explosion on the specified position.
				The impact range is calculated automatically via intensity.
				</summary><param name="pos"></param><param name="intensity"></param>
	*/
	void(_stdcall* Explosion)(Vector3* pos, float intensity);

	/*
	<summary>
				Shoot an invisable bullet to apply impulse to the rigidbody it hits.
				</summary><param name="from">eye pos.</param><param name="force">contains direction and intensity.</param>
	*/
	void(_stdcall* Shoot)(Vector3* from, Vector3* force);

	/*

	*/
	void(_stdcall* PickBody)();

	/*

	*/
	void(_stdcall* ReleaseBody)();

}PhsicsAPI;


// Containts all the physics system API
// Call [InitPhysicsInterface] before using these API
extern PhsicsAPI gPhysics;

// Call this function to initialize [gPhysics].
extern "C" void InitPhysicsInterface(char* msg);