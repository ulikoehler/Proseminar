#include <string>
using namespace std;

/**
 * Utility function to extract the filename from the path
 */
string getFilenameFromPath(const string& path) {
	size_t sepIndex = path.find_last_of('/');
	if(sepIndex == std::string::npos) {return path;}
	return path.substr(sepIndex+1);
}
