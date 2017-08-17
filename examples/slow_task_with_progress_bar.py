from unreal_engine import SWindow, SProgressBar, SButton, SVerticalBox
import unreal_engine as ue


progress_bar = SProgressBar()
canceled = False

def cancel_operation():
    global canceled
    ue.log_warning('slow operation canceled')
    canceled = True

SWindow(title='slow task', sizing_rule=1)(
    SVerticalBox()
    (
        progress_bar
    )
    (
        SButton(text='cancel', on_clicked=cancel_operation)
    )
)

for i in range(0, 10000):
    progress_bar.set_percent(1/10000 * i)
    ue.log('slow task iteration: {0}'.format(i))
    if canceled:
        break
    ue.slate_tick()
