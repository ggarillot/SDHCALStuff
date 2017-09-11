########################################################
# cmake file for building Marlin example Package
# @author Jan Engels, Desy IT
CMAKE_MINIMUM_REQUIRED(VERSION 3.6 FATAL_ERROR)
########################################################

# project name
PROJECT( Minimisation )

### DEPENDENCIES ############################################################

FIND_PACKAGE( ILCUTIL REQUIRED COMPONENTS ILCSOFT_CMAKE_MODULES )

# load default settings from ILCSOFT_CMAKE_MODULES
INCLUDE( ilcsoft_default_settings )

FIND_PACKAGE( ROOT 6.08 REQUIRED COMPONENTS TMVA XMLIO Minuit MLP MathMore MathCore Minuit2 RooFit RooFitCore) # minimum required Marlin version
INCLUDE_DIRECTORIES( SYSTEM ${ROOT_INCLUDE_DIRS} )
LINK_LIBRARIES( ${ROOT_LIBRARIES} )
ADD_DEFINITIONS( ${ROOT_DEFINITIONS} )

### LIBRARY #################################################################

# definitions to pass to the compiler
ADD_DEFINITIONS( "-Wall -ansi -pedantic -std=c++11" )
ADD_DEFINITIONS( "-Wno-long-long" )

# include directories
INCLUDE_DIRECTORIES( ./include )

# add library
AUX_SOURCE_DIRECTORY( ./src library_sources )

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)

file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)

add_library( MyLib ${sources} ${headers} )


file( GLOB APP_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cxx )
foreach( testsourcefile ${APP_SOURCES} )
	get_filename_component(testname ${testsourcefile} NAME_WE)
	add_executable( ${testname} ${testsourcefile} )
	target_link_libraries( ${testname} MyLib ${ROOT_COMPONENT_LIBRARIES} )
	install(TARGETS ${testname} RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)
endforeach( testsourcefile ${APP_SOURCES} )


#SET (Minim ./src/main.cxx)
#SET (TuneElectrons ./src/tuneElectrons.cxx)
#SET (trainNN ./src/trainNN.cxx)
#SET (applyNN ./src/applyNN.cxx)

#SET (createOKTrees ./src/createOKTrees.cxx)

##set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/")

#ADD_EXECUTABLE( Minim ${Minim} ${headers} ${sources} )
#ADD_EXECUTABLE( TuneElectrons ${TuneElectrons} ${headers} ${sources} )
#ADD_EXECUTABLE( trainNN ${trainNN} ${headers} ${sources} )
#ADD_EXECUTABLE( applyNN ${applyNN} ${headers} ${sources} )
#ADD_EXECUTABLE( createOKTrees ${createOKTrees} ${headers} ${sources} )

#TARGET_LINK_LIBRARIES( Minim ${ROOT_COMPONENT_LIBRARIES} )
#TARGET_LINK_LIBRARIES( TuneElectrons ${ROOT_COMPONENT_LIBRARIES} )
#TARGET_LINK_LIBRARIES( trainNN ${ROOT_COMPONENT_LIBRARIES} )
#TARGET_LINK_LIBRARIES( applyNN ${ROOT_COMPONENT_LIBRARIES} )
#TARGET_LINK_LIBRARIES( createOKTrees ${ROOT_COMPONENT_LIBRARIES} )

#INSTALL(TARGETS Minim TuneElectrons trainNN applyNN createOKTrees RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)
