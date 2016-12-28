README v0.0 / DECEMBER 2016

# TopicServer

## Introduction

A simple client - server demonstration of classic topic server with unlimited number of subscribers, as well as publishers. Read [**this**](https://en.wikipedia.org/wiki/Publish%E2%80%93subscribe_pattern) to get familiar with this topic.  
This server forwards publishers messages for given topic to subscribed clients. It does that by implementing **hash map** that has **keys** with _topic names_, and **values** as _list_ of subscribed users. It does not store messages anywhere during that proccess.

## Usage

###Starting the server

Server listens for connections bouth from subscribers and from publishers. It _distinguishes_ subscribers from publisher _over the port_ they used to connect to server. That being said, ports **need to be provided** to server when starting it, because there are no default ports. This is done using -s and -p flag for subscribers, publishers respectively. If you don't provide right flags you will see error codes:  
* 1 - Number of arguments is not valid.
* 2 - Flags are not correctly set.
* 3 - Invalid flag is used.

Server lets ports to be initalised from full range 0 - 65535, but if they are reserved, OS will give error message (usually about permissions). It is not recommended to mess with that, instead change port to other (higher are more likely to work).

Here is an example of starting the server from the top directory:

>./out/server -p 30000 -s 27015

Server will try to create socket for publisher at port 30000 and port for subscribers at 27015. If successfull it will print info about that, and start listening for connections.

This is the output for this example, when started:

>Port for publishers: 30000  
>Port for subscribers: 27015  
>To shutdown server type: quit/QUIT  
>Socket for publishers created!  
>Socket for subscribers created!  

####Server Behaviour
Server is not interactive, except when command quit/QUIT is entered. It only provides information on new connections or when someone disconnects.  
Command quit/QUIT is self-explanatory and tells the server to go down.

####Starting the 

## Contributing

For now, only one publisher should publish news, because every time one disconnects, list of subscribers for that topic is erased and the topic is removed from the hash map. If two publishers provide info for the same topic, and one disconnects, list of subscribers is erased and topic removed although second is still active.  
This happens because topics are not connected to publishers ip's. One of the ways to this is to implement another hashmap that also connects topics for list of publishers and keeps topics active as long as that list is not empty.  
This could be patched.

## Help

For any questions, bugs, or development you can contact us:  
* stevan.stevic4343@gmail.com
* marko.godra@gmail.com

## Installation

### Requirements

You will need...
* *gcc* - C/C++ compiler.
* [glib2](https://developer.gnome.org/glib/) - Library that provides the core application building blocks for libraries and applications written in C.
* clang-format-3.8 - Application that formats source code (optional)
* Unix-like, Debian-based host (the implementation was tested on Ubuntu14.04)

Or, you can just type:  
>make install

### Installation

You should postion yourself in desired directory. For example, if you want to be downloaded in Home directory simply type:  
>cd ~

Next step is to get the program:
>git clone https://github.com/stevanStevic/TopicServer  
>cd TopicServer

Now, you need to install applications and programs from **Requirements** section or just type:  
>make install  
>make  

to install and compile programs.

That's it now you are ready!

### Configuration

No further configuration is needed.

## Credits

Project atuhors are:
* Marko Dragojevic
* Stevan Stevic

It is done as part of out final exam for Fundamentals of _Computer Networks 1_ course.

## License

It uses GPL-3.0
