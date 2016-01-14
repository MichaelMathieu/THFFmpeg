require 'thffmpeg'

--x = THFFmpeg()
y = THFFmpeg()
x = nil
collectgarbage()
collectgarbage()

print (y:open('/scratch/datasets/ucf101/UCF-101/ApplyEyeMakeup/v_ApplyEyeMakeup_g01_c01.avi'))
frame = y:next_frame()
print(frame[3]:mean())
print(frame[1][1][1])
print(frame[3][y.h][y.w])
require 'image'
image.display(frame)
--image.display(frame[2])
--image.display(frame[3])

print("ok")