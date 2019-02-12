#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include "Global.h"
#include "Executor.h"

using namespace std;
typedef basic_string<char> string;

MyItr * DO_NOT_PRINT; 

bool Executor::existingFile( string file_name ){
  struct stat buff;
//cout << file_name << endl;
  if ( stat( file_name.c_str(), &buff) != -1){
  	return true;
  }
  else 
  	return false;
}



char* Executor::define_table( string table_name, int n_col, string* col_names ){

  ofstream writeTo;
  string first_row;
  string file_name( "./data/" );
  file_name.append( table_name );
  file_name.append( ".tbl" );

  // sta file path constructing
  string sta_path = get_sta_path( table_name );

  //cout << *col_names << endl; 

  if ( existingFile( file_name ) ){
    cout << "Table already exists" << endl;
    return FAIL;
  }
    
  writeTo.open( file_name.c_str() );

  if( !writeTo.is_open() ){
    cout << "Cannot create table" << endl;
    return FAIL;
  }

  // prepare for the sta file
  string sta = "0\t";

  // create the first row 

  for ( int i = 0; i < n_col; i++)
  {
    if ( col_names[i][0] == '*' )
    {
      sta.append( col_names[i].substr( 1, col_names[i].size() ) + string( "#" ) + string ( "0\t") );
    }

    first_row.append( col_names[i] + string( "\t" ) );
  }

  // write to the file
  writeTo << first_row << endl;
  writeTo.close();

  // build sta file 
  writeTo.open( sta_path );
  writeTo << sta << endl;
  writeTo.close();
  // update data base information in memory
  //ifstream file( file_name.c_str() );
  add_table( table_name, first_row );

  
  //cout << "here" << endl;
  return NULL;
}

string Executor::get_sta_path( string table_name )
{
  string sta_path( "./data/" );
  sta_path.append( table_name );
  sta_path.append( ".sta" );
  return sta_path;
}

char* Executor::insert_row( string table_name, int n_val, string* values ){
  int col_cnt = 0;
  ofstream table;
  string file_name( "./data/" );
  file_name.append( table_name );

  file_name.append( ".tbl" );

  // precondition check
  if ( !hasTable( table_name ) )
  {
    cerr << "Table does not exist" << endl;
    return FAIL;
  } // check if table exist

  col_cnt = Tables::get_colCnt(table_name);

  // - 1 because we the first element is the row count;
  if( n_val > col_cnt ){
    cerr << "Too many values" << endl;
    return FAIL;
  }
  else if( n_val < col_cnt ) {
    cerr << "Too few values" << endl;
    return FAIL;
  }

/*
  // update tuple_count
  FILE * file = fopen ( file_name.c_str(), "r+" );
  char line[21];
  char * token;
  int line_cnt = 0;

  if( !file )
  {
    cerr << "EMPTY FILE IN Executor::insert_row()." << endl;
    return NULL;
  }

  fgets( line, 20, file );
  token = strtok ( line, "\t#" );
  if ( token )
  {
    line_cnt = atoi( token );
  }
  else
  {
    cerr << "Token invalid in Executor::insert_row()." << endl;
    return NULL;
  }

  line_cnt++;
  string new_num = to_string ( line_cnt );

  for ( int i = 0; i < 10 - strlen( new_num.c_str() ); i++ )
  {
    new_num.append( "#" );
  }

  fseek( file, 0, SEEK_SET );
  fwrite( new_num.c_str(), sizeof(char), new_num.size(), file );
  fclose( file );
*/

  //cout << file_name << endl;
  table.open( toChars( file_name ), ios_base::app);
  string tuple; 
  streampos s_pos = 0;

  // construct the new tuple;
  if ( !table.is_open() )
  {
    cerr << "ERROR OPENING FILE IN insert_row()." << endl;
    return NULL;
  }

  s_pos = table.tellp();

  for ( int i = 0; i < n_val; i++)
    tuple.append( values[i] + string( "\t" ) );

  table << tuple << endl;
  table.close( );

  // get all the tuples that needs to be updates 
  //cout << get_sta_path( table_name ) << endl;
  fstream sta_fs( get_sta_path( table_name ) );
  if( !sta_fs.is_open() )
  {
    cerr << "sta file does not exist in Executor::insert_row(). " << endl;
    return NULL;
  }

  // get all the keys that nees to be update
  string line;
  char * token;
  int tot_cnt;
  vector<string> keys_to_update;
  vector<int> new_counts;

  getline( sta_fs, line );
  char * sta = toChars( line );

  token = strtok ( sta, "\t" );
  tot_cnt = atoi ( token );

  token = strtok ( NULL, "#\t" );
  
  while ( token )
  {
    // collect keys
    keys_to_update.push_back( string ( token ) );
    token = strtok ( NULL, "#\t" );

    // collect number
    int n = atoi( token );
    new_counts.push_back( n );
    token = strtok ( NULL, "#\t" );
  }

  
  // find the table, 
  Table * tbl = get_table( table_name );

//cout << s_pos << endl;
  for ( int i = 0; i < keys_to_update.size(); i++ )
  {
    int new_count;

    new_count = tbl->update_btree( keys_to_update[i], tuple, s_pos );
    new_counts[i] = new_count;
  } 

  // construct new sta and write to the file
  string write_from ( to_string( ++tot_cnt ) + string ( "\t" ) );

  for ( int i = 0; i < keys_to_update.size(); i++ )
  {
    write_from.append( keys_to_update[i] + string( "#" ) + to_string( new_counts[i] ) + string( "\t" ) );
  } 

  sta_fs.seekg( 0, sta_fs.beg );
  sta_fs << write_from << endl;

  sta_fs.close();

  return NULL;

}


