#include<iostream>
#include <fmt/format.h>
#include<HappyHTTP/happyhttp.h>

static int count=0;

// invoked when response headers have been received
void OnBegin( const happyhttp::Response* r, void* userdata )
{
    std::cout<<"BEGIN" << r->getstatus() << r->getreason() << endl;
    count = 0;
}

// invoked to process response body data (may be called multiple times)
void OnData( const happyhttp::Response* r, void* userdata, const unsigned char* data, int n )
{
    fwrite( data,1,n, stdout );
    count += n;
}

// invoked when response is complete
void OnComplete( const happyhttp::Response* r, void* userdata )
{
    printf( "COMPLETE (%d bytes)\n", count );
}


void TestGET()
{
    happyhttp::Connection conn( "www.scumways.com", 80 );
    conn.setcallbacks( OnBegin, OnData, OnComplete, 0 );

    conn.request( "GET", "/happyhttp/test.php" );

    while( conn.outstanding() )
        conn.pump();
}