#!/usr/bin bash

echo "install boost-pretty-printer"
sudo git clone git://github.com/ruediger/Boost-Pretty-Printer.git ~/.boost 

echo "create ~/.gdbinit"
echo "python" > ~/.gdbinit
echo "import sys" >> ~/.gdbinit
echo "sys.path.insert(0, '~/.boost/Boost-Pretty-Printer')" >> ~/.gdbinit
echo "from boost.printers import register_printer_gen" >> ~/.gdbinit
echo "register_printer_gen(None)" >> ~/.gdbinit
echo "end" >> ~/.gdbinit
