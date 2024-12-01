#include <iostream>
#include <string>

// Why are they different? One year later, I know why: Because one's in the C++-standard and the other one isn't!
// Hint: the latter one is in the standard
#define UNUSED			__attribute__((unused))
#define MAYBE_UNUSED	[[maybe_unused]]

/** @name initialize
 *  @brief function called to initialize global variable or other things
 *  @param lineCount: number of lines the input file has
 *  @return `void`
*/
void initialize(uint64_t lineCount);

/** @name handleLine
 * 	@brief parser-function called for every line of the input file
 *  @param line: current line of the file, read in order
 *  @return `bool`: whether to terminate the program early or not.
 * 	return `true` to continue execution, `false` to terminate early. This will result in `finalize()` being skipped!
 **/
bool handleLine(const std::string& line);

/** @name finalize
 * 	@brief function called when all lines have been parsed successfully. Usally this is used to print the final result.
 *  @return `void`
 **/
void finalize();
