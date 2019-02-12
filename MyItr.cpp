#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include "Global.h"
#include "MyItr.h"
#include "Executor.h"

using namespace std;
typedef basic_string<char> string;

extern Executor executor;



MyItr::MyItr( string _filename, int lc )
{
  filename = _filename;
  load_size = lc;
  cur_file_pos = -1;
  cur_pos = 0;
  //open();
  EOI.assign( "*EOI" );
}

MyItr::MyItr(string t_name, std::vector<std::string> _tuples )
{
  filename = t_name;
  cur_file_pos = -1;
  cur_pos = 0;
  offset_cnt = 0;
  EOI.assign( "*EOI" );
  _tuples.push_back( EOI ); // mark the end of itr
  tuples = _tuples;
  //itr = tuples.begin();

}



void MyItr::operator += ( MyItr &lhs )
{
  vector<string>::iterator lhs_beg = lhs.tuples.begin();
  vector<string>::iterator lhs_ed = lhs.tuples.end();

  lhs_beg++; // get rid of the headers
  tuples.erase( tuples.end() );
  tuples.insert( tuples.end(), lhs_beg, lhs_ed ); 
  //print_all();
}

bool MyItr::is_open()
{
  if( tuples.size() > 0 )
    return false;
  else
    return true;
} 

void MyItr::open()
{
  // check if it is a temporary itr ( AKA, no file read is needed )
  if ( filename == EOI )
  {
    itr = tuples.begin();
    cur_file_pos = 0;
    return;
  }

  // check filename exist and it is valid
  if ( filename.size() == 0 )
  {
    cerr << "MyItr: No file specified. " << endl; 
    return;
  }

  // construct the correct file path
  path.assign( "./data/" );
  path.append ( filename );
  path.append ( ".tbl" ); 

  // trying open the file
  fs.open ( path.c_str() );
  if ( !fs.is_open() )
  {
    cerr << "MyItr: file name is invalid. " << endl;
    return;
  } 

  fs.close();

  // load the file 
  tuples.clear();
  cur_pos = 0;
  offset_cnt = 0;
  load();

  itr = tuples.begin();
  offsets_itr = offsets.begin();

}


string MyItr::get_next()
{
  if( cur_pos < tuples.size() )
  {
    //offsets_itr++;
    //cout << "!" << tuples[cur_pos] << endl;
    offset_cnt++;
    return tuples[cur_pos++];
  }
  else
  {
    if ( tuples[tuples.size()-1].compare( EOI ) == 0  )
      return EOI;
    load();
    return get_next();
  }
    
}

std::ios_base::streampos MyItr::get_cur_pos()
{
  return offsets[offset_cnt];
} 

void MyItr::close()
{
  if ( fs.is_open() )
    fs.close();
  //tuples.clear();
}

bool MyItr::hasMore ()
{

  if ( tuples[tuples.size() - 1].compare ( EOI ) == 0 )
    return false;
  else
    return true;
}

void MyItr::load()
{
  string line;
  int load_cnt = 0;
  streampos cur_pos = 0;
  fs.open( path );

  if( !fs.is_open() )
  {
    tuples.push_back( string( "*EOI" ) );
    cerr << "ERROR openning file in MyItr::load(). " << endl;
    return;
  }

  if (tuples.size() != 0 )
  {
    fs.clear();
    fs.seekg( cur_file_pos, fs.beg );
  }

  cur_pos = fs.tellg();

  while (load_cnt < load_size && getline( fs, line ) && line[0] != '@' )
  {
    offsets.push_back( cur_pos );
    tuples.push_back( line );
    load_cnt++;

    cur_pos = fs.tellg();
  }

  // if eof then put EOI to the end
  if ( fs.eof() )
    tuples.push_back( EOI );

  cur_file_pos = fs.tellg();
  fs.close();
}

string MyItr::first( )
{
  if ( tuples.size() > 0)
  {
    cur_pos = 1;
    return tuples[0];
  }
  else
    return EOI;
}

