import serial
import csv
import time
import threading
import queue

# Serial port settings
PORT = 'COM5'
BAUDRATE = 115200
TIMEOUT = 1

# Enable real-time plotting
ENABLE_PLOT = True

# Generate timestamp for file name
timestamp = time.strftime('%Y-%m-%d_%H-%M-%S')
CSV_FILE = f'uart_data_{timestamp}.csv'

# Open serial port
try:
    ser = serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT)
    print(f"Connected to {PORT} at {BAUDRATE} baud")
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    exit(1)

# Setup real-time plotting if enabled
if ENABLE_PLOT:
    import matplotlib
    matplotlib.use('TkAgg')
    import matplotlib.pyplot as plt
    
    data_queue = queue.Queue()
    
    def plot_thread():
        plt.ion()
        fig, ax1 = plt.subplots(figsize=(16, 9))
        ax2 = ax1.twinx()
        
        times = []
        temps = []
        targets = []
        volts = []
        
        line_temp, = ax1.plot([], [], label='Temperature (°C)', color='blue')
        line_target, = ax1.plot([], [], label='Target Temperature (°C)', color='orange', linestyle='--')
        line_volt, = ax2.plot([], [], label='Voltage (V)', color='red')
        
        ax1.set_xlabel('Time (s)')
        ax1.set_ylabel('Temperature (°C)', color='blue')
        ax2.set_ylabel('Voltage (V)', color='red')
        ax1.set_title('Real-Time Temperature and Voltage')
        ax1.grid(True)
        
        lines = [line_temp, line_target, line_volt]
        labels = [l.get_label() for l in lines]
        ax1.legend(lines, labels, loc='upper left')
        
        plt.show(block=False)
        
        while True:
            try:
                data = data_queue.get(timeout=1)  # Wait for data
                if data == 'STOP':
                    break
                t, temp, volt, target = data
                times.append(t)
                temps.append(temp)
                volts.append(volt)
                targets.append(target)
                
                line_temp.set_data(times, temps)
                line_target.set_data(times, targets)
                line_volt.set_data(times, volts)
                ax1.relim()
                ax1.autoscale_view()
                ax2.relim()
                ax2.autoscale_view()
                plt.draw()
                fig.canvas.flush_events()  # Process GUI events
                plt.pause(0.01)  # Small pause for responsiveness
            except queue.Empty:
                continue
        
        # Final save
        save_filename = CSV_FILE.replace('.csv', '')
        save_filename += "_real_time.png"
        plt.savefig(save_filename, dpi=80)
        print(f"Plot saved as {save_filename}")
        plt.ioff()
        plt.show()
    
    plot_thread = threading.Thread(target=plot_thread)
    plot_thread.start()

# Open CSV file for writing
with open(CSV_FILE, 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    # Write headers
    writer.writerow(['Temperature', 'Target_Temperature', 'ADC_Value', 'Voltage', 'Time', 'FSM_State_Index', 'Heater_Currently_On'])
    print("Reading from serial port... Press Ctrl+C to stop")

    try:
        while True:
            # Read a line from serial
            line = ser.readline().decode('utf-8').strip()
            if line:
                # Parse the line into columns
                row = line.split(',')
                # Write to CSV
                writer.writerow(row)
                print(f"Written: {line}")
                
                # Update plot if enabled
                if ENABLE_PLOT:
                    t = float(row[4]) / 1000
                    temp = float(row[0]) / 1000
                    volt = float(row[3]) / 1000
                    target = float(row[1]) if len(row) > 1 else 0
                    data_queue.put((t, temp, volt, target))
    except KeyboardInterrupt:
        print("\nStopped by user")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        ser.close()
        print("Serial port closed")

print(f"Data saved to {CSV_FILE}")

# Final plot update and save if enabled
if ENABLE_PLOT:
    data_queue.put('STOP')
    plot_thread.join()