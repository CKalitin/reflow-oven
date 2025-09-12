import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def plot_data(csv_file, plot_type='temp', show_state_changes=True, time_min=None, time_max=None):
    """
    Plot data from CSV file.

    Parameters:
    - csv_file: Path to the CSV file.
    - plot_type: String containing 't' for temperature, 'v' for voltage (e.g., 't', 'v', 'tv').
    - show_state_changes: If True, show vertical lines for FSM state changes.
    - time_min: Minimum time (in seconds) for the plot. Default: None (use data min).
    - time_max: Maximum time (in seconds) for the plot. Default: None (use data max).
    """
    # Read CSV
    df = pd.read_csv(csv_file)
    
    # Convert units
    df['Time'] = df['Time'] / 1000  # ms to s
    df['Temperature'] = df['Temperature'] / 1000  # mC to C
    df['Voltage'] = df['Voltage'] / 1000  # mV to V
    
    # Filter by time if specified
    if time_min is not None:
        df = df[df['Time'] >= time_min]
    if time_max is not None:
        df = df[df['Time'] <= time_max]
    
    if df.empty:
        print("No data in the specified time range.")
        return
    
    # Determine what to plot
    plot_temp = 't' in plot_type.lower()
    plot_volt = 'v' in plot_type.lower()
    
    if not plot_temp and not plot_volt:
        print("Invalid plot_type. Must contain 't' or 'v'.")
        return
    
    # Create plot
    fig, ax1 = plt.subplots(figsize=(16, 9))
    ax2 = None
    
    if plot_temp:
        ax1.plot(df['Time'], df['Temperature'], label='Temperature (°C)', color='blue')
        if 'Target_Temperature' in df.columns:
            ax1.plot(df['Time'], df['Target_Temperature'], label='Target Temperature (°C)', color='orange', linestyle='--')
        ax1.set_ylabel('Temperature (°C)', color='blue')
        ax1.tick_params(axis='y', labelcolor='blue')
    
    if plot_volt:
        if plot_temp:
            # Dual axes
            ax2 = ax1.twinx()
            ax2.plot(df['Time'], df['Voltage'], label='Voltage (V)', color='red')
            ax2.set_ylabel('Voltage (V)', color='red')
            ax2.tick_params(axis='y', labelcolor='red')
        else:
            # Single axis for voltage
            ax1.plot(df['Time'], df['Voltage'], label='Voltage (V)', color='red')
            ax1.set_ylabel('Voltage (V)', color='red')
            ax1.tick_params(axis='y', labelcolor='red')
    
    # Set title
    if plot_temp and plot_volt:
        title = 'Temperature and Voltage vs Time'
    elif plot_temp:
        title = 'Temperature vs Time'
    elif plot_volt:
        title = 'Voltage vs Time'
    
    ax1.set_xlabel('Time (s)')
    ax1.set_title(title)
    ax1.grid(True)
    
    if ax2 is not None:
        lines1, labels1 = ax1.get_legend_handles_labels()
        lines2, labels2 = ax2.get_legend_handles_labels()
        ax1.legend(lines1 + lines2, labels1 + labels2, loc='upper left')
    else:
        ax1.legend()
    
    # Add vertical lines for FSM state changes
    if show_state_changes:
        state_changes = df[df['FSM_State_Index'] != df['FSM_State_Index'].shift()]
        for idx, row in state_changes.iterrows():
            ax1.axvline(x=row['Time'], color='green', linestyle='--', alpha=0.7)
            ax1.text(row['Time'], ax1.get_ylim()[1] * 0.95, f'State {int(row["FSM_State_Index"])}', 
                     rotation=90, verticalalignment='top', fontsize=8, color='green')
    
    plt.tight_layout()
    
    # Save the plot in 720p (1280x720)
    save_filename = csv_file.replace('.csv', '.png')
    plt.savefig(save_filename, dpi=80)
    print(f"Plot saved as {save_filename}")
    
    plt.show()

# Example usage:
plot_data('uart_data_2025-09-11_21-57-22.csv', plot_type='tv', show_state_changes=True, time_min=None, time_max=None)