void Executor::print_result(vector<string> col_headers,  vector<int> col_max_length, vector< vector <string >> wanted_tuples)
{

  cout << wanted_tuples.size() << " matches found." << endl;
  if ( wanted_tuples.size() == 0 )
    return;

cout << col_headers[0] << endl;

  if( !( col_max_length.size() == col_headers.size()  && wanted_tuples[0].size() == col_max_length.size() ) )
  {
    cout << "error in Executor::print_result() "<< endl;
    return;
  }

  // printing header
  for ( int i = 0; i < col_headers.size(); i++ )
  {
    cout << "| " << left <<  setw( col_max_length[i] )  << col_headers[i] ;
  }
  cout << "|" << resetiosflags(ios::showbase) << endl;

  // print a line
  for ( int i = 0; i < col_max_length.size(); i++ )
  {
    cout << "+ ";
    for ( int j = 0; j < col_max_length[i]; j++ )
      cout << "-"; 

  }
  cout << "+" << resetiosflags(ios::showbase) << endl;

  // print content
  for ( vector< vector <string> >::iterator itr = wanted_tuples.begin(); itr != wanted_tuples.end(); itr++ )
  {
    for ( int i = 0; i < col_max_length.size(); i++ )
    {
      cout << "| " << left <<  setw( col_max_length[i] )  << (*itr)[i] ;
    }
    cout << "|" << resetiosflags(ios::showbase) << endl;
    
  }
}
string UNEXPECTED( "*unexpected" );

string number( expr e )
{
  expression *ep = (expression *) e;
  //cout << ep->func << " " << ep->count << endl;
  string number = to_string ( ep->values[0].num );
  //cout << number << endl;
  return number;
}

string col_name( expr e )
{
  expression *ep = (expression *) e;
  return string( ep->values[0].name );
}

string get_string( expr  e)
{
  expression *ep = (expression *) e;

  string str( ep->values[0].data );
  return str;
}

string col_def( expr e )
{
  expression *ep = (expression *) e;
  if( ep->values[0].ep->func == OP_COLNAME )
    return col_name( ep->values[0].ep );
  else 
    return UNEXPECTED;

}

string column( expr e )
{
  expression *ep = (expression *) e;
  //cout << 
  if( ep->values[0].ep->func == OP_COLNAME )
    return col_name( ep->values[0].ep );
  else 
    return UNEXPECTED; 
}

