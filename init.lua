--[[
Copyright (c) 2016, Michael Mathieu
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.
--]]

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
      print("THFFmpeg: file " .. filename .. " does not exist")
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
   if (buffer:dim() ~= 3) or (buffer:size(1) ~= 3)
      or (buffer:size(2) ~= self.h) or (buffer:size(3) ~= self.w) then
	 print("THFFmpeg: wrong buffer size: {buffer, self}")
	 print{buffer, {3, self.h, self.w}}
	 return nil
   end
   if buffer.libthffmpeg.readframe(self.avs, buffer) then
      return buffer
   else
      return nil
   end
end

function THFFmpeg:skip_frame()
   return libthffmpeg.skipframe(self.avs)
end

function THFFmpeg:seek(idx)
   return libthffmpeg.seek(self.avs, idx)
end

function THFFmpeg:length()
   return libthffmpeg.length(self.avs)
end
