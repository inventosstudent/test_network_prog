#!/usr/bin/perl

use IO::Socket;
use IO::Select;
use POSIX;
use Fcntl;

##
##  Простой тестер сетевого POP3 демона - открывает nconnection соединений и ждет
##

unless ($ARGV[0] && $ARGV[1] && $ARGV[2]) {
    die "Usage: ./test.pl addr port nconnection";
}


my $select = IO::Select->new();

for(my $x=0;$x<$ARGV[2];$x++)
{

    my $sock=IO::Socket::INET->new(
        PeerAddr	=>	$ARGV[0],
        PeerPort	=>	$ARGV[1],
        Type     => SOCK_STREAM,
    )||die;


    nonblock($sock);
    print STDERR "Connected client #$x\n" ;



    $select->add($sock);
}


print STDERR "Doing something with this connections\n" ;


while($select->count()>0){
    foreach my $socket ($select->can_read(0.1)) {
        print STDERR "Here\n";
        my $data = '';
        my $rv   = undef;
        eval {
            $rv=$socket->recv($data, POSIX::BUFSIZ, 0);
        };
#        if ($@){$rv=undef;}

        unless (defined($rv) && length $data) {
            warn "Can't read on server socket";
            $select->remove($socket);
            close($socket);
            last;
        }

        warn "recv ".(length $data)." bytes";

    }

    foreach my $socket ($select->can_write(0.1)) {
        print STDERR "Here2\n";

        my $rv;
        eval {
           $rv = $socket->send("GET / HTTP/1.0\r\n\r\n", 0);
        };

        if ($@ || !defined $rv){
            warn "UNABLE to write";
            $socket->close();
            $select->remove($socket);
            last;
        }
    }
}

print STDERR "Done\n";


# nonblock($socket) puts socket into nonblocking mode
sub nonblock {
    my $socket = shift;
    my $flags;

    
    $flags = fcntl($socket, F_GETFL, 0)
            or die "Can't get flags for socket: $!\n";
    fcntl($socket, F_SETFL, $flags | O_NONBLOCK)
            or die "Can't make socket nonblocking: $!\n";
}

