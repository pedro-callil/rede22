Programa Rede22, para uso no laboratório de Redes e Tubulações da disciplina PQI-3409
=====================================================================================

O Programa
----------

Foi escrito um programa, em C, que implementa o algoritmo descrito no artigo
citado [[1]](#1) para análise de redes de tubulação, para utilização no
laboratório "Redes e Tubulações" da disciplina PQI-3409 (EP-USP).

Instalação e Uso
----------------

O modo mais simples de adquirir o programa é o simples _download_ do executável,
[nas _releases_](https://github.com/pedro-callil/rede22/releases/tag/v0.0.0).

O programa deve ser invocado em um terminal, fornecendo através das _flags_
`--file` e `--results` os arquivos de entrada e saída, respectivamente.
Alguns exemplos de arquivos de entrada estão mostrados no diretório
[examples](https://github.com/pedro-callil/rede22/tree/main/examples).

```
rede.exe --file <arquivo de entrada.r2> --results <arquivo de saída.r2out>
```

Autor
-----

Pedro Callil <pedrocallil@usp.br>

Referências
-----------

<a id="1">[1]</a>
GUT, J. A. W.; PINTO, J. M.; SONG, T. W. Análise de redes de tubulação:
Desenvolvimento de instalação experimental e modelagem matemática. In: XIV Congresso
Brasileiro de Engenharia Mecânica, Anais da COBEM 97. Bauru: [s.n.], 1997.

