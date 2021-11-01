//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
// New clipping style camera - original idea by Xwider
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "hud.h"
#include "cl_util.h"
#include "camera.h"
#include "in_defs.h"

extern "C" 
{
	void DLLEXPORT CAM_Think( void );
	int DLLEXPORT CL_IsThirdPerson( void );
	void DLLEXPORT CL_CameraOffset( float *ofs );
}

int iMouseInUse = 0;

void DLLEXPORT CAM_Think()
{
}

void CAM_Init()
{
	Vector		boxmins, boxmaxs;// enclose the test object along entire move
	float		*mins, *maxs;	// size of the moving object
	Vector		mins2, maxs2;	// size when clipping against mosnters
	float		*start, *end;
	trace_t		trace;
	int			type;
	edict_t		*passedict;
	qboolean	monsterclip;
} moveclip_t;

extern trace_t SV_ClipMoveToEntity (edict_t *ent, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end);

void DLLEXPORT CAM_Think( void )
{
//	RecClCamThink();

	vec3_t origin;
	vec3_t ext, pnt, camForward, camRight, camUp;
	moveclip_t	clip;
	float dist;
	vec3_t camAngles;
	float flSensitivity;
#ifdef LATER
	int i;
#endif
	vec3_t viewangles;

	switch( (int) cam_command->value )
	{
		case CAM_COMMAND_TOTHIRDPERSON:
			CAM_ToThirdPerson();
			break;

		case CAM_COMMAND_TOFIRSTPERSON:
			CAM_ToFirstPerson();
			break;

		case CAM_COMMAND_NONE:
		default:
			break;
	}

	if( !cam_thirdperson )
		return;
	
#ifdef LATER
	if ( cam_contain->value )
	{
		gEngfuncs.GetClientOrigin( origin );
		ext[0] = ext[1] = ext[2] = 0.0;
	}
#endif

	camAngles[ PITCH ] = cam_idealpitch->value;
	camAngles[ YAW ] = cam_idealyaw->value;
	dist = cam_idealdist->value;
	//
	//movement of the camera with the mouse
	//
	if (cam_mousemove)
	{
	    //get windows cursor position
		SDL_GetCursorPos (&cam_mouse);
		//check for X delta values and adjust accordingly
		//eventually adjust YAW based on amount of movement
	  //don't do any movement of the cam using YAW/PITCH if we are zooming in/out the camera	
	  if (!cam_distancemove)
	  {
		
		//keep the camera within certain limits around the player (ie avoid certain bad viewing angles)  
		if (cam_mouse.x>gEngfuncs.GetWindowCenterX())
		{
			//if ((camAngles[YAW]>=225.0)||(camAngles[YAW]<135.0))
			if (camAngles[YAW]<c_maxyaw->value)
			{
				camAngles[ YAW ] += (CAM_ANGLE_MOVE)*((cam_mouse.x-gEngfuncs.GetWindowCenterX())/2);
			}
			if (camAngles[YAW]>c_maxyaw->value)
			{
				
				camAngles[YAW]=c_maxyaw->value;
			}
		}
		else if (cam_mouse.x<gEngfuncs.GetWindowCenterX())
		{
			//if ((camAngles[YAW]<=135.0)||(camAngles[YAW]>225.0))
			if (camAngles[YAW]>c_minyaw->value)
			{
			   camAngles[ YAW ] -= (CAM_ANGLE_MOVE)* ((gEngfuncs.GetWindowCenterX()-cam_mouse.x)/2);
			   	
			}
			if (camAngles[YAW]<c_minyaw->value)
			{
				camAngles[YAW]=c_minyaw->value;
				
			}
		}

		//check for y delta values and adjust accordingly
		//eventually adjust PITCH based on amount of movement
		//also make sure camera is within bounds
		if (cam_mouse.y>gEngfuncs.GetWindowCenterY())
		{
			if(camAngles[PITCH]<c_maxpitch->value)
			{
			    camAngles[PITCH] +=(CAM_ANGLE_MOVE)* ((cam_mouse.y-gEngfuncs.GetWindowCenterY())/2);
			}
			if (camAngles[PITCH]>c_maxpitch->value)
			{
				camAngles[PITCH]=c_maxpitch->value;
			}
		}
		else if (cam_mouse.y<gEngfuncs.GetWindowCenterY())
		{
			if (camAngles[PITCH]>c_minpitch->value)
			{
			   camAngles[PITCH] -= (CAM_ANGLE_MOVE)*((gEngfuncs.GetWindowCenterY()-cam_mouse.y)/2);
			}
			if (camAngles[PITCH]<c_minpitch->value)
			{
				camAngles[PITCH]=c_minpitch->value;
			}
		}

		//set old mouse coordinates to current mouse coordinates
		//since we are done with the mouse

		if ( ( flSensitivity = gHUD.GetSensitivity() ) != 0 )
		{
			cam_old_mouse_x=cam_mouse.x*flSensitivity;
			cam_old_mouse_y=cam_mouse.y*flSensitivity;
		}
		else
		{
			cam_old_mouse_x=cam_mouse.x;
			cam_old_mouse_y=cam_mouse.y;
		}
		SDL_SetCursorPos (gEngfuncs.GetWindowCenterX(), gEngfuncs.GetWindowCenterY());
	  }
	}

	//Nathan code here
	if( CL_KeyState( &cam_pitchup ) )
		camAngles[ PITCH ] += CAM_ANGLE_DELTA;
	else if( CL_KeyState( &cam_pitchdown ) )
		camAngles[ PITCH ] -= CAM_ANGLE_DELTA;

	if( CL_KeyState( &cam_yawleft ) )
		camAngles[ YAW ] -= CAM_ANGLE_DELTA;
	else if( CL_KeyState( &cam_yawright ) )
		camAngles[ YAW ] += CAM_ANGLE_DELTA;

	if( CL_KeyState( &cam_in ) )
	{
		dist -= CAM_DIST_DELTA;
		if( dist < CAM_MIN_DIST )
		{
			// If we go back into first person, reset the angle
			camAngles[ PITCH ] = 0;
			camAngles[ YAW ] = 0;
			dist = CAM_MIN_DIST;
		}

	}
	else if( CL_KeyState( &cam_out ) )
		dist += CAM_DIST_DELTA;

	if (cam_distancemove)
	{
		if (cam_mouse.y>gEngfuncs.GetWindowCenterY())
		{
			if(dist<c_maxdistance->value)
			{
			    dist +=CAM_DIST_DELTA * ((cam_mouse.y-gEngfuncs.GetWindowCenterY())/2);
			}
			if (dist>c_maxdistance->value)
			{
				dist=c_maxdistance->value;
			}
		}
		else if (cam_mouse.y<gEngfuncs.GetWindowCenterY())
		{
			if (dist>c_mindistance->value)
			{
			   dist -= (CAM_DIST_DELTA)*((gEngfuncs.GetWindowCenterY()-cam_mouse.y)/2);
			}
			if (dist<c_mindistance->value)
			{
				dist=c_mindistance->value;
			}
		}
		//set old mouse coordinates to current mouse coordinates
		//since we are done with the mouse
		cam_old_mouse_x=cam_mouse.x*gHUD.GetSensitivity();
		cam_old_mouse_y=cam_mouse.y*gHUD.GetSensitivity();
		SDL_SetCursorPos (gEngfuncs.GetWindowCenterX(), gEngfuncs.GetWindowCenterY());
	}
#ifdef LATER
	if( cam_contain->value )
	{
		// check new ideal
		VectorCopy( origin, pnt );
		AngleVectors( camAngles, camForward, camRight, camUp );
		for (i=0 ; i<3 ; i++)
			pnt[i] += -dist*camForward[i];

		// check line from r_refdef.vieworg to pnt
		memset ( &clip, 0, sizeof ( moveclip_t ) );
		clip.trace = SV_ClipMoveToEntity( sv.edicts, r_refdef.vieworg, ext, ext, pnt );
		if( clip.trace.fraction == 1.0 )
		{
			// update ideal
			cam_idealpitch->value = camAngles[ PITCH ];
			cam_idealyaw->value = camAngles[ YAW ];
			cam_idealdist->value = dist;
		}
	}
	else
#endif
	{
		// update ideal
		cam_idealpitch->value = camAngles[ PITCH ];
		cam_idealyaw->value = camAngles[ YAW ];
		cam_idealdist->value = dist;
	}

	// Move towards ideal
	VectorCopy( cam_ofs, camAngles );

	gEngfuncs.GetViewAngles( (float *)viewangles );

	if( cam_snapto->value )
	{
		camAngles[ YAW ] = cam_idealyaw->value + viewangles[ YAW ];
		camAngles[ PITCH ] = cam_idealpitch->value + viewangles[ PITCH ];
		camAngles[ 2 ] = cam_idealdist->value;
	}
	else
	{
		if( camAngles[ YAW ] - viewangles[ YAW ] != cam_idealyaw->value )
			camAngles[ YAW ] = MoveToward( camAngles[ YAW ], cam_idealyaw->value + viewangles[ YAW ], CAM_ANGLE_SPEED );

		if( camAngles[ PITCH ] - viewangles[ PITCH ] != cam_idealpitch->value )
			camAngles[ PITCH ] = MoveToward( camAngles[ PITCH ], cam_idealpitch->value + viewangles[ PITCH ], CAM_ANGLE_SPEED );

		if( fabs( camAngles[ 2 ] - cam_idealdist->value ) < 2.0 )
			camAngles[ 2 ] = cam_idealdist->value;
		else
			camAngles[ 2 ] += ( cam_idealdist->value - camAngles[ 2 ] ) / 4.0;
	}
#ifdef LATER
	if( cam_contain->value )
	{
		// Test new position
		dist = camAngles[ ROLL ];
		camAngles[ ROLL ] = 0;

		VectorCopy( origin, pnt );
		AngleVectors( camAngles, camForward, camRight, camUp );
		for (i=0 ; i<3 ; i++)
			pnt[i] += -dist*camForward[i];

		// check line from r_refdef.vieworg to pnt
		memset ( &clip, 0, sizeof ( moveclip_t ) );
		ext[0] = ext[1] = ext[2] = 0.0;
		clip.trace = SV_ClipMoveToEntity( sv.edicts, r_refdef.vieworg, ext, ext, pnt );
		if( clip.trace.fraction != 1.0 )
			return;
	}
#endif
	cam_ofs[ 0 ] = camAngles[ 0 ];
	cam_ofs[ 1 ] = camAngles[ 1 ];
	cam_ofs[ 2 ] = dist;
}

int DLLEXPORT CL_IsThirdPerson()
{
	return (gHUD.m_iCameraMode ? 1 : 0) || (g_iUser1 && (g_iUser2 == gEngfuncs.GetLocalPlayer()->index) );
}

void DLLEXPORT CL_CameraOffset( float *ofs )
{
	VectorCopy( vec3_origin, ofs );
}