void MyItr::print_all()
{
  for ( int i = 0; i < tuples.size(); i++ )
  {
    std::cout << tuples[i] << std::endl;
  }
}


int MyItr::get_col_pos( std::string s )
{
  int i = 0;
  char str[tuples[0].size()];
  char * token;

  strcpy( str, tuples[0].c_str() );

  token = strtok ( str, "\t" );
  while ( token )
  {
    if ( s.compare( token ) == 0 )
      return i;

    i++;
    token = strtok(NULL, "\t"  );
  }

  return -1;
}

int POS;

bool sortfunction(string i, string j)
{
  //cout <<"!"<< i << " " << j << endl;
  char si[i.size()+1];
  char sj[j.size()+1];
  string targeted_i;
  string targeted_j;

  strcpy( si, i.c_str() );
  strcpy( sj, j.c_str() );

  char * token = strtok (si, "\t" );

  for (int cnt = 0; cnt < POS; cnt++ )
    token = strtok( NULL, "\t" );
  targeted_i.assign( token );

  token = strtok( sj, "\t" );  
  for (int cnt = 0; cnt < POS; cnt++ )
    token = strtok( NULL, "\t" );
  targeted_j.assign( token );
  return targeted_i.compare( targeted_j ) < 0;
  /*
  cout << ( i.compare(  j ) < 0 ) << endl; 
  //return i.compare(  j ) < 0;
  return i.compare(  j ) < 0;
  */
}

void MyItr::sort( std::string sortkey )
{
  POS = get_col_pos( sortkey );

  if ( POS == -1 )
  {
    cerr << "MyItr: Invalid sortkey." << endl;
    return; 
  }
  //cout << *(tuples.end()--) << endl;
  //print_all();
  vector<string>::iterator s = tuples.begin();
  s++;
  vector<string>::iterator e = tuples.end();
  e--;
  std::sort (s, e, sortfunction );
}


char * MyItr::dump_table( string table_name )
{
  // construct path name
  string path ( "data/");
  path.append( table_name );

  ofstream ofs( path );

  if ( !ofs.is_open() )
  {
    cerr << "ERROR: opening file in MyItr::dump_table()."  << endl;
    return NULL ;
  }

  for ( int i = 0 ; i < tuples.size() - 1; i++  ) // tuple.size() - 1 because we dont want the last tuple that marks the end of the iterator
  {
    ofs << tuples[i] << endl;
  }

  char * to_return = new char[path.size()+1];
  strcpy ( to_return, path.c_str() );

  return to_return;
}

bool MyItr::is_last()
{
  //cout << cur_pos << " " << tuples[cur_pos] << endl;
  if ( tuples[cur_pos-1].compare( EOI ) == 0 )
    return true;
  else
    return false;
}

std::vector<int> MyItr::get_positions( std::string & tuple, std::vector<std::string> & intercept )
{
  std::vector<int> to_return;
  std::vector<std::string> vs = tokenize_string( tuple, "\t" );

  for ( int i = 0; i < vs.size(); i++ )
  {
    for ( int j = 0; j < intercept.size(); j++ )
    {
      if ( compare_without_star( vs[i], intercept[j] ))
      {
        to_return.push_back( i );
        break;
      }
    } // current col_name is not in intercept,
  }

  return to_return;
} // to get the position of the attributes that are not in intercept 

std::vector<int> MyItr::get_positions( std::string & tuple, std::vector<std::string> & intercept, std::vector<int> & not_intercept )
{
  std::vector<int> to_return;
  std::vector<std::string> vs = tokenize_string( tuple, "\t" );

  for ( int i = 0; i < vs.size(); i++ )
  {
    int j;
    for ( j = 0; j < intercept.size(); j++ )
    {
      if ( compare_without_star( vs[i], intercept[j] ))
      {
        to_return.push_back( i );
        break;
      }
    } // current col_name is not in intercept,
    if ( j >= intercept.size() )
    {
      //cout << i << endl;
      not_intercept.push_back( i );
    } // when current column is not 
  }

  return to_return;
}

