# Título do Projeto

Sistema de monitoramento de movimento de Joystick

## Objetivo Geral

O sistema proposto faz um monitoramento do movimento de um Joystick, verificando se houve movimento, e apresentando indicações visuais e sonoras do acontecido, além de movimentar um indicador no display, podendo movê-lo pela tela.

## Descrição Funcional

O sistema monitora o joystick presente na placa e indica o seu movimento. O movimento é mapeado no display OLED, mostrando ao usuário em qual posição o joystick está. Além disso, apresenta indicações visuais e sonoras do movimento do joystick, aumentando a intensidade do LED RGB e também o volume do buzzer conforme o mesmo se afasta do centro. É possível, ainda, mudar a cor do indicador visual, e até mesmo cancelar os indicadores visuais e sonoros, caso o usuário queira, apertados nos botões B e A, respectivamente.

## Uso dos Periféricos da BitDogLab
Foram utilizados os seguintes periféricos no projeto, para satisfazer as exigências da presente atividade proposta:

- **Potenciômetros do joystick:** servem para mapear a direção do **joystick**, e verificar em qual posição ele está, podendo assim mostrar essa posição no **display OLED**, usando uma fórmula de equivalência;
- **Botões A e B:** foram utilizados para ativação de **interrupções** externas;
- **Display OLED:** utilizado para mapear o movimento do **joystick**, utilizando a posição do mesmo com base nos **potenciômetros**;
- **Matriz de LEDs:** utilizada para indicar qual botão foi pressionado, apresentando uma seta apontando para o botão em questão;
- **LED RGB:**  utilizado para indicar, visualmente, que o **joystick** se afastou do centro, aumentando a intensidade com base na distância do **joystick** do centro;
- **Buzzer:** utilizado para indicar, sonoramente, que o **joystick** se afastou do centro, aumentando o volume com base na distância do **joystick** do centro;
- **Interrupções nos botões:** utilizadas para desligar os avisos visuais e sonoros de movimento do **joystick** (**botão A**) e para alterar o aviso visual (**botão B**);
- **Interrupções de timer:** utilizado para limpar a **Matriz de LEDs** logo depois de apresentar a seta para o botão pressionado.
- **Tratamento de debounce:** utilizado para tratar o efeito bouncing nos botões A e B que foram utilizados para **interrupções**. Foi feito um tratamento capturando a última vez que o botão foi pressionado, e colocando um atraso de 200 milissegundos para a próxima ativação.

## Vídeo ensaio

Clique em ***[link do video]()*** para visualizar o vídeo ensaio do projeto.