#pragma once

#include "AEConfig.h"
#include "entry.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "AE_EffectCBSuites.h"
#include "String_Utils.h"
#include "AEGP_SuiteHandler.h"

#ifdef AE_OS_WIN
#include <Windows.h>
#endif

#include "LogoScale_Strings.h"

#define	MAJOR_VERSION			2
#define	MINOR_VERSION			0
#define	BUG_VERSION				0
#define	STAGE_VERSION			PF_Stage_DEVELOP
#define	BUILD_VERSION			1

#define PERCENT_MIN				0.0
#define PERCENT_MAX				1000.0
#define PERCENT_SLIDER_MIN		0.0
#define PERCENT_SLIDER_MAX		500.0
#define PERCENT_DFLT			100.0

#define LAYER_SIZE_MAX			30000


#define CHANNELS				4
#define ERR_ALLOC(s)			if(!err && (s)==NULL)err|=PF_Err_OUT_OF_MEMORY


enum{
	SCALE_INPUT=0,
	SCALE_MODE,
	SCALE_ANCHOR,
	SCALE_PERCENT,
	SCALE_NUM_PARAMS
};
#define ANCHOR_X(choise)	(((choise)-2)%3)
#define ANCHOR_X_LEFT		0
#define ANCHOR_X_MID		1
#define ANCHOR_X_RIGHT		2
#define ANCHOR_Y(choise)	(((choise)-2)/3)
#define ANCHOR_Y_UPPER		0
#define ANCHOR_Y_MID		1
#define ANCHOR_Y_LOWER		2
enum{
	ANCHOR_LAYER=1,
	ANCHOR_UPPER_LEFT,
	ANCHOR_UPPER_MID,
	ANCHOR_UPPER_RIGHT,
	ANCHOR_MID_LEFT,
	ANCHOR_MID_MID,
	ANCHOR_MID_RIGHT,
	ANCHOR_LOWER_LEFT,
	ANCHOR_LOWER_MID,
	ANCHOR_LOWER_RIGHT

};


#ifdef __cplusplus
extern "C"{
#endif
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
//#include <libavformat/avformat.h>
	DllExport PF_Err
		EntryPointFunc(
		PF_Cmd		cmd,
		PF_InData	*in_data,
		PF_OutData	*out_data,
		PF_ParamDef *params[],
		PF_LayerDef *output,
		void		*extra);
#ifdef __cplusplus
}
#endif
/*
typedef struct{
	A_long				width, height, target_width, target_height;
	AVPixelFormat		fmt;
	int					mode;
	SwsContext			*img_convert_ctx;
	AVFrame				*inFrame, *outFrame;
	uint8_t				*inBuffer, *outBuffer;
}FRAME_DATA;*/

static void PRINTXX1(A_long num, char *strt = NULL){
	if (num >= 100000){
		MessageBox(0, "too big", "1", 0);
		return;
	}
	if (num < 0){
		MessageBox(0, "too small", "2", 0);
		return;
	}
	char sss[10];
	sss[0] = (char)(num / 10000 + '0');
	num -= num / 10000 * 10000;
	sss[1] = (char)(num / 1000 + '0');
	num -= num / 1000 * 1000;
	sss[2] = (char)(num / 100 + '0');
	num -= num / 100 * 100;
	sss[3] = (char)(num / 10 + '0');
	sss[4] = (char)(num % 10 + '0');
	sss[5] = 0;
	MessageBox(NULL, strt, sss, 0);
}