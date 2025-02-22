/*
 * ProviewR   Open Source Process Control.
 * Copyright (C) 2005-2025 SSAB EMEA AB.
 *
 * This file is part of ProviewR.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ProviewR. If not, see <http://www.gnu.org/licenses/>
 *
 * Linking ProviewR statically or dynamically with other modules is
 * making a combined work based on ProviewR. Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the copyright holders of
 * ProviewR give you permission to, from the build function in the
 * ProviewR Configurator, combine ProviewR with modules generated by the
 * ProviewR PLC Editor to a PLC program, regardless of the license
 * terms of these modules. You may copy and distribute the resulting
 * combined work under the terms of your choice, provided that every
 * copy of the combined work is accompanied by a complete copy of
 * the source code of ProviewR (the version used to produce the
 * combined work), being distributed under the terms of the GNU
 * General Public License plus this exception.
 */

#include <time.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#if defined OS_LINUX
#include <termio.h>
#endif
#if defined OS_LINUX || defined OS_MACOS
#include <sgtty.h>
#endif
#include <sys/ioctl.h>
#include "remote_utils.h"

/************************************************************************
**************************************************************************
*
* Namn : RemUtils_InitSerialDev
*
* Typ  : int
*
* Typ           Parameter              IOGF     Beskrivning
*
* Beskrivning : Assigns and sets up serial device
*
**************************************************************************
**************************************************************************/

int RemUtils_InitSerialDev(
    char* device, int speed, int databits, int stopbits, int parity)
{
#define SER_NONE 0
#define SER_ODD 1
#define SER_EVEN 2

  struct termios tty_attributes;
  int fd;
  int sts;
  int l_parity = SER_NONE;

  fd = open(device, O_RDWR | O_NDELAY | O_NOCTTY); /*�ppna seriedevicet*/

  if (fd == -1)
    return 0;

  tcgetattr(fd, &tty_attributes); // h�mta serieportsv�rdena
  tty_attributes.c_cc[VMIN] = 1;
  tty_attributes.c_cc[VTIME] = 0;
  tty_attributes.c_lflag &= ~(ICANON | ISIG | ECHO | IEXTEN);
  tty_attributes.c_cflag |= (CLOCAL | CREAD);
  tty_attributes.c_oflag &= ~(OPOST);
  tty_attributes.c_oflag &= ~(ONLCR);
  tty_attributes.c_iflag &= ~(INLCR | ICRNL);

  /*
    speed = rn_serial->Speed;
    bits = rn_serial->DataBits;
    stopbit = rn_serial->StopBits;
  */

  if (parity == 1)
    l_parity = SER_ODD;
  if (parity == 2)
    l_parity = SER_EVEN;

/* hastighetsval */

#if defined OS_LINUX
  tty_attributes.c_cflag &= ~CBAUD; // maska bort all hastighet
#endif
  switch (speed) {
  case 300:
    tty_attributes.c_cflag |= B300;
    break;
  case 1200:
    tty_attributes.c_cflag |= B1200;
    break;
  case 2400:
    tty_attributes.c_cflag |= B2400;
    break;
  case 4800:
    tty_attributes.c_cflag |= B4800;
    break;
  case 9600:
    tty_attributes.c_cflag |= B9600;
    break;
  case 19200:
    tty_attributes.c_cflag |= B19200;
    break;
  case 38400:
    tty_attributes.c_cflag |= B38400;
    break;
  case 57600:
    tty_attributes.c_cflag |= B57600;
    break;
  case 115200:
    tty_attributes.c_cflag |= B115200;
    break;
  case 230400:
    tty_attributes.c_cflag |= B230400;
    break;
  default:
    tty_attributes.c_cflag |= B9600;
    break;
  }

  /* s�tt bitl�ngd p� porten, 5, 6, 7 eller 8 bitar */

  tty_attributes.c_cflag &= ~CSIZE; // bytesize maska bort alla bitl�ngdsval
  if (databits == 5)
    tty_attributes.c_cflag |= CS5; // 5bit
  else if (databits == 6)
    tty_attributes.c_cflag |= CS6; // 6bit
  else if (databits == 7)
    tty_attributes.c_cflag |= CS7; // 7bit
  else
    tty_attributes.c_cflag |= CS8; // 8bit

  // tty_attributes.c_iflag |=ISTRIP;

  /* paritetsval */

  if (parity == SER_ODD || parity == SER_EVEN) {
    tty_attributes.c_cflag |= PARENB; // sl� p� paritet
    tty_attributes.c_iflag
        |= IGNPAR; // vi vill bara skicka paritet inte kolla inkommande
    if (parity == SER_EVEN)
      tty_attributes.c_cflag &= ~PARODD; // even om inte odd
    if (parity == SER_ODD)
      tty_attributes.c_cflag |= PARODD; // odd
  } else
    tty_attributes.c_cflag &= ~PARENB; // sl� av paritet

  /* stopbitsval */

  if (stopbits == 2)
    tty_attributes.c_cflag |= CSTOPB; // 2 stopbitar
  else
    tty_attributes.c_cflag &= ~CSTOPB; // 1 stopbit

  /* fl�deskontroll */
  tty_attributes.c_iflag &= ~IXON; // ingen XON/XOFF in

  // tty_attributes.c_iflag &= (V_IGNCR);
  // tty_attributes.c_iflag &= (IGNPAR | V_IGNCR);
  // tty_attributes.c_iflag &= ~(BRKINT | IXON | V_INLCR | V_ICRNL);
  // tty_attributes.c_cflag &= ~(CSIZE | CSTOPB | PARENB);  //fippla om v�rden
  // lite granna

  sts = tcsetattr(fd, TCSANOW, &tty_attributes);

  if (sts < 0)
    return (0);

  tcflush(fd, TCIOFLUSH); // spola porten innan vi b�rjar

  return fd;
}

