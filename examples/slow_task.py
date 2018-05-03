from unreal_engine import FSlowTask
import time

# Create an FSlowTask object, defining the amount of work that
# will be done, and the initial message.
t = FSlowTask(10, "Doing Something")
t.initialize()

# Make the dialog, and include a Cancel button (default is not to
# allow a cancel button).
t.make_dialog(True)

time.sleep(1)

for i in range(10) :
    # Update the progress bar. Note that the first argument is the
    # amount of work to be done this frame, not the overall work
    # done so far.
    t.enter_progress_frame(1, "Progress Position : {}".format(i))
    time.sleep(0.2)

    # If there was a cancel button included, we can check if it was
    # pressed.
    if t.received_user_cancel():
        print("Cancelled")
        break

t.destroy()
