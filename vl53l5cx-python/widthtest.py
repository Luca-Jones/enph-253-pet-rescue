import time
import numpy
import vl53l5cx_ctypes as vl53l5cx
from vl53l5cx_ctypes import STATUS_RANGE_VALID, STATUS_RANGE_VALID_LARGE_PULSE

print("Uploading, please wait...")
vl53 = vl53l5cx.VL53L5CX()
vl53.set_resolution(8 * 8)
vl53.set_ranging_frequency_hz(8)
print("Done!")

prev_distance = None
prev_reflectance = None
stable_distant_count = 0
stable_reflectance_count = 0

vl53.start_ranging()

def checkCenter(array: numpy.ndarray) -> bool:
    middleL = numpy.mean(array[2:6, 3])
    middleR = numpy.mean(array[2:6, 4])
    diff_middle = numpy.abs(middleL - middleR)

    sideL = numpy.mean(array[2:6, 2])
    sideR = numpy.mean(array[2:6, 5])
    diff_side = numpy.abs(sideL - sideR)

    if diff_middle <= 20 and diff_side <= 40 and sideL - middleL >= 20:
        return True
    else:
        return False

while True:
    if vl53.data_ready():
        data = vl53.get_data()

        # 2d array of distance
        distance = numpy.flipud(numpy.array(data.distance_mm).reshape((8, 8)))
        center_dist = distance[2:6, 2:6]  # Center grid
        mean_distance = numpy.mean(center_dist)

        if checkCenter(distance):
            print(distance)

        








    
