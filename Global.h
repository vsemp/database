#include <string>
#include <vector>
#include "expr.h"

#ifndef	GLOBAL_H
	#define GLOBAL_H
	
char* toChars( std::string s );
std::vector<std::string> tokenize_string( std::string s, const char * delimiter  );
std::vector<std::string> intercept_cols( std::vector<std::string> vs1, std::vector<std::string> vs2 );
bool compare_without_star( std::string s1, std::string s2 );
int get_col_pos(std::vector<std::string> header, std::string col );
std::string join_headers( std::string header1, std::string header2 );
std::string join_tuples( std::string s1, std::string s2 );
bool is_number(std::string s); 



	#endif 