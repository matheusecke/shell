# Sobre o Projeto

Projeto desenvolvido para a matéria de Projetos de Sistemas Operacionais. Neste trabalho, criamos um Shell utilizando a linguagem C.

## Como usar o Shell

```sh
sudo mount -t vboxsf <nome da sua pasta de compartilhamento> <nome da sua pasta de compartilhamento>

Comandos disponiveis:
pwd
ls
ls -lha
cd <pasta dentro do repositorio>

Como compilar o programa:
gcc -c shell.c
gcc -o shell shell.o
./shell

Comandos internos:
cd
exit
path /<path default>
path default é /bin /usr/bin
Comandos externos:
ls
ls -l
ls -a
path default -> path /bin/ usr/bin 
