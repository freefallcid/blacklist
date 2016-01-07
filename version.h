#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "06";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2015";
	static const char UBUNTU_VERSION_STYLE[] =  "15.06";
	
	//Software Status
	static const char STATUS[] =  "Release Candidate [raspberry port]";
	static const char STATUS_SHORT[] =  "rc";
	
	//Standard Version Type
	static const long MAJOR  = 0;
	static const long MINOR  = 3;
	static const long BUILD  = 56;
	static const long REVISION  = 276;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 56;
	#define RC_FILEVERSION 0,3,56,276
	#define RC_FILEVERSION_STRING "0, 3, 56, 276\0"
	static const char FULLVERSION_STRING [] = "0.3.56.276";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 56;

}
#endif //VERSION_H

/* vim: set ts=2 sw=2 tw=0 et :*/
