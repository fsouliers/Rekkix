# rekkix
# released under the terms of the GNU General Public License

# Often changed parameters
CONFIG += release
VERSION = 0.1.0

# The project is an application and we should avoid generating several configuration
TEMPLATE = app
CONFIG -= debug_and_release debug_and_release_target

# Rekkix is coded using c++11 (eg, use of lamba function)
CONFIG += c++11

# Config targets
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

# Folders used for dependencies
DEPENDPATH += ./src ./inc

# Included folders at compile time
INCLUDEPATH += ./inc ./ui_inc

# Preprocessor defines (-D directives)
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# Third party library : libzip
win32:INCLUDEPATH += ./win32_libs/libzip-1.1.3/inc/ ./win32_libs/poppler-0.45/include/poppler/cpp/
win32:LIBS += -L./win32_libs/libzip-1.1.3/lib/ -L./win32_libs/poppler-0.45/bin/ C:/Qt/Tools/mingw530_32/i686-w64-mingw32/lib/libiconv.a
LIBS += -lzip -lz -lpoppler-cpp 


# all objects are generated at the same place
OBJECTS_DIR = ./obj

# Same as OBJECTS_DIR but with moc generated source files
MOC_DIR = ./moc

# Same again but with resource generated files
RCC_DIR = ./rcc

# Folder in which uic will output its generated files
UI_DIR = ./ui_inc

# output folder 
win32:DESTDIR = ./bin/windows
unix:DESTDIR = ./bin/linux
macx:DESTDIR = ./bin/macx



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
           ./inc/RequirementFile_odt.h \ 
           ./inc/RequirementFile_pdf.h \ 
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
           ./srcxx/RequirementFile_odt.cpp \
           ./srcxx/RequirementFile_pdf.cpp \
           ./srcxx/RequirementFile_txt.cpp 
           