/************************************************************************
**************************************************************************
*
* Namn : RemUtils_ConvertR50ToAscii
*
* Typ  : int
*
* Typ           Parameter              IOGF     Beskrivning
*
* Beskrivning : Help routine for conversion from Radix50 to Ascii
*
**************************************************************************
**************************************************************************/

char RemUtils_ConvertR50ToAscii(int i)
{
  char asc;

  if (i == 0) {
    asc = ' ';
    return asc;
  }

  if (i >= 1 && i <= 26) {
    asc = i + 64;
    return asc;
  }

  if (i == 27) {
    asc = '$';
    return asc;
  }

  if (i == 28) {
    asc = '.';
    return asc;
  }

  if (i >= 30 && i <= 39) {
    asc = i + 18;
    return asc;
  }

  asc = 0;
  return asc;
}

/************************************************************************
**************************************************************************
*
* Namn : RemUtils_R50ToAscii
*
* Typ  : int
*
* Typ           Parameter              IOGF     Beskrivning
*
* Beskrivning : Conversion from Radix50 to Ascii
*
**************************************************************************
**************************************************************************/

int RemUtils_R50ToAscii(unsigned short R50[], char asc[])
{
  int i, j, k, index, ascindex = 0;
  unsigned short radix[2];

  for (index = 0; index < 2; index++) {
    k = R50[index] % 40;
    radix[index] = R50[index] - k;
    radix[index] = radix[index] / 40;
    j = radix[index] % 40;
    radix[index] = radix[index] - j;
    i = radix[index] / 40;
    asc[ascindex++] = RemUtils_ConvertR50ToAscii(i);
    asc[ascindex++] = RemUtils_ConvertR50ToAscii(j);
    asc[ascindex++] = RemUtils_ConvertR50ToAscii(k);
  }

  return true;
}

/************************************************************************
**************************************************************************
*
* Namn : RemUtils_AsciiToR50
*
* Typ  : int
*
* Typ           Parameter              IOGF     Beskrivning
*
* Beskrivning : Conversion from Ascii to Radix50
*
**************************************************************************
**************************************************************************/

int RemUtils_AsciiToR50(char asc[], short R50[])
{
  int i;
  short dig_vec[6];

  for (i = 0; i < 6; i++) {
    if (asc[i] >= '0' && asc[i] <= '9')
      dig_vec[i] = asc[i] - 18;
    else {
      if (asc[i] >= 'A' && asc[i] <= 'Z')
        dig_vec[i] = asc[i] - 64;
      else {
        if (asc[i] >= 'a' && asc[i] <= 'z')
          dig_vec[i] = asc[i] - 96;
        else {
          if (asc[i] == '$')
            dig_vec[i] = 27;
          else {
            if (asc[i] == '.')
              dig_vec[i] = 28;
            else {
              if (asc[i] == ' ')
                dig_vec[i] = 0;
              else
                dig_vec[i] = 35;
            }
          }
        }
      }
    }
  }
  R50[0] = ((dig_vec[0] * 40 + dig_vec[1]) * 40 + dig_vec[2]);
  R50[1] = ((dig_vec[3] * 40 + dig_vec[4]) * 40 + dig_vec[5]);
  return true;
}
