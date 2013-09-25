#!/usr/bin/python
# -*- coding: utf-8 -*-

import re
from Tkinter import *
from Tix import *
from subprocess import *

class CoreInfo( Frame ):
    def __init__( self, parent, core, sensor, **params ):
        Frame.__init__( self, parent, **params )
        self.core = core
        self.sensor = sensor
        corelabel = Label( self, text=core )
        corelabel.pack( side=LEFT, fill=BOTH, expand=NO )
        self.tempMeter = Meter( self, text="", value=0 )
        self.tempMeter.pack( side=LEFT, fill=BOTH, expand=NO )
        self._update( self.core )

    def _update( self, core ):
        tempString = self.sensor.getTemperature( core )
        tempFloat = float( tempString )
        if tempFloat < 60:
            color = "lightgreen"
        elif tempFloat < 80:
            color = "yellow"
        else:
            color = "red"
        self.tempMeter.configure(
                fillcolor=color, text=tempString + "℃", value=tempFloat/100 )
        self.after( 3000, self._update, core )

class Dashboard:
    def __init__( self, parent, sensor, **params ):
        self.parent = parent
        self.sensor = sensor
        self.coreinfo = {}
        self.parent.title( "CPU Temp" )
        for core in sensor.cores:
            self.coreinfo[ core ] = CoreInfo(
                        self.parent, core, sensor, bd=2, relief=RIDGE )
            self.coreinfo[ core ].pack( side=TOP, fill=BOTH, expand=YES )

class Sensor:
    def __init__( self ):
        self.cores = self._getCores()

    def _getCores( self ):
        cores = []
        output = Popen( [ "sensors" ], stdout=PIPE )
        #Core 3:        +52.0°C  (high = +80.0°C, crit = +99.0°C) 
        for line in output.stdout:
            match = re.match( r"(Core\s+\d+):\s+\S+", line )
            if match:
                cores.append( match.group(1) )
        return cores

    def getTemperature( self, core ):
        temperature = 0
        output = Popen( [ "sensors" ], stdout=PIPE )
        #Core 3:        +52.0°C  (high = +80.0°C, crit = +99.0°C) 
        for line in output.stdout:
            match = re.match( core + r":\s+\S(\d+\.\d+)", line )
            if match:
                temperature = match.group(1)
        return temperature

if __name__ == '__main__':
    sensor = Sensor()
    root = Tk()
    root.resizable( 0, 0 )
    dashboard = Dashboard( root, sensor )
    root.mainloop()
