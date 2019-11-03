#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "03";
	static const char MONTH[] = "11";
	static const char YEAR[] = "2019";
	static const char UBUNTU_VERSION_STYLE[] =  "19.11";
	
	//Software Status
	static const char STATUS[] =  "Release Candidate [vserver]";
	static const char STATUS_SHORT[] =  "rc";
	
	//Standard Version Type
	static const long MAJOR  = 0;
	static const long MINOR  = 4;
	static const long BUILD  = 66;
	static const long REVISION  = 276;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 66;
	#define RC_FILEVERSION 0,4,66,276
	#define RC_FILEVERSION_STRING "0, 4, 66, 276\0"
	static const char FULLVERSION_STRING [] = "0.4.66.276";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 66;

}
#endif //VERSION_H

/* vim: set ts=2 sw=2 tw=0 et :*/