void Executor::outcolname( expr e )
{
  expression *ep = (expression *) e;
  //cout <<  ep->values[1].ep->values[0].name  <<"!" << endl;
  requested_cols.insert( requested_cols.begin(),  ep->values[0].ep->values[0].name  );
  cast_cols.insert( cast_cols.begin(), ep->values[1].ep->values[0].name );

}

string Executor::sortspec( expression * ep )
{
  return string( ep->values[0].ep->values[0].name );
}

vector<string> Executor::Rlist( expr e )
{

  expression *ep = (expression *) e;
  vector<string> list;
  //cout << ep->values[0].ep->func << endl;

  while ( ep )
  {
    if (ep->values[0].ep->func == OP_COLUMNDEF)
    {
      if ( ep->values[1].num == 0 )
      {
      cout << ep->values[1].num << endl;
        keys.insert(keys.begin(), col_def( ep->values[0].ep ));
        list.insert(list.begin(), string("*") + col_def( ep->values[0].ep ));
        //cout << list[0] << endl;
      }
      else if ( ep->values[1].num == 3 )
      {
        keys.insert(list.begin(), col_def( ep->values[0].ep ));
        list.insert(list.begin(), string("*") + col_def( ep->values[0].ep ));
      }
      else
        list.insert(list.begin(), col_def( ep->values[0].ep ));
    }
    else if (ep->values[0].ep->func == OP_NUMBER)
      list.insert(list.begin(), number( ep->values[0].ep ));
    else if (ep->values[0].ep->func == OP_STRING)
      list.insert(list.begin(), get_string( ep->values[0].ep ));
    else if (ep->values[0].ep->func == OP_OUTCOLNAME )
      outcolname( ep->values[0].ep );
    else if (ep->values[0].ep->func == OP_SORTSPEC )
    {
      list.insert( list.begin(), sortspec( ep->values[0].ep ) );
    }
    else if (ep->values[0].ep->func == OP_NULL )
    {
      list.insert(list.begin(), string ( "null" ));
    }
    else 
    {
      cerr << "error in Rlist()" << endl;
    }

    ep = ep->values[1].ep;
  }
 // print_cml();
  return list;
}

void Executor::create_table(expr e){
  expression *ep = (expression *) e;
  string table_name;
  vector<string> cols;
  int col_cnt = 0;

  table_name.assign( ep->values[0].ep->values[0].name ); 
  ep = ep->values[1].ep;
  //cout << "ep func is " << ep->func << endl;

  cols = Rlist( ep );
  define_table(table_name, cols.size(), &cols[0] );


}

void Executor::insert(expr e ) 
{
  expression *ep = (expression *) e;
  string table_name;
  vector<string> cols;
  int col_cnt = 0;
  table_name.assign( ep->values[0].ep->values[0].name ); 
  ep = ep->values[1].ep;

  cols = Rlist( ep );

  insert_row(table_name, cols.size(), &cols[0] );
}


void print_ep ( expression *ep )
{
  cout << ep->func << endl;
  cout << ep->values[0].ep->func << " " << ep->values[1].ep->func << endl;
}

MyItr * Executor::selectrow( MyItr * itr, expr e )
{
  if ( !itr )
    return NULL;

  expression * ep = (expression *) e;
  itr->open();
  string cur_str = itr->first();
  vector<string> qualified;

  // check if column exist


  Table * tbl = get_table( itr->filename );

  // put the header to the first row
  qualified.push_back( cur_str );
  cur_str = itr->get_next();
  //cur_str = itr->get_next();

  while ( cur_str.compare( itr->EOI ) != 0 )
  {
    //cout << cur_str << endl;
    if ( ep->values[1].ep->func == OP_AND )
    {
      if ( tbl->AND( ep->values[1].ep, cur_str ) )
        qualified.push_back( cur_str );
    }
    else if  ( ep->values[1].ep->func == OP_OR ) 
    {
      if ( tbl->OR( ep->values[1].ep, cur_str ) )
        qualified.push_back( cur_str );
    }
    else
    {
      if ( tbl->comparison( ep->values[1].ep, cur_str ) )
        qualified.push_back( cur_str );
    }

    cur_str = itr->get_next();
  }

  return new MyItr(tbl->name, qualified ); 
}

