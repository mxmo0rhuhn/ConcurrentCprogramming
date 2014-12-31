# Seminar Concurrent C programming
Term paper for the Zurich university of applied sciences seminar in Concurrent C programming.

Implementation of a virtual (in memory) Fileserver that can be accessed via TCP/IP and supports the following commands:
- CREATE
- READ
- UPDATE
- DELETE
- LIST

The implementation is optimized for concurrent multi client interaction. A client is also provided.

## Make 
The following make targets exist: 

* all: creates the most relevant binaries of the term paper: `run` and `test`
* run: creates only the server: `run`
* test: creates only the module test binary: `test`
* client: creates an interactive client for manual tests of the server: `client`

## Usage
### Server
The `run` binary is the server for the application.
```
$ ./ run -h
Help: 

Usage:
./run  [-p Port] [-d Out] [-i Out] [-e Out]

Server for the term paper in concurrent C programming
Will start a virtual file server that accepts connections via
TCP


[-p Port] Optional: Tries to connect to a server on the given port.
           Default: 7000

[-d Loglevel] Optional: Alter the output for DEBUG messages.
               Default: No logging

[-i Loglevel] Optional: Alter the output for INFO messages.
               Default: stdout

[-i Loglevel] Optional: Alter the output for ERROR messages.
               Default: stderr 

 Possible log Outputs: 0 = No logging
                       1 = Logfile
                       2 = stdout
                       3 = stderr


(c) Max Schrimpf - ZHAW 2014
```

### Client
```
$ ./ client -h
Help:

Usage:
./client [-c Line1] [-C Line2] [-a IP] [-p Port] [-d Out] [-i Out] [-e Out]

Connects to a server and provides an interactive multiline command
interface. You can simply start a new line with \n (Enter)
The command will be send to the server as soon as a empty line is
entered. The interactive interface can be ended using <Ctrl-C> 
or by entering QUIT

You may also provide a command that is executed without an interactive
command interface using the [-c Line] option
If you want to specify a second line that is send to the server after
-c you can use -C 


[-c Line1] Optional: A command that should be send to the server.
                     This option disables the interactive mode

[-C Line2] Optional: A second line for the command - only possible
                     if a first line is provided

[-a IP] Optional: Tries to connect to the server on the given IP.
         Default: 127.0.0.1

[-p Port] Optional: Tries to connect to a server on the given port.
           Default: 7000

[-d Loglevel] Optional: Alter the output for DEBUG messages.
               Default: No logging

[-i Loglevel] Optional: Alter the output for INFO messages.
               Default: stdout

[-i Loglevel] Optional: Alter the output for ERROR messages.
               Default: stderr 

 Possible log Outputs: 0 = No logging
                       1 = Logfile
                       2 = stdout
                       3 = stderr


(c) Max Schrimpf - ZHAW 2014

```

### Test
There are several automated tests available to ensure the full functionality of the server.
```
$ ./ test -h
Help:

Usage:
./test  [-a IP] [-p Port] [-d Out] [-i Out] [-e Out]

Executes various tests on the fileserver
A running server at the given address is needed


[-a IP] Optional: Tries to connect to the server on the given IP.
         Default: 127.0.0.1

[-p Port] Optional: Tries to connect to a server on the given port.
           Default: 7000

[-d Loglevel] Optional: Alter the output for DEBUG messages.
               Default: No logging

[-i Loglevel] Optional: Alter the output for INFO messages.
               Default: stdout

[-i Loglevel] Optional: Alter the output for ERROR messages.
               Default: stderr 

 Possible log Outputs: 0 = No logging
                       1 = Logfile
                       2 = stdout
                       3 = stderr


(c) Max Schrimpf - ZHAW 2014
```

## License
This term paper is free software: You can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

The file is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
