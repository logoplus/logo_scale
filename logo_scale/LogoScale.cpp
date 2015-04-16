#include "LogoScale.h"
/*
by logo人间之表
QQ 346381522
仅供学习，盗用追究*/

//#include<iostream>
static PF_Err
About(
PF_InData		*in_data,
PF_OutData		*out_data,
PF_ParamDef		*params[],
PF_LayerDef		*output){

	AEGP_SuiteHandler suites(in_data->pica_basicP);
	suites.ANSICallbacksSuite1()->sprintf(	out_data->return_msg,
											"%s",
											STR(StrID_Description));
	return PF_Err_NONE;
}

static PF_Err
GlobalSetup(
PF_InData		*in_data,
PF_OutData		*out_data,
PF_ParamDef		*params[],
PF_LayerDef		*output){
	out_data->my_version =	PF_VERSION(MAJOR_VERSION,
							MINOR_VERSION,
							BUG_VERSION,
							STAGE_VERSION,
							BUILD_VERSION);
	out_data->out_flags =	PF_OutFlag_I_EXPAND_BUFFER|
							PF_OutFlag_USE_OUTPUT_EXTENT|
							PF_OutFlag_DEEP_COLOR_AWARE;
	return PF_Err_NONE;
}

static PF_Err
ParamsSetup(
PF_InData		*in_data,
PF_OutData		*out_data,
PF_ParamDef		*params[],
PF_LayerDef		*output){
	PF_Err		err = PF_Err_NONE;
	PF_ParamDef	def;

	AEFX_CLR_STRUCT(def);
	PF_ADD_POPUP(STR(StrID_Mode), 11, 10, STR(StrID_ModeChoices), SCALE_MODE);

	AEFX_CLR_STRUCT(def);
	PF_ADD_POPUP(STR(StrID_Anchor), 10, 6, STR(StrID_AnchorChoices), SCALE_ANCHOR);

	AEFX_CLR_STRUCT(def);
	PF_ADD_FLOAT_SLIDER(STR(StrID_Percent),
						PERCENT_MIN,
						PERCENT_MAX,
						PERCENT_SLIDER_MIN,
						PERCENT_SLIDER_MAX,
						AEFX_AUDIO_DEFAULT_CURVE_TOLERANCE,
						PERCENT_DFLT,
						2,
						PF_ValueDisplayFlag_PERCENT,
						1,
						SCALE_PERCENT);
	out_data->num_params = SCALE_NUM_PARAMS;

	return err;
}
const int mode_choices[] = {
	0,
	SWS_FAST_BILINEAR,
	SWS_BILINEAR,
	SWS_BICUBIC,
	SWS_X,
	SWS_POINT,
	SWS_AREA,
	SWS_BICUBLIN,
	SWS_GAUSS,
	SWS_SINC,
	SWS_LANCZOS,
	SWS_SPLINE
};

static PF_Err
FrameSetup(
	PF_InData	*in_data,
	PF_OutData	*out_data,
	PF_ParamDef	*params[],
	PF_LayerDef	*output){
	PF_Err				err				= PF_Err_NONE;
	PF_EffectWorld		*input			= &params[SCALE_INPUT]->u.ld;
	PF_FpLong			scale_percent	= params[SCALE_PERCENT]->u.fs_d.value / 100.0;
	AEGP_SuiteHandler	suites(in_data->pica_basicP);
	int anchor_mode = params[SCALE_ANCHOR]->u.pd.value;
	out_data->width = static_cast<A_long>(input->width*scale_percent);
	out_data->height = static_cast<A_long>(input->height*scale_percent);
	if (anchor_mode == ANCHOR_LAYER){
		AEGP_LayerH layer;
		AEGP_StreamVal	stream_val;
		AEFX_CLR_STRUCT(stream_val);
		A_Time	current_time = { in_data->current_time, in_data->time_step };
		ERR(suites.PFInterfaceSuite1()->AEGP_GetEffectLayer(in_data->effect_ref, &layer));
		ERR(suites.StreamSuite2()->AEGP_GetLayerStreamValue(layer,
															AEGP_LayerStream_ANCHORPOINT,
															AEGP_LTimeMode_LayerTime,
															&current_time,
															FALSE,
															&stream_val,
															NULL));
		//PRINTXX1(stream_val.two_d.x, "x");
		//PRINTXX1(stream_val.two_d.y, "y");
		out_data->origin.h = static_cast<A_long>(	(out_data->width - input->width)*
													stream_val.two_d.x*
													in_data->downsample_x.num/
													in_data->downsample_x.den/
													input->width);
		out_data->origin.v = static_cast<A_long>(	(out_data->height - input->height)*
													stream_val.two_d.y*
													in_data->downsample_y.num/
													in_data->downsample_y.den/ 
													input->height);
	}
	else{
		switch (ANCHOR_X(anchor_mode)){
		case ANCHOR_X_LEFT:
			break;
		case ANCHOR_X_MID:
			out_data->origin.h = out_data->width - input->width >> 1;
			break;
		case ANCHOR_X_RIGHT:
			out_data->origin.h = out_data->width - input->width;
			break;
		}
		switch (ANCHOR_Y(anchor_mode)){
		case ANCHOR_Y_UPPER:
			break;
		case ANCHOR_Y_MID:
			out_data->origin.v = out_data->height - input->height >> 1;
			break;
		case ANCHOR_Y_LOWER:
			out_data->origin.v = out_data->height - input->height;
			break;
		}
	}

	return PF_Err_NONE;
}

