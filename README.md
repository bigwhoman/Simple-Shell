# Simple Shell
A simple Unix shell

## Building
```bash
sudo apt install libreadline8 libreadline-dev readline-doc
gcc -O3 -o shell main.c exec.c -lreadline
```
