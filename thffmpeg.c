#include <TH.h>
#include <luaT.h>

//#define THFFMPEG_VERBOSE

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc  avcodec_alloc_frame
#endif

#include <stdio.h>

typedef struct {
  AVFormatContext   *pFormatCtx;
  AVCodecContext    *pCodecCtx;
  AVCodec           *pCodec;
  AVFrame           *pFrame;
  AVFrame           *pFrameRGB;
  AVPacket          packet;
  uint8_t           *buffer;
  AVDictionary      *optionsDict;
  int               videoStream;
  struct SwsContext *sws_ctx;
  int               h, w;
} AV_Struct;

void AV_init() {
  // Register all formats and codecs
  if (LIBAVFORMAT_VERSION_INT != avformat_version()) {
    fprintf(stderr, "FFMPEG avformat header has version %d, but the library has version %d\n",
	   LIBAVFORMAT_VERSION_INT, avformat_version());
    exit(0);
  }
  if (LIBAVCODEC_VERSION_INT != avcodec_version()) {
    fprintf(stderr, "FFMPEG avcodec header has version %d, but the library has version %d\n",
	   LIBAVCODEC_VERSION_INT, avcodec_version());
    exit(0);
  }
  if (LIBSWSCALE_VERSION_INT != swscale_version()) {
    fprintf(stderr, "FFMPEG swscale header has version %d, but the library has version %d\n",
	   LIBSWSCALE_VERSION_INT, swscale_version());
    exit(0);
  }
    
  av_register_all();

#ifndef THFFMPEG_VERBOSE
  av_log_set_level(AV_LOG_PANIC);
#endif
}

void AV_close(AV_Struct* avs) {
  if (avs->buffer != NULL)
    av_free(avs->buffer);
  if (avs->pFrameRGB != NULL)
    av_free(avs->pFrameRGB);
  if (avs->pFrame != NULL)
    av_free(avs->pFrame);
  if (avs->pCodecCtx != NULL)
    avcodec_close(avs->pCodecCtx);
  if (avs->pFormatCtx != NULL)
    avformat_close_input(&(avs->pFormatCtx));
  memset(avs, 0, sizeof(AV_Struct));
}

int AV_open(AV_Struct* avs, const char* filename) {
  // Open video file
  if (avformat_open_input(&(avs->pFormatCtx), filename, NULL, NULL) != 0)
    return 0;
  // Retrieve stream information
  if (avformat_find_stream_info(avs->pFormatCtx, NULL) < 0)
    return 0;
#ifdef THFFMPEG_VERBOSE
  // Dump information about file onto standard error
  av_dump_format(avs->pFormatCtx, 0, filename, 0);
#endif
  // Find the first video stream
  avs->videoStream = -1;
  int i;
  for (i = 0; i < avs->pFormatCtx->nb_streams; ++i) {
    if (avs->pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
      avs->videoStream = i;
      break;
    }
  }
  if (avs->videoStream == -1)
    return 0;
  // Get a pointer to the codec context for the video stream
  avs->pCodecCtx = avs->pFormatCtx->streams[avs->videoStream]->codec;
  // Find the decoder for the video stream
  avs->pCodec = avcodec_find_decoder(avs->pCodecCtx->codec_id);
  if (avs->pCodec == NULL) {
    fprintf(stderr, "Unsupported codec!\n");
    return 0;
  }
  // Open codec
  if (avcodec_open2(avs->pCodecCtx, avs->pCodec, &(avs->optionsDict)) < 0)
    return 0;
  // Allocate video frame
  avs->pFrame = av_frame_alloc();
  // Allocate an AVFrame structure
  avs->pFrameRGB = av_frame_alloc();
  if(avs->pFrameRGB == NULL)
    return 0;
  // Determine required buffer size and allocate buffer
  int numBytes = avpicture_get_size(PIX_FMT_RGB24, avs->pCodecCtx->width,
				    avs->pCodecCtx->height);
  avs->buffer = (uint8_t*)av_malloc(numBytes*sizeof(uint8_t));
  // fill sws_ctx
  avs->sws_ctx =
    sws_getContext(avs->pCodecCtx->width, avs->pCodecCtx->height, avs->pCodecCtx->pix_fmt,
		   avs->pCodecCtx->width, avs->pCodecCtx->height, PIX_FMT_RGB24,
		   SWS_BILINEAR, NULL, NULL, NULL);
  // Assign appropriate parts of buffer to image planes in pFrameRGB
  // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
  // of AVPicture
  avpicture_fill((AVPicture*)avs->pFrameRGB, avs->buffer, PIX_FMT_RGB24,
		 avs->pCodecCtx->width, avs->pCodecCtx->height);
  avs->h = avs->pCodecCtx->height;
  avs->w = avs->pCodecCtx->width;
  return 1;
}

