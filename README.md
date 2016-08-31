# Rekkix
Rekkix is a requirement traceability matrix generator written in Qt/C++. Let's say it's a kind of cheap Reqtify(tm) or TBReq(tm).

Input files for Rekkix can be:
* MS Word docx files
* LibreOffice odt files
* Plain text files, such as html, latex or even source code files
* Pdf files, using libpoppler

Rekkix has been inspired by reqflow, but slightly different in its approach :
* first of all, it has a GUI and merely use the Qt framework (libzip is also used to read odt & docx files and libpoppler is used for pdf)
* technically, rekkix is multi-threaded and therefore runs much faster on our multi-core computers
* the biggest difference is that rekkix handles composite requirements (it means requirements only made of several other requirements) and detects many inconsistencies in the requirements definition (requirements expected but never defined, requirements defined several times, and so on)
* rekkix is able to generate html & csv reporting of its analysis
* once I have implemented it ... that means one day (or night) ... Rekkix will be able to follow requirements in the gui (useful for impact analysis), search for requirements, etc ...




