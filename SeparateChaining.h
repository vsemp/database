        #ifndef _SEPARATE_CHAINING_H_
        #define _SEPARATE_CHAINING_H_

        #include <vector>
        #include <string>
        #include <vector>
        #include <list>

        // SeparateChaining Hash table class
        //
        // CONSTRUCTION: an initialization for ITEM_NOT_FOUND
        //               and an approximate initial size or default of 101
        //
        // ******************PUBLIC OPERATIONS*********************
        // void insert( x )       --> Insert x
        // void remove( x )       --> Remove x
        // Hashable find( x )     --> Return item that matches x
        // void makeEmpty( )      --> Remove all items
        // int hash( string str, int tableSize )
        //                        --> Global method to hash strings

        struct Row
        {
            std::string key;
            std::string tuple; 

            bool operator == ( const Row rhs  )const;

        };

        class ChainingHashTable
        {
          public:
            explicit ChainingHashTable( const Row & notFound, int size = 101 );
            ChainingHashTable( const ChainingHashTable & rhs )
              : ITEM_NOT_FOUND( rhs.ITEM_NOT_FOUND ), theLists( rhs.theLists ) { }

            const Row & find( const Row & x ) const;

            void makeEmpty( );
            void insert( const Row & x );
            void remove( const Row & x );

            const ChainingHashTable & operator=( const ChainingHashTable & rhs );
          private:
            std::vector<std::list<Row> > theLists;   // The array of Lists
            const Row ITEM_NOT_FOUND;
            int hash( const Row & key, int tableSize ) const;

            bool isPrime( int n ) const;
            int nextPrime( int n ) const;
        };



        #include "SeparateChaining.cpp"
        #endif
