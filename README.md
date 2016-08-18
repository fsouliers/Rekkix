# Rekkix
Rekkix is a requirement traceability matrix generator written in Qt/C++. Let's say it's a kind of cheap Reqtify(tm) or TBReq(tm).

Input files for Rekkix can be:
* MS Word docx files
* LibreOffice odt files
* Plain text files
* Pdf files (well ... some day, because I didn't implemented it yet)

Rekkix has been inspired by reqflow, but slightly different in its approach :
* first of all, it has a GUI and merely use the Qt framework (libzip is also used to read odt & docx files)
* the biggest difference is that rekkix handles composite requirements
* rekkix is able to generate html & csv reporting of its analysis
* once I have implemented it ... that means one day (or night) ... Rekkix will be able to follow requirements in the gui, search for requirements, etc ...





