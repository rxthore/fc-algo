import serial
import matplotlib.pyplot as plt
import csv

ser = serial.Serial('/dev/cu.usbserial-10', 9600)

def main():
    orientations = []
    tempList = []

    for x in range(100):
        data = ser.readline().strip()
        data = str(data)[2:][:-1]
        rotations = data.split(",")
        # gyro_roll, gyro_pitch, gyro_yaw = 0
        # acc_roll, acc_pitch, acc_yaw = 0
        # roll, pitch, yaw = 0

        if len(rotations) == 3:
            try:
                _rotations = list(map(float, rotations))
                print(_rotations)
                x, y, z = _rotations

                if len(tempList) == 3:
                    print(tempList)
                    orientations.append(tempList)
                    tempList = []

                tempList.append([x, y, z])
                print(tempList)

            except:
                print("Initialising...")
        
    print("DATA COLLECTED")

    gyro_rotations = []
    accel_rotations = []
    fused_rotations = []
    cols = ["gyro_x", "gyro_y", "gyro_z", "accel_x", "accel_y", "accel_z", "fused_x", "fused_y", "fused_z"]
    rows = []

    for item in orientations:
        rows.append(item[0] + item[1] + item[2])
        gyro_rotations.append(item[0])
        accel_rotations.append(item[1])
        fused_rotations.append(item[2])

    with open("readings.csv", "w") as csvfile:
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(cols)
        csvwriter.writerows(rows)

if __name__ == "__main__":
    main()