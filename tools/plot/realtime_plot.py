import time
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.patches as patches
import mplcursors as mplc

class RealtimePlot:

    def __init__(self, callback, config, interval, max_entries):
        # Config
        self.config = config
        self.columns = list(config.keys())
        self.interval = interval
        self.max_entries = max_entries
        self.callback = callback

        # Plot
        self.values = { }
        self.lines = { }

        # FPS
        self.last_update = time.time()
        self.frame_count = 0
        self.fps_text = None

    def show(self):
        # Create the animation
        self.fig, self.ax = plt.subplots(1, 1)
        self._setup_plot()
        ani = animation.FuncAnimation(
            self.fig,
            self._animate_callback,
            frames=self.max_entries,
            interval=self.interval,
            blit=False
        )
        mplc.cursor(hover=False)
        plt.show()

    def _setup_plot(self):
        # Initialize the values
        for column in self.columns + ["time"]:
            self.values[column] = []

        # Create the lines
        self.lines = { }
        for column in self.columns:
            self.lines[column], = self.ax.plot([], [], label=column, color=self.config[column]["color"], linestyle=self.config[column]["line"])

        # Setup FPS label on the top-right corner
        self.fps_text = self.ax.text(0.95, 0.95, "", transform=self.ax.transAxes, ha="right")

        # Setup the legend
        self.ax.set_xlabel("Time")
        self.ax.set_ylabel("Value")

        # Setup time format
        self.ax.xaxis.set_major_formatter(plt.FuncFormatter(lambda value, _ : time.strftime("%H:%M:%S", time.localtime(value))))

    def _animate_callback(self, frame):
        # Handle FPS calculation
        self.frame_count += 1
        now = time.time()
        elapsed = now - self.last_update
        if elapsed >= 1:
            fps = self.frame_count / elapsed
            self.fps_text.set_text(f'FPS: {fps:.2f}')
            self.last_update = now
            self.frame_count = 0

        # Generate new data
        data = self.callback()
        if data is None:
            return

        # Rotate the values
        if len(self.values["time"]) >= self.max_entries:
            for column in self.columns + ["time"]:
                self.values[column].pop(0)

        # Append the new data
        for column in self.columns + ["time"]:
            self.values[column].append(data[column])

        # Update the lines
        for column in self.columns:
            self.lines[column].set_data([self.values["time"]], [self.values[column]])

        # Reset the axis limits
        self.ax.relim()
        self.ax.autoscale_view()

        # Update the legend
        handles = []
        for column in self.columns:
            handles.append(
                patches.Patch(
                    color=self.config[column]["color"],
                    label=f"{column}: {self.values[column][-1]:.2f}{self.config[column]['suffix']}"
                )
            )
        self.ax.legend(handles=handles, loc="upper left")

        return self.lines.values()