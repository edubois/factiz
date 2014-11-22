Import( 'project' )
Import( 'libs' )


factizFlags = { 'LIBPATH': [project.inOutputLib()],
                'CCFLAGS': [project.CC['warning3'],project.CC['sharedobject'] ],
                'CXXFLAGS':[],
                'CPPDEFINES':[],
             }

if project.env['mode'] == 'production' :
	factizFlags['CPPDEFINES'].append( 'FACTIZ_PRODUCTION' )
	if 'visibilityhidden' in project.CC:
		factizFlags['SHCCFLAGS'] = [project.CC['visibilityhidden']]

# If your compiler as a flag to mark undefined flags as error in shared libraries
if 'sharedNoUndefined' in project.CC:
	factizFlags['SHLINKFLAGS'] = [project.CC['sharedNoUndefined']]

SConscript( [
              'libraries/factiz/SConscript',
            ]
            +
            project.scanFiles( ['applications', 'tests'], accept=['SConscript'] )
          )

