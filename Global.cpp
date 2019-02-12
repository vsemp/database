#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include "Global.h"

char* toChars( std::string s ) {

  char *c = new char[s.size() + 1];
  c[s.size()] = '\0';
  memcpy( c, s.c_str(), s.size() );
}


std::vector<std::string> tokenize_string( std::string s, const char * delimiter  )
{
  char * ss = toChars( s );
  char * token;
  std::vector<std::string> to_return;

  token = strtok( ss, delimiter );
  while ( token )
  {
    to_return.push_back( std::string( token ) );
    token = strtok( NULL, delimiter );
  }

  return to_return;

}

bool compare_without_star( std::string s1, std::string s2 )
{
  std::string ss1;
  std::string ss2;
  if ( s1[0] == '*' )
    ss1 = s1.substr( 1, s1.size() );
  else 
    ss1 = s1;

  if ( s2[0] == '*' )
    ss2 = s2.substr( 1, s2.size() );
  else 
    ss2 = s2;

  return ss1.compare( ss2 ) == 0;

}

std::vector<std::string> intercept_cols( std::vector<std::string> vs1, std::vector<std::string> vs2 )
{
  std::vector<std::string> to_return;
  for( int i = 0; i < vs1.size(); i++ )
  {
    for( int j = 0; j < vs2.size(); j++ )
    {
      if ( compare_without_star( vs1[i], vs2[j] ))
      {
        to_return.push_back( vs1[i] );
      }
    }
  }
  return to_return;
}

int get_col_pos(std::vector<std::string> header, std::string col ){

  int i = 0;
  if( col[0] == '*' || col[0] == '&' )
    col = col.substr( 1, col.size() );

  for ( std::vector<std::string>::iterator itr = header.begin() ; itr != header.end(); itr++ )
  {

    if ( (*itr).compare( col ) == 0){
      return i;
    }
    i++;
  }

  return -1;
  
}

std::string join_headers( std::string header1, std::string header2 )
{
  std::vector<std::string> vs1 = tokenize_string( header1, "\t" );
  std::vector<std::string> vs2 = tokenize_string( header2, "\t" );
  std::vector<std::string> intercept = intercept_cols( vs1, vs2 );
  std::string header;


  for ( int i = 0; i < vs1.size(); i++ )
  {
    int j = 0;
    for ( j = 0; j < intercept.size(); j++ )
    {
      if ( compare_without_star( vs1[i], intercept[j] ))
        break;
    }
    if ( j == intercept.size() )
    {
      if ( vs1[i][0] == '*' )
        header.append( vs1[i].substr( 1, vs1[i].size() ) );
      else
        header.append( vs1[i] );
      header.push_back( '\t' );
    } // current col_name is not in intercept,
  }

  for ( int i = 0; i < intercept.size(); i++)
  {
      if ( intercept[i][0] == '*' )
        header.append( intercept[i].substr( 1, vs1[i].size() ) );
      else
        header.append( intercept[i] );

      header.push_back( '\t' );
  }

  for ( int i = 0; i < vs2.size(); i++ )
  {
    int j = 0;
    for ( j = 0; j < intercept.size(); j++ )
    {
      if ( compare_without_star( vs2[i], intercept[j] ))
        break;
    }

    if ( j == intercept.size() )
    {
      if ( vs2[i][0] == '*' )
        header.append( vs2[i].substr( 1, vs2[i].size() ) );
      else
        header.append( vs2[i] );

      header.push_back( '\t' );
    } // j reach the end, current string is not in intercept
  }

  return header;

}


bool is_number(std::string s){
  //scan();
  std::string::const_iterator it = s.begin();
  while ( it != s.end() && isdigit( *it ) )
    it++;
  return !s.empty() && it == s.end();
}