std::string MyItr::eliminate_intercept( std::vector<std::string> tuple, std::vector<int> poses )
{
  std::string to_return;

  for( int i = 0; i < tuple.size(); i++ )
  {
    int j;
    for ( j = 0; j < poses.size(); j++ )
    {
      if ( poses[j] == i )
        break;
    }
    if ( j >= poses.size() )
    {
      to_return.append( tuple[i] + std::string( "\t" ) );
    }
  }

  return to_return;
} // eliminate the columns specified by the position on poses

std::string MyItr::strings_match( std::string s1, std::vector<int> poses1, std::string s2, std::vector<int> poses2, std::vector<int> & intercept )
{
  // check if the intercept parts are the same, if not, return EOI
  std::vector<std::string> vs1 = tokenize_string( s1, "\t" );
  std::vector<std::string> vs2 = tokenize_string( s2, "\t" );
  std::string to_return;

  for ( int i = 0; i < poses1.size(); i++ )
  {
    if ( !compare_without_star( vs1[poses1[i]], vs2[poses2[i]] ))
    {
      return EOI;
    }
  }

  to_return.append( eliminate_intercept( vs1, poses1 ) );
  to_return.append( eliminate_intercept( vs1, intercept)  );
  to_return.append( eliminate_intercept( vs2, poses2 ) );
 
  return to_return; 


}



MyItr * MyItr::nested_join( MyItr & rhs )
{
  open();
  rhs.open();
  std::string header1 = first();
  std::string header2 = rhs.first();
  std::vector<std::string> new_table;
  std::vector<std::string> intercept = intercept_cols( tokenize_string( header1, "\t" ) , tokenize_string( header2, "\t" ) );



  //cout << header << endl;
  std::string header = join_headers( header1, header2 );
  new_table.push_back( header );
  // nested loop now 
  std::string outter = get_next();
  while ( !is_last() )
  {
    std::string inner = rhs.first();
    inner = rhs.get_next();

    while ( !rhs.is_last() )
    {
      // cout << "!" << outter << " " << inner << endl;
      std::vector<int> temp;
      std::string s = strings_match( outter, get_positions( header1, intercept, temp ), inner, get_positions( header1, intercept ), temp );
      if ( s.compare( EOI ) != 0 )
      {
        //cout << s << endl;
        new_table.push_back( s );
      }

      inner = rhs.get_next();
    }
    //cout << outter << endl;
    outter = get_next();
  }
  //cout << header << endl;
  MyItr * temp_itr = new MyItr ( std::string( "product_temp1" ),  new_table  );
  executor.add_table( "product_temp1", header, temp_itr ) ;
  return temp_itr;
}


MyItr * MyItr::nested_join( std::string h, std::string tuple )
{
  open();
  std::string header1 = first();
  std::string header2 = h;
  std::vector<std::string> new_table;
  std::vector<std::string> intercept = intercept_cols( tokenize_string( header1, "\t" ) , tokenize_string( header2, "\t" ) );
  std::vector<int> h_poses = get_positions( header1, intercept );


  //cout << header << endl;
  std::string header = join_headers( header1, header2 );
  new_table.push_back( header );
  // nested loop now 
  std::string outter = get_next();
  while ( !is_last() )
  {

    std::vector<int> temp;
    std::string s = strings_match( outter, get_positions( header1, intercept, temp ), tuple, h_poses, temp );
    if ( s.size() > 0 && s.compare( EOI ) != 0 )
    {
      //cout << s << endl;
      new_table.push_back( s );
    }

    outter = get_next();
  }
  //cout << header << endl;
  MyItr * temp_itr = new MyItr ( std::string( "product_temp1" ),  new_table  );
  executor.add_table( "product_temp1", header, temp_itr ) ;
  return temp_itr;
/*
*/
  return NULL;
}