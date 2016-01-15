require 'torch'
require 'paths'
require 'libthffmpeg'

local THFFmpeg = torch.class("THFFmpeg")

--TODO cmakefile (libs)
local global_ffmpeg_init = false
function THFFmpeg:__init()
   self.avs = libthffmpeg.init(global_ffmpeg_init)
   global_ffmpeg_init = true
end

function THFFmpeg:open(filename)
   if not paths.filep(filename) then
      print("THFFmpeg: file " .. filename .. "does not exist")
      return false
   end
   self.h, self.w = libthffmpeg.open(self.avs, filename)
   return (self.h ~= nil)
end

function THFFmpeg:close()
   libthffmpeg.close(self.avs)
end

function THFFmpeg:next_frame(buffer)
   if self.h == nil then
      error("THFFmpeg: next_frame called without opening a video")
   end
   buffer = buffer or torch.Tensor(3, self.h, self.w)
   assert((buffer:dim() == 3) and (buffer:size(1) == 3)
	     and (buffer:size(2) == self.h) and (buffer:size(3) == self.w))
   if buffer.libthffmpeg.readframe(self.avs, buffer) then
      return buffer
   else
      return nil
   end
end

function THFFmpeg:seek(idx)
   return libthffmpeg.seek(self.avs, idx)
end

function THFFmpeg:length()
   return libthffmpeg.length(self.avs)
end