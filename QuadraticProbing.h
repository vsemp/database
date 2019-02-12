        #ifndef _QUADRATIC_PROBING_H_
        #define _QUADRATIC_PROBING_H_

        #include <vector>
        #include <string>
        #include <list>

        // QuadraticProbing Hash table class
        //
        // CONSTRUCTION: an initialization for ITEM_NOT_FOUND
        //               and an approximate initial size or default of 101
        //
        // ******************PUBLIC OPERATIONS*********************
        // void insert( x )       --> Insert x
        // void remove( x )       --> Remove x
        // Hashable find( x )     --> Return item that matches x
        // void makeEmpty( )      --> Remove all items
        // int hash( String str, int tableSize )
        //                        --> Static method to hash strings

        struct Row
        {
            std::string key;
            std::string tuple; 

            bool operator == ( const Row rhs  )const;
            bool operator != ( const Row rhs  )const;

        };

        class QuadraticHashTable
        {
          public:
            explicit QuadraticHashTable( const std::list<Row> & notFound, int size = 101 );
            QuadraticHashTable( const QuadraticHashTable & rhs )
              : array( rhs.array), ITEM_NOT_FOUND( rhs.ITEM_NOT_FOUND ),
                currentSize( rhs.currentSize ) { }

            const std::list<Row> & find( const Row & x ) const;

            void makeEmpty( );
            void insert( const Row & x );
            void insert( std::list<Row> list );
            void remove( const Row & x );

            const QuadraticHashTable & operator=( const QuadraticHashTable & rhs );

            enum EntryType { ACTIVE, EMPTY, DELETED };
          private:
            struct HashEntry
            {
                std::list<Row> theList;
                //Row element;
                EntryType info;

                //HashEntry( const Row & e = Row( ), EntryType i = EMPTY )
                  //: element( e ), info( i ) { }
                HashEntry(){ info = EMPTY;}
                HashEntry( const Row & e, EntryType i)
                {
                    theList.push_back( e );
                    info = i;
                }
                // for rehashing
                HashEntry( const std::list<Row> & rows, EntryType i)
                {
                    theList = rows;
                    info = i;
                }
                /*
                void delete( const Row & e )
                {
                    bool to_continue = true;
                    while( to_continue ) 
                    {
                        std::list<Row>::iterator itr;
                        for ( itr = theList.begin(), itr != theList.end(); itr++ )
                        {
                            if ( (*itr).theSame( e ) )
                            {
                                theList.remove( itr );
                                break;
                            }
                        }
                        if ( itr == theList.end() )
                            to_continue = false;
                    }
                }
                */
            };

            std::vector<HashEntry> array;
            int currentSize;
            const std::list<Row> ITEM_NOT_FOUND;
            bool isPrime( int n ) const;
            int nextPrime( int n ) const;
            bool isActive( int currentPos ) const;
            int findPos( const Row & x ) const;
            int hash( const Row & key, int tableSize ) const;
            int hash( int key, int tableSize ) const;
            void rehash( );
        };

        //#include "QuadraticProbing.cpp"
        #endif
