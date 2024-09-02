<h1 align="center">
  <br>
  HTTP Server in C
</h1>

<h4 align="center">A minimalistic HTTP server implementation written in C.</h4>

<p align="center">
  <a href="#key-features">Key Features</a> •
  <a href="#how-to-use">How To Use</a> •
  <a href="#license">License</a>
</p>

## Key Features

* 🚀 **Supports Multiple HTTP Versions**: Choose between HTTP/0.9, HTTP/1.0, and HTTP/1.1 by specifying the desired version
  when starting the server.
* 📄 **Handles GET Requests**: The server processes GET requests and returns the content of the requested file.
* 📂 **Static File Serving**: Serves files located in the `www/` directory, using efficient file handling with `sendfile`.
* ⚠️ **Error Handling**: Provides appropriate error messages and responses for invalid requests or missing files.
* 🛠️ **Simple Setup**: Minimal dependencies and easy setup for running and testing the server locally

## How To Use

To clone and run this application, you'll need [Git](https://git-scm.com) and [CMake](https://cmake.org/)
installed on your computer. From your command line:

```bash
# Clone this repository
$ git clone https://github.com/alessandrofoglia07/minimalist-http-server

# Go into the repository
$ cd minimalist-http-server

# Build the app
$ cmake --build ./cmake-build-debug --target http_server -- -j 6

# Run the app
$ ./http_server
```

<img src="https://cloud-b2znb881c-hack-club-bot.vercel.app/0image.png"></img>

## License

MIT

---

> GitHub [@alessandrofoglia07](https://github.com/alessandrofoglia07) &nbsp;&middot;&nbsp;
> StackOverflow [@Alexxino](https://stackoverflow.com/users/21306952/alexxino) &nbsp;&middot;&nbsp;
> NPM [alessandrofoglia07](https://www.npmjs.com/~alessandrofoglia07)
