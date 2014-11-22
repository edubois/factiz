EnsureSConsVersion( 1, 2 )
EnsurePythonVersion( 2, 5 )

import os
import sys

sys.path.append('tools')
from sconsProject import SConsProject

class FactizMatchProject( SConsProject ):
    def getOutputMayaPlugin( self ) :
        name = self.getName()
        return os.path.join( self.dir_output_bin, name, self.getOfxPlatformName(), name + '.so' )

#______________________________________________________________________________#

project = FactizMatchProject()


Export('project')
Export({'libs':project.libs})

#______________________________________________________________________________#

project.begin()
project.SConscript()
project.end()



