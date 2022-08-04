# Instruções para compilar
g++ -o montador main.cpp -std=c++11

./montador -p input.asm output.obj

ou

./montador -o PPInput.asm output.obj


- [X] NÃO ser sensível ao caso, podendo aceitar instruções/diretivas/rótulos em maiúsculas e minúsculas.
- [X] Desconsiderar tabulação, quebras de linhas e espaços desnecessários em qualquer lugar do código.
- [X] A diretiva CONST deve aceitar números positivos, negativos e hexadecimal (0xF1)
- [X] O comando COPY deve utilizar uma vírgula e um espaço entre os operandos (COPY A, B)
- [] Poder criar um rótulo, dar quebra de linha e continuar a linha depois (o rótulo seria equivalente a linha seguinte)
- [X] Possível fazer comentários
- [X] -p: pre-processar as diretivas EQU e IF
- [X] -o: traduzir o código utilizando o algoritmo de duas ou uma passagem
- [X] Processar BEGIN, EXTERN, PUBLIC e END
- [] Criar tabela de def.
- [] Criar tabela de uso
- [] Detectar erros:
    - [] declarações rótulos ausentes;
    – [] declarações ou rótulos repetidos;
    – [] instruções com a quantidade de operando errado;
    – [] tokens inválidos;
    – [] dois rótulos na mesma linha;
    - [] instruções ou diretivas nas seções erradas;
    - [X] falta de seção de texto.