AVFrame* AV_read_frame(AV_Struct* avs) {
  int frameFinished;
  while (av_read_frame(avs->pFormatCtx, &(avs->packet)) >= 0) {
    // Is this a packet from the video stream?
    if(avs->packet.stream_index == avs->videoStream) {
      // Decode video frame
      avcodec_decode_video2(avs->pCodecCtx, avs->pFrame, &frameFinished,  &avs->packet);
      // Did we get a video frame?
      if(frameFinished) {
	// Convert the image from its native format to RGB
        sws_scale(avs->sws_ctx, (uint8_t const * const *)avs->pFrame->data,
		  avs->pFrame->linesize, 0, avs->pCodecCtx->height,
		  avs->pFrameRGB->data, avs->pFrameRGB->linesize);
	// Free the packet that was allocated by av_read_frame
	av_free_packet(&avs->packet);
	return avs->pFrameRGB;
      }
    }
  }
  return NULL;
}

int AV_Struct_gc(lua_State* L) {
  AV_Struct* avs = (AV_Struct*)lua_touserdata(L, 1);
  AV_close(avs);
  return 0;
}

int thffmpeg_Main_avinit(lua_State* L) {
  if (!lua_toboolean(L, 1))
    AV_init();
  AV_Struct* avs = (AV_Struct*)lua_newuserdata(L, sizeof(AV_Struct));
  lua_newtable(L);
  lua_pushstring(L, "__gc");
  lua_pushcfunction(L, AV_Struct_gc);
  lua_settable(L, -3);  
  lua_setmetatable(L, -2);
  memset(avs, 0, sizeof(AV_Struct));
  return 1;
}

int thffmpeg_Main_avopen(lua_State* L) {
  AV_Struct* avs = (AV_Struct*)lua_touserdata(L, 1);
  const char* filename = lua_tostring(L, 2);
  if (AV_open(avs, filename) == 0) {
    lua_pushnil(L);
    lua_pushnil(L);
  } else {
    lua_pushnumber(L, avs->h);
    lua_pushnumber(L, avs->w);
  }
  return 2;
}

int thffmpeg_Main_avclose(lua_State* L) {
  AV_Struct* avs = (AV_Struct*)lua_touserdata(L, 1);
  AV_close(avs);
  return 0;
}

static const struct luaL_Reg thffmpeg_Main__ [] = {
  {"init", thffmpeg_Main_avinit},
  {"open", thffmpeg_Main_avopen},
  {"close", thffmpeg_Main_avclose},
  {NULL, NULL}
};

#define torch_(NAME) TH_CONCAT_3(torch_, Real, NAME)
#define torch_Tensor TH_CONCAT_STRING_3(torch., Real, Tensor)
#define thffmpeg_(NAME) TH_CONCAT_3(thffmpeg_, Real, NAME)

#include "generic/thffmpeg.c"
#include "THGenerateAllTypes.h"

DLL_EXPORT int luaopen_libthffmpeg(lua_State *L) {
  thffmpeg_FloatMain_init(L);
  thffmpeg_DoubleMain_init(L);
  thffmpeg_ByteMain_init(L);

  lua_newtable(L);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "libthffmpeg");

  lua_newtable(L);
  luaT_setfuncs(L, thffmpeg_DoubleMain__, 0);
  lua_setfield(L, -2, "libthffmpeg");

  lua_newtable(L);
  luaT_setfuncs(L, thffmpeg_FloatMain__, 0);
  lua_setfield(L, -2, "libthffmpeg");

  lua_newtable(L);
  luaT_setfuncs(L, thffmpeg_ByteMain__, 0);
  lua_setfield(L, -2, "libthffmpeg");
  
  luaL_register(L, "libthffmpeg", thffmpeg_Main__);

  return 1;
}
