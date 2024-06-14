# 10

Sistema Operativo Usado : Windows 10 usado WSL.
Cálculo do tempo : utilidade "time" do sistema UNIX.
Cálculo médio do tempo : média do tempo real da execução do prorgama 3.

CHILD_N = 1 ARRAY_SIZE = 1000000 Tempo Médio: 17 ms
CHILD_N = 2 ARRAY_SIZE = 1000000 Tempo Médio: 15 ms
CHILD_N = 3 ARRAY_SIZE = 999999 Tempo Médio: 15 ms
CHILD_N = 4 ARRAY_SIZE = 1000000 Tempo Médio: 15 ms
CHILD_N = 5 ARRAY_SIZE = 1000000 Tempo Médio: 14 ms
CHILD_N = 6 ARRAY_SIZE = 999996 Tempo Médio: 15 ms
CHILD_N = 12 ARRAY_SIZE = 999996 Tempo Médio: 16 ms

Conclusão: Num sistema de 6 cores, obtém-se melhor performance ao correr 5 processos adicionais, pois cada processo, incluindo o processo pai, fica livre de correr num core diferente. Com um número reduzido de processos filho, a performance diminui, pois a capacidade total do processador não é aproveitada. Com um número maior de processos filho, a performance sofre devido ao overhead de criar um novo processo e de gestão de processos por parte do sistema operativo.