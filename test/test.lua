require 'thffmpeg'
require 'sys'
require 'math'

x = THFFmpeg()
y = THFFmpeg()
x = nil
collectgarbage()
collectgarbage()

if not y:open('/scratch/datasets/ucf101/UCF-101/ApplyEyeMakeup/v_ApplyEyeMakeup_g01_c01.avi') then
   print("Failed to open video")
   sys.exit(0)
end

frame1 = y:next_frame()
if math.abs(frame1:mean() - 87.119574652778) > 1e-4 then
   print("Frame 1 doesn't seem to be correct")
   sys.exit(0)
end
frame2 = y:next_frame()
if math.abs(frame2:mean() - 87.233272569444) > 1e-4 then
   print("Frame 2 doesn't seem to be correct")
   sys.exit(0)
end
frame3 = y:next_frame()
if math.abs(frame3:mean() - 86.159526909722) > 1e-4 then
   print("Frame 3 doesn't seem to be correct")
   sys.exit(0)
end


print("Test ok")