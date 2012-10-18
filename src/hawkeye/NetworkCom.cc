#include "NetworkCom.hh"
#include <iostream>
//Added by qt3to4:
#include <Q3TextStream>
#include <QLabel>

using namespace std;


/*
  The ClientSocket class provides a socket that is connected with a client.
  For every client that connects to the server, the server creates a new
  instance of this class.
*/
ClientSocket::ClientSocket( int sock, QObject *parent, const char *name ) :
	Q3Socket( parent, name )
{
  connect(this, SIGNAL(readyRead()),        SLOT(readClient()));
  connect(this, SIGNAL(connectionClosed()), SLOT(deleteLater()));
  setSocket(sock);
}

ClientSocket::~ClientSocket()
{
}

void ClientSocket::readClient()
{
  Q3TextStream ts( this );
  while ( canReadLine() ) 
  {
    QString status;
    QString str;

    ts >> str;

    int err = 1;

    if (str == "contig")
    {
      int pos;
      ts >> str >> pos;
      emit setContigLocation(str, pos);
      status = "contig: " + str + ":" + QString::number(pos) + "\n";
      err = 0;
    }
    else if (str == "kill")
    {
      ts >> str;

      if (str == "flanders")
      {
        emit logText("Kill Flanders!");
      }
    }

    if (err)
    {
      status = "unknown command\n";
    }

    str = ts.readLine();

    ts << status;
  }
}

/*
  The SimpleServer class handles new connections to the server. For every
  client that connects, it creates a new ClientSocket -- that instance is now
  responsible for the communication with that client.
*/
SimpleServer:: SimpleServer( QObject* parent, int port ) 
  : Q3ServerSocket( port, 1, parent )
{
  if ( !ok() ) 
  {
    qWarning("Failed to bind to port");
    exit(1);
  }
}

SimpleServer::~SimpleServer()
{
}

void SimpleServer::newConnection( int socket )
{
  ClientSocket *s = new ClientSocket( socket, this );
  emit newConnect( s );
}


ServerInfo::ServerInfo()
{
  SimpleServer *server = new SimpleServer( this );

  QString itext = tr(
      "This is a small server example.\n"
      "Connect with the client now."
      );
  QLabel *lb = new QLabel( itext, this );
  lb->setAlignment( Qt::AlignHCenter );
  infoText = new Q3TextView( this );
  QPushButton *quit = new QPushButton( tr("Quit") , this );

  connect( server, SIGNAL(newConnect(ClientSocket*)), SLOT(newConnect(ClientSocket*)) );
  connect( quit, SIGNAL(clicked()), qApp, SLOT(quit()) );
}

ServerInfo::~ServerInfo()
{
}

void ServerInfo::newConnect( ClientSocket *s )
{
  infoText->append( tr("New connection\n") );
  connect( s, SIGNAL(logText(const QString&)), infoText, SLOT(append(const QString&)) );
  connect( s, SIGNAL(connectionClosed()),                SLOT(connectionClosed()) );
}

void ServerInfo::connectionClosed()
{
  infoText->append( tr("Client closed connection\n") );
}

