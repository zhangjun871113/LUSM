// gdef.h
#pragma once
#define SAMPLE_MAX				65536u
#define CHANNEL_MAX				5u
#define INDEX_MAX				256u
#define INDEX_SET_START			1u
#define INDEX_CONTROL_START		90u
#define INDEX_METHOD_START		100u
#define INDEX_STATUS_START		200u
//
/**		@group set value index */
#define INDEX_FREQ				1u
#define INDEX_DUTYA				2u
#define INDEX_DUTYB				3u
#define INDEX_PHASE				4u
#define INDEX_DUTYEQUAL			5u
#define INDEX_NUMBER			6u
#define INDEX_GROUP				7u
#define INDEX_INTERVAL			8u
#define INDEX_POSITION			9u
#define INDEX_SPEED				10u
#define INDEX_ZP				11u
#define INDEX_ZN				12u

/**		@group status */
#define INDEX_RUNSTATE			90u
#define INDEX_RUNMODE			91u
#define INDEX_CONNECT			92u
#define INDEX_CURVE				93u
#define INDEX_METHOD			94u

/**		@group method */
#define INDEX_PID_P				100u
#define INDEX_PID_I				101u
#define INDEX_PID_D				102u
#define INDEX_PID_MU			103u
#define INDEX_PID_LAMBDA		104u
#define INDEX_FUN_PARA0			150u
#define INDEX_FUN_PARA1			151u

/**		@group current value index */
#define INDEX_CURFREQ			200u
#define INDEX_CURDUTYA			201u
#define INDEX_CURDUTYB			202u
#define INDEX_CURGROUP			203u
#define INDEX_CURPOSITION		204u
#define INDEX_CURPHASE			205u

#define INDEX_DATA				255u		/* long data identifier */

/**********************************************/

/***************************************/

#define INDEX_RUNSTATE_STOP			0u
#define INDEX_RUNSTATE_CW			1u
#define INDEX_RUNSTATE_CCW			2u

#define INDEX_METHOD_NONE			0u
#define INDEX_METHOD_PID			1u
#define INDEX_METHOD_FOLLOW_SIN		2u


#define DEF_DISCONNECT				0u
#define DEF_CONNECT					1u

#define DEF_OFF						0u
#define DEF_ON						1u

#define MODE_IDLE					0u
#define MODE_MANUAL					1u
#define MODE_PULSE					2u
#define MODE_MOHE					3u
#define MODE_POSITION				4u
#define MODE_MONITOR				5u


typedef union DataUnion {
	UINT32	data32;
	UINT16	data16[2];
	UINT8	data8[4];
}  TYPE_UNION;
