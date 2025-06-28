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

def detect_Pet_Ground(distance: numpy.ndarray) -> bool:

    middleL = numpy.mean(distance[3:7, 3])
    middleR = numpy.mean(distance[3:7, 4])
    diff_middle = numpy.abs(middleL - middleR)
    mean_middle = numpy.mean([middleL, middleR])

    sideL = numpy.mean(distance[3:7, 2])
    sideR = numpy.mean(distance[3:7, 5])
    mean_side = numpy.mean([sideR, sideL])

    print("helo")
    if numpy.abs(mean_side - mean_middle) <= 20  and diff_middle <= 10:
        return True
            
    else:
        return False
    

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

        if numpy.abs(mean_side - mean_middle) <= 20 and diff_middle <= 10:
            return True
        else:
            return False
    else:
        return False


def detect_Pet_Pillar(distance: numpy.ndarray, center_dist: numpy.ndarray, mean_reflectance: float) -> bool:
    global prev_distance, stable_distant_count

    if mean_reflectance <= 20:
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

        #distance
        distance = numpy.flipud(numpy.array(data.distance_mm).reshape((8, 8)))
        center_dist = distance[3:7, 2:6]
        mean_distance = float(numpy.mean(center_dist))
        
        if 170 <= mean_distance <= 240:

            #reflectance
            reflectance = numpy.flipud(numpy.array(data.reflectance).reshape((8, 8)))
            mean_reflectance = float(numpy.mean(reflectance[3:7, 2:6]))

            detect_Pet_Pillar(distance, center_dist, mean_reflectance)

            if detect_Pet_Debris(distance, mean_distance):
                 print("There's pet behind debris \n", distance)

            elif(detect_Pet_Ground(distance)):
                print("There's pet on the ground \n", distance)

    time.sleep(0.2)