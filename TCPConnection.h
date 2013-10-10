#pragma once

#include <string>
#include <iostream>

#include <SDL2/SDL_net.h>

class TCPConnection
{
public:
	bool Init( const std::string &host, unsigned short port, bool server )
	{
		isConnected = false;

		isServer = server;
		hostName = host;
		portNr = port;
		std::cout << "Initialising" << std::endl;
		socketSet = SDLNet_AllocSocketSet( 1 );

		if ( !ResolveHost() )
			return false;

		if ( !OpenConnectionToHost() )
			return false;

		isConnected = true;
		return true;
	}
	bool ResolveHost()
	{
		int success = 0;

		if ( isServer )
			success = SDLNet_ResolveHost( &ipAddress, nullptr, portNr );
		else
			success = SDLNet_ResolveHost( &ipAddress, hostName.c_str(), portNr );

		if ( success == -1 )
		{
			std::cout << "Failed to open port host :"
				"\nIP Adress : " << hostName <<
				"\nPort : " << portNr <<
				"\nError : " << SDLNet_GetError() <<
				"\nLine : " << __LINE__
				<< std::endl;
			return false;
		}

		return true;
	}

	bool OpenConnectionToHost( )
	{
		tcpSocket = SDLNet_TCP_Open( &ipAddress );

		if ( !isServer )
			SDLNet_TCP_AddSocket( socketSet, tcpSocket );

		if ( tcpSocket == nullptr )
		{
			std::cout << "Failed to open port host :"
				"\nIP Adress : " << hostName <<
				"\nPort : " << portNr <<
				"\nError : " << SDLNet_GetError() <<
				"\nLine : " << __LINE__
				<< std::endl;
			return false;

		}
		return true;
	}

		
	void Send( std::string str )
	{
		if ( !isConnected  )
		{
			std::cout << "Error! Not connected " << std::endl;
			return;
		}
		
		void* messageData = ConvertStringToVoidPtr(str);
		int messageSize = static_cast< int > ( str.length() );
		int bytesSent = 0;

		if ( isServer )
		{
			bytesSent = SDLNet_TCP_Send( serverSocket,  messageData,  messageSize);
		}
		else
		{
			bytesSent = SDLNet_TCP_Send( tcpSocket,  messageData,  messageSize);
		}

		if ( bytesSent < messageSize )
		{
			std::cout << "Send failed : " << SDLNet_GetError() << std::endl;
			isConnected = false;
		}

	}
	void Close()
	{
		if ( isConnected )
			SDLNet_TCP_Close( tcpSocket );

	}
	bool StartServer( )
	{
		if (!isServer )
		{
			std::cout << "Not in server mode, can't start server" << std::endl;
			return false;
		}

		bool quit = false;
		while ( !quit )
		{
			if ( AcceptConnection() )
			{
				if ( GetPeerAddress() )
				{
					SDLNet_TCP_AddSocket( socketSet, serverSocket );
					quit = true;
					isConnected = true;
					return true;
				}
			}
			SDL_Delay( 20 );
		}
		return false;
	}

	bool AcceptConnection()
	{
		serverSocket = SDLNet_TCP_Accept( tcpSocket );

		if ( serverSocket  == nullptr ) 
		{
			std::cout << "Cannot accept TCP connection : " << SDLNet_GetError()  << std::endl;
			isConnected = false;	
			return false;
		}

		return true;
	}
	bool GetPeerAddress()
	{
		IPaddress* ipRemote = SDLNet_TCP_GetPeerAddress( serverSocket );

		if ( ipRemote == nullptr ) 
		{
			std::cout << "Failed to get peer addres : " << hostName << " : " << portNr << std::endl;
			std::cout << "\tServer : " << std::boolalpha << isServer << std::endl;
			isConnected = false;	
			return false;
		}

		std::cout << "Host connected : " << SDLNet_Read32( &ipRemote->host ) << " : " << SDLNet_Read32( &ipRemote->host ) << std::endl;

		isConnected = true;	
		return true;
	}
	std::string ReadMessages()
	{
		if ( !CheckForActivity() )
			return "";

		char buffer[512];
		memset( buffer, 0, 512 );

		int byteCount  = 0;
		std::string received("");

		if ( !isConnected )
			return received;

		if ( isServer )
			byteCount = SDLNet_TCP_Recv( serverSocket, buffer, 512 );
		else
			byteCount = SDLNet_TCP_Recv( tcpSocket, buffer, 512 );

		if ( byteCount > 0 )
		{
			//memset( &buffer[byteCount], 0, static_cast< size_t > (512 - byteCount ) );
			buffer[byteCount] = '\0';
			received = buffer;

		// A bytecount of 0 means the connection has been terminated
		} else if ( byteCount == 0 )
		{
			std::cout << "Connection terminated" << std::endl;
			isConnected = false;
		// A bytecount of < 0 means an error occured
		} else if ( byteCount < 0 )
		{
			std::cout << "Read failed!" <<
				"\nSocket : " << ( isServer ? serverSocket : tcpSocket )  << 
				"\nByte count : " << byteCount << 
				"\nERrror : " << SDLNet_GetError() << 
				std::endl;
		}

		return received;
	}
	bool CheckForActivity()
	{
		int countReady = SDLNet_CheckSockets( socketSet, 0 );

		if ( countReady < 0 )
		{
			std::cout << "Error! " << SDLNet_GetError() << std::endl;
			return false;
		}

		if ( isServer )
			return SDLNet_SocketReady( serverSocket ) != 0;
		else 
			return SDLNet_SocketReady( tcpSocket ) != 0;
	}
	bool IsConnected()
	{
		return isConnected;
	}

private:
	void* ConvertStringToVoidPtr( const std::string &str )
	{
		const char* charPtr = str.c_str();

		return const_cast< char* > ( charPtr );
	}
	bool isServer;
	std::string hostName;
	unsigned short portNr;
	bool isConnected;
	IPaddress ipAddress;

	TCPsocket tcpSocket;
	TCPsocket serverSocket;
	SDLNet_SocketSet socketSet;
};