#include "LogoScale.h"

typedef struct {
	unsigned long	index;
	char			str[256];
} TableString;


TableString		g_strs[StrID_NUMTYPES] = {
	StrID_NONE,					"",
	StrID_Name,					"Logo Scale",
	StrID_Description,			"Logo Scale based on ffmpeg.\r"
								"Version:0.8.0(Prometheus Ver)\r"
								"This is a free beta version.\r"
								"DO NOT COPY to others!\r"
								"Download from logo_scale.mad.moe ONLY!\r"
								"Written By Logo+.\r"
								"Email:logo@mad.moe\r"
								"github:https://github.com/logoplus/logo_scale\r",
	StrID_Mode,					"Scale Mode",
	StrID_ModeChoices,			"FAST_BILINEAR|"
								"BILINEAR|"
								"BICUBIC|"
								"X|"
								"POINT|"
								"AREA|"
								"BICUBLIN|"
								"GAUSS|"
								"SINC|"
								"LANCZOS|"
								"SPLINE",
	StrID_Anchor,				"Scale Anchor Point",
	StrID_AnchorChoices,		"Layer Anchor Point|"
								"Upper Left|"
								"Upper Mid|"
								"Upper Right|"
								"Mid Left|"
								"Mid Center|"
								"Mid Right|"
								"Lower Left|"
								"Lower Mid|"
								"Lower Right",
	StrID_Percent,				"Scale Percent",
	StrID_DependString1,		"All Dependencies requested.",
	StrID_DependString2,		"Missing Dependencies requested.",
	StrID_Err_LoadSuite,		"Error loading suite.",
	StrID_Err_FreeSuite,		"Error releasing suite."
};

char *GetStringPtr(int strNum){
	return g_strs[strNum].str;
}

