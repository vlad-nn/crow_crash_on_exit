//
// composed_2.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/use_future.hpp>
#include <asio/write.hpp>
#include <cstring>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

using asio::ip::tcp;

//------------------------------------------------------------------------------

// This next simplest example of a composed asynchronous operation involves
// repackaging multiple operations but choosing to invoke just one of them. All
// of these underlying operations have the same completion signature. The
// asynchronous operation requirements are met by delegating responsibility to
// the underlying operations.

template <typename CompletionToken>
auto async_write_message(tcp::socket& socket,
    const char* message, bool allow_partial_write,
    CompletionToken&& token)
  // The return type of the initiating function is deduced from the combination
  // of CompletionToken type and the completion handler's signature. When the
  // completion token is a simple callback, the return type is void. However,
  // when the completion token is asio::yield_context (used for stackful
  // coroutines) the return type would be std::size_t, and when the completion
  // token is asio::use_future it would be std::future<std::size_t>.
  //
  // In C++14 we can omit the return type as it is automatically deduced from
  // the return type of our underlying asynchronous operation
{
  // As the return type of the initiating function is deduced solely from the
  // CompletionToken and completion signature, we know that two different
  // asynchronous operations having the same completion signature will produce
  // the same return type, when passed the same CompletionToken. This allows us
  // to trivially delegate to alternate implementations.
  if (allow_partial_write)
  {
    // When delegating to an underlying operation we must take care to
    // perfectly forward the completion token. This ensures that our operation
    // works correctly with move-only function objects as callbacks, as well as
    // other completion token types.
    return socket.async_write_some(
        asio::buffer(message, std::strlen(message)),
        std::forward<CompletionToken>(token));
  }
  else
  {
    // As above, we must perfectly forward the completion token when calling
    // the alternate underlying operation.
    return asio::async_write(socket,
        asio::buffer(message, std::strlen(message)),
        std::forward<CompletionToken>(token));
  }
}

//------------------------------------------------------------------------------

void test_callback()
{
  asio::io_context io_context;

  tcp::acceptor acceptor(io_context, {tcp::v4(), 55555});
  tcp::socket socket = acceptor.accept();

  // Test our asynchronous operation using a lambda as a callback.
  async_write_message(socket, "Testing callback\r\n", false,
      [](const std::error_code& error, std::size_t n)
      {
        if (!error)
        {
          std::cout << n << " bytes transferred\n";
        }
        else
        {
          std::cout << "Error: " << error.message() << "\n";
        }
      });

  io_context.run();
}

//------------------------------------------------------------------------------

void test_future()
{
  asio::io_context io_context;

  tcp::acceptor acceptor(io_context, {tcp::v4(), 55555});
  tcp::socket socket = acceptor.accept();

  // Test our asynchronous operation using the use_future completion token.
  // This token causes the operation's initiating function to return a future,
  // which may be used to synchronously wait for the result of the operation.
  std::future<std::size_t> f = async_write_message(
      socket, "Testing future\r\n", false, asio::use_future);

  io_context.run();

  try
  {
    // Get the result of the operation.
    std::size_t n = f.get();
    std::cout << n << " bytes transferred\n";
  }
  catch (const std::exception& e)
  {
    std::cout << "Error: " << e.what() << "\n";
  }
}

//------------------------------------------------------------------------------

int main()
{
  test_callback();
  test_future();
}
