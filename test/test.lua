require 'thffmpeg'
require 'sys'
require 'math'

x = THFFmpeg()
y = THFFmpeg()
x = nil
collectgarbage()
collectgarbage()

if not y:open('v_ApplyEyeMakeup_g01_c01.avi') then
   print("Failed to open video")
   os.exit(0)
end

local fmeans = {87.119574652778, 87.233272569444, 86.159526909722}
print(fmeans)

require 'image'

frame1 = y:next_frame()
--image.display(frame1)
--os.execute('sleep 10')
print(frame1:mean())

if math.abs(frame1:mean() - fmeans[1]) > 1e-4 then
   print("Frame 1 doesn't seem to be correct")
   sys.exit(0)
end
frame2 = y:next_frame()
print(frame2:mean())
if math.abs(frame2:mean() - fmeans[2]) > 1e-4 then
   print(frame2:mean(), fmeans[2])
   print("Frame 2 doesn't seem to be correct")
   sys.exit(0)
end
frame3 = y:next_frame()
print(frame3:mean())
if math.abs(frame3:mean() - fmeans[3]) > 1e-4 then
   print("Frame 3 doesn't seem to be correct")
   sys.exit(0)
end

if not y:open('v_ApplyEyeMakeup_g01_c01.avi') then
   print("Failed to reopen video")
   sys.exit(0)
end

frame1 = y:next_frame()
if math.abs(frame1:mean() - fmeans[1]) > 1e-4 then
   print("Frame 1 doesn't seem to be correct")
   sys.exit(0)
end
frame2 = y:next_frame()
if math.abs(frame2:mean() - fmeans[2]) > 1e-4 then
   print("Frame 2 doesn't seem to be correct")
   sys.exit(0)
end
frame3 = y:next_frame()
if math.abs(frame3:mean() - fmeans[3]) > 1e-4 then
   print("Frame 3 doesn't seem to be correct")
   sys.exit(0)
end

if not y:open('/scratch/datasets/ucf101/UCF-101/ApplyEyeMakeup/v_ApplyEyeMakeup_g01_c01.avi') then
   print("Failed to reopen video")
   sys.exit(0)
end

assert(y:seek(0), "seek 0 failed")
frame = y:next_frame()
print(0, frame:mean())
assert(math.abs(frame:mean() - fmeans[1]) < 1e-4)
assert(y:seek(0), "seek 0 failed")
frame = y:next_frame()
print(0, frame:mean())
assert(math.abs(frame:mean() - fmeans[1]) < 1e-4)
print(y:length())
assert(y:seek(2), "seek 2 failed")
frame = y:next_frame()
print(2, frame:mean())
assert(math.abs(frame:mean() - fmeans[3]) < 1e-4)
assert(y:seek(1), "seek 1 failed")
frame = y:next_frame()
print(1, frame:mean())
assert(math.abs(frame:mean() - fmeans[2]) < 1e-4)


print("Test ok")