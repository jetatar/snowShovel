#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <termios.h>


long long getTimeMs() {
   return static_cast<long long>(time(0)) * 1000000ll;
}

template<typename T>
int readNextBytes( const int fid, T* bytes, const unsigned int n,
                   const unsigned int msTimeout=1000,
                   const bool showTimeout=true) {
   // try to read 'n' elements of type T into the array 'bytes'
   // stop after msTimeout miliseconds
   //
   // return 1 on timeout
   // return -1 * number of bytes on actual read
   
   int rd=0;
   const long long startMs = getTimeMs();
   while (true) {
      rd = read(fid, bytes, n*sizeof(T));
      if (rd<1) {
         if ((getTimeMs() - startMs)>msTimeout) {
            if (showTimeout) {
	      fprintf(stderr,"timing out\n");
            }
            return 1; // timeout => return val > 0
         }
      } else {
         // read something
         return -1*rd;
      }
   }
}


template<typename T>
int sendData(const int fid, const T* data, const unsigned int n) {
   // send stuff to the port
   // n = number of elements in 'data' array
   // returns number of bytes written
   
   const ssize_t nbytes = n*sizeof(T);
   ssize_t rd = write(fid, data, nbytes);
   if (rd != nbytes) {
     printf("sendData<%s>: Sent only %u bytes to id %d.\n",
            typeid(T).name(), (UInt_t)rd, fid);
   }
   return rd;
}

void mbedMsgTo(const int fid, FILE* strm=stderr, const int endTimeout=20) {
   char ccc=0;
   time_t strt = time(0);
   while (1) {//ccc!='/') {
      if ( (read(fid, &ccc, 1)) > 0 ) {
         if (ccc==0) ccc='0';
         fprintf(strm,"%c",ccc);
      }
      if ( (time(0) - strt) > endTimeout ) break; // don't wait forever
   }
   fprintf(stderr,"\n");
}

int openDevice() {
   //fprintf( stderr, "openDevice()\n" );

   struct termios tio;

   memset( &tio, 0, sizeof(tio) );
   tio.c_iflag             = 0;
   tio.c_oflag             = 0;
   tio.c_cflag             = CS8|CREAD|CLOCAL;     // 8 bit no parity
   /*
   tio.c_lflag             = 0;
   tio.c_cc[VMIN]          = 1;    // One byte enough to return from read
   tio.c_cc[VTIME]         = 0;    // Inter-character timer


   fd = open( "/dev/ttyACM0", O_RDWR | O_NONBLOCK );
   */
   // allow read to wait up to 1 second for a byte
   tio.c_lflag            &= ~ICANON;
   tio.c_cc[VMIN]          = 0;
   tio.c_cc[VTIME]         = 10; // in tenths of a second
   int fd = open( "/dev/ttyACM0", O_RDWR | O_NOCTTY );

   if ( fd < 0 ) {
      printf("Failed to open usb device: %s\n", strerror(-1*fd) );
      return -1;
   }

   //cfsetospeed( &tio, B921600 ); // max for USB2.0?
   //cfsetispeed( &tio, B921600 );
   //cfsetospeed( &tio, B460800 );
   //cfsetispeed( &tio, B460800 );
   cfsetospeed( &tio, B230400 );
   cfsetispeed( &tio, B230400 );
   //cfsetospeed( &tio, B115200 );
   //cfsetispeed( &tio, B115200 );

   tcsetattr( fd, TCSANOW, &tio );

   fd_set readfd;
   fd_set writefd;

   FD_ZERO( &readfd );
   FD_SET( fd, &readfd );
   FD_ZERO( &writefd );
   FD_SET( fd, &writefd );

   tcflush(fd, TCIOFLUSH);

   return fd;
}
