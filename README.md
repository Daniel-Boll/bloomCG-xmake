## Crítico

- Criar objeto com posição inicial diferente buga gizmo.
- Luzes com posições zoadas.
- Atenuância não pegando ao movimentar as luzes, apenas o objeto.
- Múltiplas luzes

## Possíveis melhorias

- Adicionar sombreamento
- Adicionar cubo

## Requisitos

1) (10 pontos)  Modelar uma cena sintética inserindo várias esferas com diferentes raios, números de  meridianos  e  paralelos.  Lembre-se  que  estas  propriedades  devem  ser armazenadas junto às esferas e podem ser alteradas pelo usuário. 
2) (5 pontos)  Durante  a  execução  do  aplicativo  o  usuário  poderá  limpar  a  cena  e  iniciar  a construção de uma nova cena, sem ter de reiniciar o software. 
3) (15 pontos) Os  objetos  podem  ser  individualizados  (seleção) e  sofrerem  transformações geométricas de translação, rotação e escala. Como a cena modelada está em 3D, as operações acima podem ser realizadas em todas as direções (X, Y e Z). Note que a escala pode deformar a esfera, caso os fatores Sx, Sy e Sz sejam distintos. 
4) (20 pontos) Os  objetos  devem  ser  apresentados  em  uma  única  área  de  desenho,  seja  em projeção  paralela  axonométrica  ou  em  projeção  perspectiva.  Caberá  ao  usuário selecionar  o  tipo  de  projeção  que  será  empregada  no  processo  de  visualização. Para obter tais projeções será necessário informar todos os parâmetros de câmera (VRP, P, vetor view-up, tipo de projeção, distância aos planos Near, Far e plano de projeção);  os  limites  do  mundo  e  os  limites  do  plano  de  projeção  também  são parâmetros  definidos  pelo  usuário.  É  importante  que  nessa  janela  de  projeção sejam  apresentados  sinalizadores  mostrando  a  direção  dos  eixos  principais  do SRU, como mostrado no círculo verde da figura 2. Figura 2 – Eixos principais do SRU exibidos na vista em perspectiva. 
5) (30 pontos) Os objetos da cena sintética podem ser renderizados em sombreamento constante, sombreamento Gouraud ou sombreamento Phong simplificado. Caberá ao usuário selecionar uma das opções de renderização da cena. 
6) (15 pontos) O  sistema  deverá  permitir  a  edição,  em  tempo  de execução,  de  todos  os parâmetros da cena (câmera, tipo de projeção, propriedades das fontes luminosas –  ambiente  e  pontual,  dos  parâmetros  dos  materiais aplicados  a  cada  um  dos objetos  –  Ka,  Kd,  Ks  e  n).  Há  cor  na  cena  sintética,  o  que  significa  que  os parâmetros das fontes luminosas e dos materiais vinculados aos objetos devem ser definidos em tuplas em consonância com o sistema de cores RGB. A criação da 
cena, em um primeiro momento, pode ser realizada com parâmetros pré-definidos, mas depois podem ser editados pelo usuário, conforme sua necessidade. 
7) (5 pontos) O sistema deve ser estável. Ou seja, deve suportar as interações com o usuário sem  travar,  abortar  operações  ou  ser  encerrado  devido  a  erros  em  tempo  de execução.

## Feitos

- [X] 1. (completo)
- [X] 2. Adicionar limpeza da cena (completo)
- [ ] 3. Adicionar transformações de rotação e escala.
- [ ] 4. Adicionar possibilidade de informar P, view-up, tipo de projeção, distância aos planos Near, Far e plano de projeção.
  - [ ] 4.1 Adicionar sinalizadores
- [ ] 5. Adicionar sombreamento constante e Gouraud. Bem como poder escolher qual método será aplicado.
- [X] 6. Poder definir Ka, Kd, Ks e n do objeto. (completo)
- [X] 7. (completo)

**Soma** *a priori* dos pontos:
$$
35 * 1.2 = 42
$$
