import unreal_engine as ue


class Sight:

    def __init__(self):
        ue.log_warning('Hello i am __init__')
        self.timer = 1.0
              
    def begin_play(self):
        ue.log_warning('Hello i am begin_play')        

    def tick(self, delta_time):
        self.timer -= delta_time
        if self.timer <= 0:
            ue.log_error('1 second elapsed !')
            self.timer = 1.0