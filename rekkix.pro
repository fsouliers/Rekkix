# rekkix
# released under the terms of the GNU General Public License

# The project is an application
TEMPLATE = app

# The generated Makefile handles a debug and a release configuration
CONFIG += c++11
CONFIG += release


CONFIG(debug, debug|release):TARGET = rekkix_dbg
CONFIG(release, debug|release):TARGET = rekkix


# When using windows in debug mode, the console config item is needed
# to see something in a DOS window
win32 {
 CONFIG(debug, debug|release):CONFIG += console
 CONFIG(release, debug|release):CONFIG += windows
}

# The widgets component has to be loaded
QT += core widgets xml

# Version Number
VERSION = 0.1.0

# Folders used for dependencies
DEPENDPATH += ./src ./inc

# Included folders at compile time
INCLUDEPATH += ./inc ./ui_inc

# Preprocessor defines (-D directives)
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

# Third party library : libzip
win32:INCLUDEPATH += ./win32_libzip/inc
win32:LIBS += -L./win32_libzip/lib/
LIBS += -lzip -lz


# all objects are generated at the same place
OBJECTS_DIR = ./obj

# Same as OBJECTS_DIR but with moc generated source files
MOC_DIR = ./moc

# Same again but with resource generated files
RCC_DIR = ./rcc

# Folder in which uic will output its generated files
UI_DIR = ./ui_inc

# output folder 
DESTDIR = ./bin



# Resources file
RESOURCES = ./resources/rekkix.qrc

# user interface files
FORMS += ./ui/Rekkix.ui

# headers
HEADERS += ./inc/AnalysisError.h \
           ./inc/IRequirementFile.h \
           ./inc/FactoryReportBaseString.h \
           ./inc/FactoryRequirementFile.h \
           ./inc/ModelCompositeReqs.h \
           ./inc/ModelConfiguration.h \
           ./inc/ModelConfigurationErrors.h \
           ./inc/ModelReqs.h \
           ./inc/ModelReqsCoveredDownstream.h \
           ./inc/ModelReqsCoveringUpstream.h \
           ./inc/ModelSngAnalysisErrors.h \
           ./inc/ModelSngReqMatrix.h \
           ./inc/ModelStreamDocuments.h \
           ./inc/Rekkix.h \
           ./inc/Requirement.h \
           ./inc/RequirementFile_docx.h \ 
           ./inc/RequirementFile_txt.h

# sources
SOURCES += ./srcxx/AnalysisError.cpp \
           ./srcxx/IRequirementFile.cpp \
           ./srcxx/FactoryReportBaseString.cpp \
           ./srcxx/FactoryRequirementFile.cpp \
           ./srcxx/main.cpp \
           ./srcxx/ModelCompositeReqs.cpp \
           ./srcxx/ModelConfiguration.cpp \
           ./srcxx/ModelConfigurationErrors.cpp \
           ./srcxx/ModelReqs.cpp \
           ./srcxx/ModelReqsCoveredDownstream.cpp \
           ./srcxx/ModelReqsCoveringUpstream.cpp \
           ./srcxx/ModelSngAnalysisErrors.cpp \
           ./srcxx/ModelSngReqMatrix.cpp \
           ./srcxx/ModelStreamDocuments.cpp \
           ./srcxx/Rekkix.cpp \
           ./srcxx/Requirement.cpp \
           ./srcxx/RequirementFile_docx.cpp \
           ./srcxx/RequirementFile_txt.cpp 
           

