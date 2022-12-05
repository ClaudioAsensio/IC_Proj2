 # Informação e Codificação - Proj1


 Departamento de Electrónica, Telecomunicações e Informática - Universidade de Aveiro

 -----

## [Pré-requisitos -> instructions](instructions)

## [Compilação dos exercícios](README.md)


- Comandos gerais:
```c++
make                            # compilar todos os programas para execução (dentro da pasta src)

make clean                      # limpar programas de execução

```
-----
- Exercício 1:
```shell
./copy_img nome_img nome_img_saida                           

```
- Exercício 2:
```shell
./effects nome_img nome_img_saida m         #Espelhar
                                  r         #Rodar
                                  l         #Luminosidade
                                  n         #Negativa

```
---

- Exercício 3:
```shell
../bin/testGolomb -e <num-to-encode> -m <m-parameter>             # Codificar um inteiro em código Golomb com um dado m
../bin/testGolomb -d <bin-string-to-decode> -m <m-parameter>      # Descodificar uma string binária para um número inteiro com um dado m

```
---

- Exercício 4 e 5:

Dentro da pasta src executar:
```shell
./audio_codec -d para descomprimir um audio anteriormente comprimido.
./audio_codec -c para comprimir um audio 
```

Ao executar,serão feitas as perguntas ao utiliador por forma a escolher qual o audio que deseja que seja comprimido e descomprimido,bem como a opção de ser lossy ou não.
O ficheiro comprimido fica em "FileCompressed.bin" e o descomprimido fica num ficheiro com nome á escolha do utilizador.
Na pasta src do repositorio encontram-se alguns audios para teste

---

- Exercício 6:

```shell
No diretório src:
../bin/test_image_codec -e input_file output_file      # compress file
../bin/test_image_codec -d compressed_file output_file # decompress file

```
-----
