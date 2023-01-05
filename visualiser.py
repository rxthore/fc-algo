import serial

# Set up serial connection to Arduino
ser = serial.Serial('/dev/cu.usbserial-10', 9600)

while True:
    # Read a line of data from the serial port
    data = ser.readline().strip()
    
    # Split the data into a list of values
    # values = [float(x) for x in data.split(',')]

    print(data)
    
    # Print the values
    # print(values)