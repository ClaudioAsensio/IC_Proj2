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
./sndfile-example-bin/wav_quant ../sample.wav 0 quantized_sample.wav

criar histograma: ./sndfile-example-bin/wav_hist quantized_sample 0 >> quantized_sample.txt

```
---

- Exercício 4 e 5:
Dentro da pasta src executar
```shell
./audio_codec 
```
Ao executar,serão feitas as perguntas ao utiliador por forma a escolher qual o audio que deseja que seja comprimido e descomprimido,bem como a opção de ser lossy ou não
O ficheiro comprimido fica em "FileCompressed.bin" e o descomprimido em "decoded.wav"
Na pasta src do repositorio encontram-se alguns audios para teste
---

- Exercício 6:

```shell
No diretório sndfile-example-src:
../sndfile-example-bin/wav_effects sample.wav left_mute.wav mute           # efeito de silenciar canal
../sndfile-example-bin/wav_effects sample.wav echo-output.wav single-echo  # efeito de single echo

```
-----
