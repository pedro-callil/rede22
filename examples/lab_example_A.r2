EXEMPLO DE SISTEMA: CONFIGURAÇÂO A

                Tubo 1
                Diâmetro 2.5cm
Cota 0.12m      Leq 20.9m              Cota 2.41m
      [ 3 ] --------------- [ 1 ] -->  Pressão 1atm (aberto)
        |
        |   Tubo 3
        |   Diâmetro 3.2cm
        |   Leq 4.15m
        |
  --> [ 4 ]  Cota 1.17m Vazão 3.6m3/hr
        |
        |   Tubo 4
        |   Diâmetro 3.2cm
        |   Leq 2.15m
        |
      [ 5 ] --------------- [ 2 ] -->  Pressão 1atm (aberto)
Cota 1.17m     Tubo 2                 Cota 2.41m
                Diâmetro 2.5cm
                Leq 24.2m

type newtonian
maxiter 1000
dampening 0.67
tolerance 0.0001

density 1.000
viscosity 0.8891

rugosity 0.0015

diameter 2.5
pipes
1 lenght 20.9 start 3 end 1
2 lenght 24.2 start 5 end 2
3 lenght 4.15 start 4 end 3 diameter 3.2
4 lenght 2.15 start 4 end 5 diameter 3.2

nodes
1 2.41 external
2 2.21 external
3 0.12
4 1.17 external
5 1.17

specifications
1 pressure 1
2 pressure 1
4 flow 3.6

