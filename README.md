# BAS

Basic TUI text editor made in C that has only one dependency: ncurses.

Right now there are two modes: _normal_ and _insertion_.

In _normal_ mode you can navigate the file using hjkl keys, save the file pressing 's', open a new one pressing 'o' and enter _insertion mode_ pressing 'i'.

In _insertion_ mode you can insert and remove text from the file, as well as go back to _normal mode_ by pressing ESC.

You can only insert ASCII characters that don't include accent marks.

It probably doesn't work on Windows. On Unix-like systems, tho, you simply install ncurses, then run:
```sh
make bas
```
Inside the project's folder and finally execute it with:
```sh
./bas
```
