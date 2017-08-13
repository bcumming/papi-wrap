#pragma once
// enuemerate the possible results of adding an event to an event set
//      PESsuccess = succuesfully added event to set
//      PESconflict = there was a conflict with one or more of the counters already in the set
//      PESunavailable = requested event is not available (maybe wrong hardware)
//      PESinvalid = invalid event requested
//      PESuninitialized = the event set has not been initialized
//      PESerror = all other errors
enum PapiEventSetReturn {PESsuccess, PESconflict, PESunavailable, PESinvalid, PESerror, PESuninitialized};

// enumerate the different output formats for counter information
// LaTex support not currently implemented
enum papiFileFormat {fileFormatMatlab, fileFormatPlain, fileFormatLaTeX};


