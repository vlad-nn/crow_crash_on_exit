#include "crow_all.h"
#include <iostream>

int main()
{
	{
		crow::SimpleApp app;

		CROW_ROUTE( app, "/hello" )
		( []() {
			return "Hello World!";
		} );

		auto server_future = app.port( 18080 ).multithreaded().run_async();
		app.wait_for_server_start();

		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

		app.stop();           // stop server
		server_future.get();  // throw exceptions if any
	}

	std::cout << "You should see this message when everything works OK.\n";
	return 0;
}
