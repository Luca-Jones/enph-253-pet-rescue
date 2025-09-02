
import cv2
import numpy as np
import time
from threading import Thread

''' PARAMETERS '''
video_stream_source = "udp://@127.0.0.1:9999" # laptop camera
min_conf_threshold = float(0.25)
resW, resH = 640, 480
imW, imH = int(resW), int(resH)

class VideoStream:
    """Camera object that controls video streaming from the Picamera"""
    def __init__(self,source,resolution=(640,480),framerate=30):
        self.stopped = False
        self.stream = cv2.VideoCapture(source)
        self.stream.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*'MJPG'))
        self.stream.set(3,resolution[0])
        self.stream.set(4,resolution[1])
        (self.grabbed, self.frame) = self.stream.read() # Read first frame from the stream

    def start(self):
        Thread(target=self.update,args=()).start()
        return self

    def update(self):
        while True:
            if self.stopped:
                self.stream.release()
                return
            (self.grabbed, self.frame) = self.stream.read()

    def read(self):
        return self.frame

    def stop(self):
        self.stopped = True

# Initialize frame rate calculation
fps = 1
freq = cv2.getTickFrequency()

# Initialize video stream
videostream = VideoStream(source=video_stream_source, resolution=(imW,imH), framerate=30).start()
time.sleep(1)

#for frame1 in camera.capture_continuous(rawCapture, format="bgr",use_video_port=True):
while True:

    t1 = cv2.getTickCount()

    frame1 = videostream.read()
    frame = frame1.copy()
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    input_data = np.expand_dims(frame_rgb, axis=0)

    # Draw framerate in corner of frame
    cv2.putText(frame,'FPS: {0:.2f}'.format(fps),(30,50),cv2.FONT_HERSHEY_SIMPLEX,1,(255,255,0),2,cv2.LINE_AA)

    # Display
    cv2.imshow('opencv_test', frame)

    # Calculate framerate
    t2 = cv2.getTickCount()
    time1 = (t2-t1)/freq
    fps= 1/time1

    # Press 'q' to quit
    if cv2.waitKey(1) == ord('q'):
        break

# Clean up
cv2.destroyAllWindows()
videostream.stop()