MyItr * Executor::projectrow( MyItr * itr, expression * ep )
{

  if ( !itr )
    return NULL;

  Table * p_table = get_table( itr->filename );
  int tg_col_pos = -1;
  string line;
  vector<int> element_pos;

  // get information from the Rlist
  // and they are stored in the member in this class

  requested_cols.clear();
  cast_cols.clear();
  Rlist( ep->values[1].ep ); 

  vector<string> wanted_cols_headers;
  vector<string> actual_cols_headers;

  for ( int i = 0; i < requested_cols.size(); i++)
  {
    int pos = p_table->get_col_pos( requested_cols[i] );
    if( pos < 0 )
    {
      cerr << "Output column does not exist" << endl;
      return new MyItr();
    }
    else
    {
      wanted_cols_headers.push_back( cast_cols[i] ) ;
      actual_cols_headers.push_back( requested_cols[i] );
      element_pos.push_back( pos );
    }
  }

  // iterate through the whole itr
  string cur_str = itr->first();
  //cur_str = itr->get_next();
  vector<string> new_tuples;

  while ( cur_str.compare( "*EOI" ) != 0 )
  {
    //cout << cur_str << endl;
    string new_tuple;

    for( int i = 0; i < element_pos.size(); i++)
    {
      char * s = toChars( cur_str );
      char * token = strtok ( s, "\t" );
      for (int count = 0; count < element_pos[i]; count++ )
        token = strtok( NULL, "\t" );
      //cout << token << endl;
      new_tuple.append( string (token) );
      new_tuple.append( "\t" );
    }
    new_tuples.push_back (new_tuple );
    cur_str = itr->get_next();
  }

  return new MyItr ( p_table->name, new_tuples );

}


MyItr * Executor::indexselect( expression * ep )
{
  expression * condition = ep->values[1].ep;
  MyItr * itr;

  Table * tbl = get_table( string ( ep->values[0].ep->values[0].name ) );
  Tuple * to_find;
  BTreeNode * node = NULL;
  string key_to_btree( condition->values[0].ep->values[0].data  );
  
  if ( condition->values[1].ep->func == OP_STRING ) 
  {

    //keys.insert( pair<string, BTree*>( key_list[i], btree )  );cout << condition->values[1].ep->values[0].data << endl;
    to_find = new Tuple ( condition->values[1].ep->values[0].name , 0 );
  }
  else
  {
    to_find = new Tuple ( condition->values[1].ep->values[0].num , 0 );
  }

  itr = tbl->index_find( condition->func, key_to_btree, *to_find  );
  delete( to_find );

  return itr;
}


