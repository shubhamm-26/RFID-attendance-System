import os
import serial
import pandas as pd
from datetime import datetime

# Serial port settings
ser = serial.Serial('COM2', 9600)  # Adjust COM2 to your Arduino's serial port

# CSV file settings
csv_file_path = 'data.csv'

def update_csv(user_id, current_time, student_id, name, phone, room):
    try:
        # Check if the CSV file exists, create an empty DataFrame if it doesn't
        if not os.path.exists(csv_file_path):
            df = pd.DataFrame(columns=['User_ID', 'Student_ID', 'Name', 'Phone No.', 'Room No.', 'InTime', 'OutTime'])
        else:
            # Read existing data
            df = pd.read_csv(csv_file_path)

        # Check if the user is already present
        user_exists = df['User_ID'].eq(user_id).any()

        if user_exists:
            # User exists, find the last index for the given user_id
            user_index = df.index[df['User_ID'] == user_id].max()

            if pd.isnull(df.at[user_index, 'OutTime']):
                # OutTime is empty, update OutTime
                df.at[user_index, 'OutTime'] = current_time
            else:
                # Both InTime and OutTime are present, add a new entry
                new_entry = {'User_ID': user_id, 'Student_ID': student_id, 'Name': name, 'Phone No.': phone,
                             'Room No.': room, 'InTime': current_time, 'OutTime': None}
                df = pd.concat([df, pd.DataFrame([new_entry])], ignore_index=True)
        else:
            # User doesn't exist, add a new entry
            new_entry = {'User_ID': user_id, 'Student_ID': student_id, 'Name': name, 'Phone No.': phone,
                         'Room No.': room, 'InTime': current_time, 'OutTime': None}
            df = pd.concat([df, pd.DataFrame([new_entry])], ignore_index=True)

        # Save the updated DataFrame to CSV
        df.to_csv(csv_file_path, index=False)

    except Exception as e:
        print(f"Error updating CSV file: {e}")

# Continuous loop for reading RFID data from Arduino
try:
    while True:
        # Read RFID data from Arduino
        rfid_data = ser.readline().decode().strip()

        # Print the received RFID data for debugging
        print("Received RFID data:", rfid_data)

        # Extract data from the received RFID data
        data_parts = rfid_data.split(',')
        if len(data_parts) == 5:
            user_id, student_id, name, phone, room = map(str.strip, data_parts)

            # Convert timestamp to datetime
            current_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

            # Update CSV file with the specified conditions
            update_csv(user_id, current_time, student_id, name, phone, room)

except KeyboardInterrupt:
    ser.close()
    print("Serial port closed.")
except Exception as e:
    print(f"An error occurred: {e}")
    ser.close()
