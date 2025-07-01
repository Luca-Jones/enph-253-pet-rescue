import time
import numpy
import vl53l5cx_ctypes as vl53l5cx
from vl53l5cx_ctypes import STATUS_RANGE_VALID, STATUS_RANGE_VALID_LARGE_PULSE

print("Uploading, please wait...")
vl53 = vl53l5cx.VL53L5CX()
vl53.set_resolution(8 * 8)
vl53.set_ranging_frequency_hz(12)  
print("Done!")

prev_distance = None
prev_reflectance = None
stable_distant_count = 0
stable_reflectance_count = 0
    
vl53.start_ranging()

"""
Detects all the pets on ground including the one behind 4" debris.
Takes middle portion of 8by8 TOF matrix and returns true if some cylindrical object is just in front of the sensor.
Assumed that the sensor is place at the same height as the top of the object and is 17~23cm away from the object.

Args:
    distance (numpy.ndarray): 8by8 distance matrix

Returns:
    bool: True if cylindrical object of base 7cm is detected 17~25cm away
    
"""
def detect_Pet_Ground(distance: numpy.ndarray) -> bool:

    middleL = numpy.mean(distance[3:5, 3])
    middleR = numpy.mean(distance[3:5, 4])
    diff_middle = numpy.abs(middleL - middleR)
    
    sideL = numpy.mean(distance[3:5, 2])
    sideR = numpy.mean(distance[3:5, 5])
    diff_side = numpy.abs(sideL - sideR) # Fine adjustment and prevents detecting walls 

    middle_Top = numpy.mean(distance[0:1, 3:4]) # To distinguish between zipline pole and pets

    
    if  diff_side <= 10 and diff_middle <= 7 and middle_Top >= 260:
        return True
            
    else:
        return False


def detect_Pet_Pillar(distance: numpy.ndarray, center_dist: numpy.ndarray, mean_reflectance: float) -> bool:
    global prev_distance, stable_distant_count

    if mean_reflectance <= 30:
        if prev_distance is not None:
            diff_dist = numpy.abs(center_dist - prev_distance)
            frame_var1 = numpy.mean(diff_dist)

            if frame_var1 < 50:
                stable_distant_count += 1
            else:
                stable_distant_count = 0

        prev_distance = center_dist.copy()

        if stable_distant_count == 3:
            print("Pillar")
            print(f"Mean Reflectance: {mean_reflectance:.2f}")
            print("Distance Map:\n", distance)
            stable_distant_count = 0
            return True
    else:
        stable_distant_count = 0
        prev_distance = None

    return False


while True:
    if vl53.data_ready():
        data = vl53.get_data()

        distance = numpy.flipud(numpy.array(data.distance_mm).reshape((8, 8)))
        center_dist = distance[4:7, 3:5]
        mean_distance = float(numpy.mean(center_dist))
        
        if 100 <= mean_distance <= 240:

            reflectance = numpy.flipud(numpy.array(data.reflectance).reshape((8, 8)))
            mean_reflectance = float(numpy.mean(reflectance[5:7, 2:6]))

            detect_Pet_Pillar(distance, center_dist, mean_reflectance)

            if detect_Pet_Ground(distance):
                print("There's pet on the ground \n", distance)
                #Write codes to send signal to esp32 for extending the arm

            else:
                print("Not in the middle")
                #Write code to send signal to esp32 for slowing down or adjusting chassiss's position

    time.sleep(0.2)


    """
def detect_Pet_Debris(distance: numpy.ndarray, mean_distance: float) -> bool:
    
    mean_debris_dist = float(numpy.mean(distance[6:7, 2:6]))
    mean_Pet_Dist = float(numpy.mean(distance[3:5, 2:6]))

    if mean_debris_dist <= 185 and 200 <= mean_Pet_Dist <= 250:

        middleL = numpy.mean(distance[3:5, 3])
        middleR = numpy.mean(distance[3:5, 4])
        diff_middle = numpy.abs(middleL - middleR)
        mean_middle = numpy.mean([middleL, middleR])

        sideL = numpy.mean(distance[3:5, 2])
        sideR = numpy.mean(distance[3:5, 5])
        mean_side = numpy.mean([sideR, sideL])

        if numpy.abs(sideL - sideR) <= 5 and numpy.abs(mean_side - mean_middle) <= 20 and diff_middle <= 10:
            return True
        else:
            return False
    else:
        return False
"""