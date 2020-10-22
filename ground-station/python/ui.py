#only works with python 3.5 to 3.7
from kivy.app import App
from kivy.uix.widget import Widget
from kivy.properties import (
    NumericProperty, ReferenceListProperty, ObjectProperty
)
from kivy.vector import Vector
from kivy.clock import Clock
from kivy.uix.gridlayout import GridLayout


class ControlsFeedback(Widget):
    def getFeedBack(self):
        pass

class Instrument(Widget):
    reading = NumericProperty(0)

    def GetReading(self):
        pass

class InstrumentPanel(GridLayout):
    instrument1 = ObjectProperty(None)
    velocity_x = NumericProperty(0)
    velocity_y = NumericProperty(0)
    velocity = ReferenceListProperty(velocity_x, velocity_y)

    def move(self):
        self.pos = Vector(*self.velocity) + self.pos

    def GetReadings(self):
        self.instrument1.GetReading()

class CommandWidget(GridLayout):
    #panel1 = ObjectProperty(None)

    def start(self, vel=(4, 0)):
        pass

    def update(self, dt):
        pass
        #self.panel1.GetReadings()

        #if (self.ball.y < self.y) or (self.ball.top > self.top):
        #    self.ball.velocity_y *= -1

class CommandAndControlApp(App):
    def build(self):
        Command = CommandWidget()
        Command.start()
        Clock.schedule_interval(Command.update, 1.0 / 60.0)
        return Command


if __name__ == '__main__':
    CommandAndControlApp().run()