MyItr * Executor::product( expression * ep )
{
  // anaylise both tables' headers, get the 3 columns 
  Table * t1 = get_table( string ( col_name( ep->values[0].ep ) ) );
  Table * t2 = get_table( string ( col_name( ep->values[1].ep ) ) );

  if( t1 == NULL || t2 == NULL )
  {
    cerr << "ERROR: table does not exist in Executor::product()" << endl;
    return NULL;
  }

  MyItr * itr1 = t1->get_itr();
  MyItr * itr2 = t2->get_itr();

  itr1->open();
  itr2->open();

  std::string header1 = itr1->first();
  std::string header2 = itr2->first();

  // tokenize the header
  std::vector<std::string> v_header1 = tokenize_string( itr1->first(), "\t" );
  std::vector<std::string> v_header2 = tokenize_string( itr2->first(), "\t" );

  // get the columns that intercept
  std::vector<std::string> intercept = intercept_cols( v_header1, v_header2 );

  //cout << intercept.size() << " " << intercept[0] << endl;
  // check if there is index for the table 
  bool t1_has_index = t1->has_index( intercept );
  bool t2_has_index = t2->has_index( intercept );

  std::vector<std::string> v;
  v.push_back( join_headers ( header1, header2 ) );

  MyItr * to_return = new MyItr( std::string( "temp" ), v );

  if ( t1_has_index )
  {
    
    std::string t2_cur_string = itr2->get_next();
    while ( !itr2->is_last() )
    {
      //cout << t2_cur_string << endl;
      // get Btree key to find => intercept[0]

      // construct Tuple to find 
      std::string key = t2->get_entry( t2_cur_string, intercept[0] );
      std::string key_to_btree = intercept[0]; 

      if ( key_to_btree[0] == '*' )
        key_to_btree = key_to_btree.substr( 1, key_to_btree.size() );

      Tuple * tuple;
      //cout << key << endl;
      if ( is_number ( key ) )
        tuple = new Tuple( atoi( key.c_str() ) , 0 );
      else 
        tuple = new Tuple( key.c_str(), 0 );

      MyItr * temp_itr = t1->index_find( OP_EQUAL, key_to_btree, *tuple );
      print_result( to_return );
      if ( temp_itr != NULL )
        *to_return += *(temp_itr->nested_join( header2, t2_cur_string ));
      // call 
      delete( tuple );
      t2_cur_string = itr2->get_next();
    }
    // get the 
    
  }
  else if ( t2_has_index )
  {

  }
  else ;

  print_result( to_return );

  return itr1->nested_join( *itr2 );
  //exit(1);

}

MyItr * Executor::iterate(expr e)
{
  expression *ep = (expression *) e;

  MyItr * itr;

  if ( ep->values[0].ep->func == OP_ITERATE )
    itr = iterate( ep->values[0].ep );
  else if ( ep->values[0].ep->func == OP_TABLENAME )
  {
    //cout << col_name( ep->values[0].ep ) << endl;
    itr = get_itr( string ( col_name( ep->values[0].ep ) ) );
    if ( !itr )
    {
      cerr << "Table does not exist." << endl;
      return NULL;
    }
  }
  else if ( ep->values[0].ep->func == OP_INDEXSELECT )
    itr = indexselect( ep->values[0].ep );
  else if ( ep->values[0].ep->func == OP_PRODUCT)
    itr = product( ep->values[0].ep );
  else
  {
    cerr << "Executor::iterate() unknown lhs operation. " << endl;
    return NULL;
    //cout << "!" << col_name( ep->values[0].ep )  << endl;
  }

  if ( ep->values[1].ep->func == OP_SELECTROW )
  {
    return selectrow( itr, ep->values[1].ep );
  }
  else if ( ep->values[1].ep->func == OP_PROJECTROW )
  {
    return projectrow( itr, ep->values[1].ep );
  }
  else if ( ep->values[1].ep->func == OP_DELETEROW )
  {
    return deleterow( itr, ep->values[1].ep );
  }
  
  else
  {
    cerr << "Executor::iterate() unknown rhs operation. " << endl;
    return NULL;
  }


  return DO_NOT_PRINT;
}


void Executor::update_max_length( vector<string> reference, vector<int> &max_length_list )
{
  if ( reference.size() != max_length_list.size() )
  {
    cout << "Size not match in update_max_length() " << endl;
    return;
  }
//cout << reference.size() << endl;
  for( int i = 0; i < reference.size(); i++ )
  {
    if( reference[i].size() + 2 > max_length_list[i] )
      max_length_list[i] = reference[i].size() + 2;
  }
}

