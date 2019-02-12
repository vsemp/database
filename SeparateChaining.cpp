        #include "SeparateChaining.h"


        using namespace std;
        typedef basic_string<char> string;
        /**
         * Internal method to test if a positive number is prime.
         * Not an efficient algorithm.
         */
        bool ChainingHashTable::isPrime( int n ) const
        {
            if( n == 2 || n == 3 )
                return true;

            if( n == 1 || n % 2 == 0 )
                return false;

            for( int i = 3; i * i <= n; i += 2 )
                if( n % i == 0 )
                    return false;

            return true;
        }

        /**
         * Internal method to return a prime number at least as large as n.
         * Assumes n > 0.
         */
        int ChainingHashTable::nextPrime( int n ) const
        {
            if( n % 2 == 0 )
                n++;

            for( ; !isPrime( n ); n += 2 )
                ;

            return n;
        }

        /**
         * Construct the hash table.
         */
        ChainingHashTable::ChainingHashTable( const Row & notFound, int size )
          : theLists( nextPrime( size ) ), ITEM_NOT_FOUND( notFound )
        {
        }

        /**
         * Insert item x into the hash table. If the item is
         * already present, then do nothing.
         */
        void ChainingHashTable::insert( const Row & x )
        {
            list<Row> & whichList = theLists[ hash( x, theLists.size( ) ) ];
            ListItr<Row> itr = whichList.find( x );

            if( itr.isPastEnd( ) )
                whichList.insert( x, whichList.zeroth( ) );
        }

        /**
         * Remove item x from the hash table.
         */
        void ChainingHashTable::remove( const Row & x )
        {
           theLists[ hash( x, theLists.size( ) ) ].remove( x );
        }

        /**
         * Find item x in the hash table.
         * Return the matching item or ITEM_NOT_FOUND if not found
         */
        const Row & ChainingHashTable::find( const Row & x ) const
        {
            ListItr<Row> itr;
            itr = theLists[ hash( x, theLists.size( ) ) ].find( x );
            return itr.isPastEnd( ) ? ITEM_NOT_FOUND : itr.retrieve( );
        }

        /**
         * Make the hash table logically empty.
         */
        void ChainingHashTable::makeEmpty( )
        {
            for( int i = 0; i < theLists.size( ); i++ )
                theLists[ i ].makeEmpty( );
        }

        /**
         * Deep copy.
         */
        const ChainingHashTable &
        ChainingHashTable::operator=( const ChainingHashTable<Row> & rhs )
        {
            if( this != &rhs )
                theLists = rhs.theLists;
            return *this;
        }

        
        /**
         * A hash routine for string objects.
         */
        int ChainingHashTable::hash( const Row & key, int tableSize) const
        {
            string to_hash = key.key;
            int hashVal = 0;

            for( int i = 0; i < to_hash.length( ); i++ )
                hashVal = 37 * hashVal + to_hash[ i ];

            hashVal %= tableSize;
            if( hashVal < 0 )
                hashVal += tableSize;

            return hashVal;
        }

        bool Row::operator == ( const Row rhs  )const
        {
            return tuple.compare( rhs.tuple ) == 0;
        }