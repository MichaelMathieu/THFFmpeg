#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/thffmpeg.c"
#else

int thffmpeg_(Main_avreadframe)(lua_State* L) {
  AV_Struct* avs = (AV_Struct*)lua_touserdata(L, 1);
  THTensor* t = luaT_checkudata(L, 2, torch_Tensor);
  luaL_argcheck(L, avs->pFrameRGB != NULL, 1, "No video has been opened");
  luaL_argcheck(L, t->nDimension == 3, 2, "Dst must be a 3D tensor");
  luaL_argcheck(L, t->size[0] == 3, 2, "Dst 1st dim must be 3");
  luaL_argcheck(L, (t->size[1] == avs->h) && (t->size[2] == avs->w),
		2, "Dst tensor has wrong size");
  AVFrame* frame = AV_read_frame(avs);
  if (frame == NULL) {
    lua_pushboolean(L, 0);
  } else {
    int x, y;
    for (y = 0; y < avs->h; ++y) {
      for (x = 0; x < avs->w; ++x) {
	uint8_t* p = frame->data[0] + y * frame->linesize[0] + x * 3;
	THTensor_(set3d)(t, 0, y, x, (real)p[0]);
	THTensor_(set3d)(t, 1, y, x, (real)p[1]);
	THTensor_(set3d)(t, 2, y, x, (real)p[2]);
      }
    }
    lua_pushboolean(L, 1);
  }
  return 1;
}

static const struct luaL_Reg thffmpeg_(Main__) [] = {
  {"readframe", thffmpeg_(Main_avreadframe)},
  {NULL, NULL}
};

void thffmpeg_(Main_init)(lua_State *L) {
  luaT_pushmetatable(L, torch_Tensor);
  luaT_registeratname(L, thffmpeg_(Main__), "libthffmpeg");
}

#endif