void Executor::print_result ( MyItr * itr )
{
  if( itr == DO_NOT_PRINT )
    return;

  vector<int> max_length;
  vector<string> header;
  vector< vector < string > > wanted_tuples;

  string cur_str = itr->first();
//cout << "!" << cur_str << endl;
  if( cur_str.compare( itr->EOI ) == 0 )
    return;
  char* s = toChars( cur_str );
  char* token = strtok( s, "\t" );
  while ( token )
  {
    header.push_back( string( token ) );
    max_length.push_back( strlen( token ) + 2 );
    token = strtok( NULL, "\t" );
  }

  cur_str = itr->get_next();
  while ( cur_str.compare( itr->EOI ) != 0 )
  {
    // construct the vector string
    vector<string> temp;
    char* s = toChars( cur_str );

    char* token = strtok( s, "\t" );
    while ( token )
    {
      temp.push_back( string( token ) );
      token = strtok( NULL, "\t" );
      //max_length.push_back( strlen( token ) + 2 );
    }

    update_max_length( temp, max_length );
    wanted_tuples.push_back ( temp );
    cur_str = itr->get_next();
  }

  print_result( header, max_length, wanted_tuples );

}



MyItr * Executor::deleterow( MyItr * itr, expression * ep )
{
  itr->open();
  string cur_str = itr->first();
  vector<streampos> qualified;

  Table * tbl = get_table( itr->filename );
  cur_str = itr->get_next();

  while ( cur_str.compare( itr->EOI ) != 0 )
  {

    if ( ep->values[1].ep->func == OP_AND )
    {
      if ( tbl->AND( ep->values[1].ep, cur_str ) );
        //tbl.deleterow( itr->get_cur_pos() );
    }
    else if  ( ep->values[1].ep->func == OP_OR ) 
    {
      if ( tbl->OR( ep->values[1].ep, cur_str ) );
        //tbl.deleterow( itr->get_cur_pos() );
    }
    else
    {
      if ( tbl->comparison( ep->values[1].ep, cur_str ) )
      {
        //cout << cur_str << endl;
        //cout << itr->get_cur_pos() << endl;
        string col; 
        if( ep->values[1].ep->values[0].ep->func == OP_COLNAME )
          col =  ep->values[1].ep->values[0].ep->values[0].name;
        else if (  ep->values[1].ep->values[1].ep->func == OP_COLNAME  )
          col =  ep->values[1].ep->values[1].ep->values[0].name;

        tbl->deleterow( itr->get_cur_pos(), cur_str, col );
      }
    }

    cur_str = itr->get_next();
  }

  return DO_NOT_PRINT;
}

void Executor::droptable( expression * ep )
{
  //cout << ep->values[0].ep->values[0].name << endl;
  string table_to_drop( ep->values[0].ep->values[0].name );
  Table * tbl = get_table( table_to_drop );

  if ( tbl == NULL )
  {
    cerr << "Table does not exist " << endl;
    return;
  }
  tbl->delete_associate();
  for ( vector<Table*>::iterator itr = tables.begin(); itr != tables.end(); itr++ )
    if ( (*itr)->name.compare( table_to_drop ) == 0 )
    {
      tables.erase( itr );
    }

}

MyItr * Executor::sort( expression * ep )
{
  //cout << ep->values[1].ep->values[0].ep->values[0].name << endl;

  string sortkey ( Rlist( ep->values[1].ep )[0] ); 
  MyItr * itr = iterate( ep->values[0].ep );
  itr->sort( sortkey );
  return itr;
  // /print_result( itr );

}

void Executor::execute(expr e, int lev)
{

  expression *ep = (expression *)e;
//cout << ep->func << endl;
  register int i, slev=lev;

  if(!ep) { return; }
  switch(ep->func) {

  case OP_COLUMNDEF:
      execute(ep->values[0].ep, lev+2);
      return;


  /* Relational operators */
  case OP_PROJECTION:break;
  case OP_SELECTION:break;
  case OP_PRODUCT:
      printf("Not yet implemented \n"); break;
  case OP_GROUP:
      printf("Not yet implemented \n"); break;
  case OP_DELTA:
      printf("Not yet implemented \n"); break;
  case OP_CREATETABLE:
      create_table( ep ); break;
  case OP_INSERTROW:
      insert( ep ); break;
  case OP_DROPTABLE:
      droptable( ep ); break;
  case OP_SORT:
      print_result( sort( ep ) ); break;


  case OP_ITERATE:
      print_result( iterate( ep ) );
      break;

  default:   break;
  }
}

void Executor::execute(expr e){

  execute(e, 0);

}