static PF_Err
ReadFrame8(
	void		*refcon,
	A_long		xL,
	A_long		yL,
	PF_Pixel	*inP,
	PF_Pixel	*outP){
	register AVFrame *frame = reinterpret_cast<AVFrame*>(refcon);
	uint8_t	*data = frame->data[0] + yL*frame->linesize[0] + xL*CHANNELS;
	data[0] = inP->red;
	data[1] = inP->green;
	data[2] = inP->blue;
	data[3] = inP->alpha;
	return PF_Err_NONE;
}
static PF_Err
ReadFrame16(
	void		*refcon,
	A_long		xL,
	A_long		yL,
	PF_Pixel16	*inP,
	PF_Pixel16	*outP){
	register AVFrame *frame = reinterpret_cast<AVFrame*>(refcon);
	uint16_t *data = (uint16_t*)(frame->data[0] + yL*frame->linesize[0]) + xL*CHANNELS;
	data[0] = inP->red;
	data[1] = inP->green;
	data[2] = inP->blue;
	data[3] = inP->alpha;
	return PF_Err_NONE;
}
static PF_Err
WriteFrame8(
	void		*refcon,
	A_long		xL,
	A_long		yL,
	PF_Pixel	*inP,
	PF_Pixel	*outP){
	register AVFrame *frame = reinterpret_cast<AVFrame*>(refcon);
	uint8_t	*data = frame->data[0] + yL*frame->linesize[0] + xL*CHANNELS;
	outP->red	=	data[0];
	outP->green	=	data[1];
	outP->blue	=	data[2];
	outP->alpha	=	data[3];
	return PF_Err_NONE;
}
static PF_Err
WriteFrame16(
	void		*refcon,
	A_long		xL,
	A_long		yL,
	PF_Pixel16	*inP,
	PF_Pixel16	*outP){
	register AVFrame *frame = reinterpret_cast<AVFrame*>(refcon);
	uint16_t *data = (uint16_t*)(frame->data[0] + yL*frame->linesize[0]) + xL*CHANNELS;
	outP->red = data[0];
	outP->green = data[1];
	outP->blue = data[2];
	outP->alpha = data[3];
	return PF_Err_NONE;
}
static PF_Err
Render(
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output){

	PF_Err				err = PF_Err_NONE, err2 = PF_Err_NONE;
	PF_EffectWorld		*input = &params[SCALE_INPUT]->u.ld;
	A_long				width, height, target_width, target_height;
	AVPixelFormat		fmt;
	int					mode;
	SwsContext			*img_convert_ctx;
	AVFrame				*inFrame, *outFrame;
	uint8_t				*inBuffer, *outBuffer;
	PF_FpLong			scale_percent;
	AEGP_SuiteHandler	suites(in_data->pica_basicP);
	//WriteFrame(fd->inFrame, input);

	if (PF_WORLD_IS_DEEP(input))
		fmt = PIX_FMT_RGBA64;
	else
		fmt = PIX_FMT_RGBA;

	scale_percent = params[SCALE_PERCENT]->u.fs_d.value / 100.0;
	mode = mode_choices[params[SCALE_MODE]->u.pd.value];
	width = input->width;
	height = input->height;
	target_width = static_cast<A_long>(width*scale_percent);
	target_height = static_cast<A_long>(height*scale_percent);
	ERR_ALLOC(inFrame = av_frame_alloc());
	ERR_ALLOC(outFrame = av_frame_alloc());
	ERR_ALLOC(inBuffer = (uint8_t *)av_malloc(avpicture_get_size(fmt, width, height)));
	ERR_ALLOC(outBuffer = (uint8_t *)av_malloc(avpicture_get_size(fmt, target_width, target_height)));
	ERR_ALLOC(avpicture_fill((AVPicture *)inFrame, inBuffer, fmt, width, height));
	ERR_ALLOC(avpicture_fill((AVPicture *)outFrame, outBuffer, fmt, target_width, target_height));
	ERR_ALLOC(img_convert_ctx = sws_getContext(	width,
												height,
												fmt,
												target_width,
												target_height,
												fmt,
												mode,
												NULL,
												NULL,
												NULL));

	
	/*
	PRINTXX1(width, "width");
	PRINTXX1(height, "height");
	PRINTXX1(target_width, "target_width");
	PRINTXX1(target_height, "target_height");
	PRINTXX1(output->width, "output->width");
	PRINTXX1(output->height, "output->height");
	PRINTXX1(mode, "mode");
	PRINTXX1(scale_percent*100, "scale_percent");
	*/
	if (!err){
		//PRINTXX1(0, "read");
		if (PF_WORLD_IS_DEEP(input))
			ERR(suites.Iterate16Suite1()->iterate(	in_data,
													0,
													output->height,
													input,
													NULL,
													reinterpret_cast<void*>(inFrame),
													ReadFrame16,
													input));
		else
			ERR(suites.Iterate8Suite1()->iterate(	in_data,
													0,
													output->height,
													input,
													NULL,
													reinterpret_cast<void*>(inFrame),
													ReadFrame8,
													input));

		//PRINTXX1(0, "sws_scale");
		if (!err)
			sws_scale(	img_convert_ctx,
						inFrame->data,
						inFrame->linesize,
						0,
						height,
						outFrame->data,
						outFrame->linesize);
		/*
		PRINTXX1(0, "write");
		PRINTXX1(outFrame->data[0][0], "r");
		PRINTXX1(outFrame->data[0][1], "g");
		PRINTXX1(outFrame->data[0][2], "b");
		PRINTXX1(outFrame->data[0][3], "a");
		*/
		//ReadFrame(fd->outFrame, output);
		if (PF_WORLD_IS_DEEP(input))
			ERR(suites.Iterate16Suite1()->iterate(	in_data,
													0,
													output->height,
													output,
													NULL,
													reinterpret_cast<void*>(outFrame),
													WriteFrame16,
													output));
		else
			ERR(suites.Iterate8Suite1()->iterate(	in_data,
													0,
													output->height,
													output,
													NULL,
													reinterpret_cast<void*>(outFrame),
													WriteFrame8,
													output));
	}
	sws_freeContext(img_convert_ctx);
	av_free(inBuffer);
	av_free(outBuffer);
	av_frame_free(&inFrame);
	av_frame_free(&outFrame);
	return err;
}
/*
static PF_Err
FrameSetdown(
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output){
	if (in_data->frame_data){
		FRAME_DATA *fd = FD(in_data->frame_data);
		sws_freeContext(fd->img_convert_ctx);
		av_frame_free(&fd->inFrame);
		av_frame_free(&fd->outFrame);
		PF_DISPOSE_HANDLE(in_data->frame_data);
	}
	return PF_Err_NONE;
}
*/
DllExport PF_Err
EntryPointFunc(
	PF_Cmd		cmd,
	PF_InData	*in_data,
	PF_OutData	*out_data,
	PF_ParamDef	*params[],
	PF_LayerDef	*output,
	void		*extra){

	//freopen("logo_dialog.txt", "w", stdout);
	PF_Err		err = PF_Err_NONE;

	switch (cmd) {
	case PF_Cmd_ABOUT:
		err = About(in_data, out_data, params, output);
		break;
	case PF_Cmd_GLOBAL_SETUP:
		err = GlobalSetup(in_data, out_data, params, output);
		break;
	case PF_Cmd_PARAMS_SETUP:
		err = ParamsSetup(in_data, out_data, params, output);
		break;
	case PF_Cmd_FRAME_SETUP:
		err = FrameSetup(in_data, out_data, params, output);
		break;
	case PF_Cmd_RENDER:
		err = Render(in_data, out_data, params, output);
		break;
	}
	return err;
}