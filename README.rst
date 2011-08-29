waitproc -- Wait for an arbitrary process
*****************************************

The wait family of system calls and the wait Bash built-in command
allow one to wait for child process, while this command allows one to
wait for an arbitrary process.


| Homepage: http://linuxbox.fi/~vmj/waitproc/
| Download: http://linuxbox.fi/~vmj/waitproc/archive/
| Source code: http://github.com/vmj/waitproc

.. contents::


Basic usage
===========

The program `waitproc(1)`_ waits for process whose process ID (PID) is
given on the command line.  Following shows a typical invocation::

    $ waitproc 12345

.. _waitproc(1): http://www.linuxbox.fi/~vmj/waitproc/waitproc.1.html


Requirements
============

GNU make and GNU C libraries on Linux.  No attempt has been made
towards portability.  Take this as a challenge.


Installation
============

Type 'make install' to install the binary into '/usr/local/bin' and
manual page to '/usr/local/man/man1'.

You can also define PREFIX to install waitproc in a different prefix:
'make install PREFIX=/usr' for example.  Similarly, there's DESTDIR,
BINDIR, MANDIR, and MAN1DIR for those who need them.


Authors
=======

Original author and current maintainer is Mikko Värri
(vmj@linuxbox.fi).


License
=======

waitproc is Free Software, licensed under GNU General Public License
(GPL), version 3 or later.  See LICENSE.txt file for details.
