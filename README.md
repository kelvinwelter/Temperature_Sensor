# Sensor de Temperatura e Umidade usando Nanofox IoT
  Nessa página iremos apresentar outra aplicação usando a placa Nanofox IoT, um pouco mais complexa que o Smart Buttom mas que ainda assim é possível realizar esse projeto sem menores problemas, se trata de um sensor de temperatura e umidade via Nanofox IoT!

## Recomendações: 
- [Arduino IDE instalada no computador.](https://www.arduino.cc/en/Guide/HomePage)
- [Conta cadastrada na plataforma Tago.](https://tago.io/)
- [Ativação do kit Nanofox IoT.](https://github.com/Gridya/NANOFOX-Activate/blob/master/README.md)
- [Realizar o projeto Smart Buttom antes!](https://github.com/Gridya/nanofox/tree/master/examples/Smart_Buttom)
- [Integração da plataforma Tago com o Backend Sigfox](https://tago.elevio.help/en/articles/33)
- [Biblioteca Adafruit_HTU21DF instalada](https://github.com/adafruit/Adafruit_HTU21DF_Library/archive/master.zip)

## Aplicação do Projeto: 
  Esse projeto tem como principal objetivo mostrar como trabalhar com sensores usando Nanofox IoT, também será mostrado como tratar os dados desses sensores. A principal aplicação do projeto é a leitura da temperatura e umidade em um ambiente para então transmitir esses dados para a plataforma Tago através da rede Sigfox.

## Resumo do projeto: 
  O projeto será dividido nas seguintes partes:
- Desenvolvimento do circuito eletrônico: onde toda a parte de hardware será montada e deixada pronta para as etapas seguintes.
- Programação do firmware: é através da programação no Arduino Nano IO que iremos estabelecer toda a lógica por trás do projeto.
- Integração com a plataforma Tago: precisamos fazer alguns acertos para que nossos dados sejam recebidos de forma correta na Tago, essa etapa se encarrega disso.
- Visualização dos dados com a plataforma Tago: após tudo estar pronto, podemos partir para a melhor visualização de nossos dados na dashboard da Tago.

## Primeiro passo: desenvolvimento do circuito eletrônico
  Para realizarmos as medições de temperatura e umidade vamos utilizar o sensor HTU21D. Esse sensor, compacto mas que ainda assim é preciso, é muito simples de se usar. Tendo apenas que conectar seus 4 pinos na placa Nanofox IoT conforme o esquemático:
  
  [![esquematico-temperature-sensor.png](https://i.postimg.cc/445fwgw9/esquematico-temperature-sensor.png)](https://postimg.cc/9zDHFs6m "Esquemático do Sensor de Temperatura")
   
  A montagem é muito simples, com somente 4 pinos para conectar, as chances de algo dar errado são mínimas. Porém, mantenha a atenção, não troque os pinos SDA e SCL de lugar, eles devem ser conectados conforme o esquemático já que o sensor é um dispositivo I²C.

[![IMG-20181212-162751.jpg](https://i.postimg.cc/wMKbPpsJ/IMG-20181212-162751.jpg)](https://postimg.cc/WFnnFx8b "Resultado final da montagem")
  
  Após realizarmos as devidas conexões, nossa placa Nanofox IoT já está pronta para ser programada. A parte física do nosso projeto deve ficar conforme a imagem abaixo, se estiver tudo de acordo prossiga para a próxima etapa:
  
## Segundo passo: programação do Arduino (Firmware)
  Após termos concluído a parte do hardware, iremos para a programação. O código fonte do projeto está disponibilizado nesse repositório, aqui iremos apenas realizar a explicações de alguns trechos de maior importância:
  #### Bibliotecas necessárias
   Para esse projeto, serão necessárias apenas as bibliotecas [Adafruit_HTU21DF](https://github.com/adafruit/Adafruit_HTU21DF_Library/archive/master.zip) e [Nanofox](https://github.com/Gridya/nanofox). Sem a biblioteca Adafruit_HTU21DF não conseguiríamos realizar as leituras do nossso sensor e sem a biblioteca Nanofox não conseguiriamos transmitir nossos dados.
   #### Função Setup
   Em nosso setup, realizamos todas as inicializações necessárias, que vão desde o Arduino Nano IO até as referências de tempo que serão usadas mais tarde. Além disso, também programamos algumas mensagens para serem exibidas no monitor serial (também inicializado na função).
   ```c++
  Serial.begin(9600);   //Init Debug serial port
  htu.begin();          //Init sensor HTU21DF 
  MyNanofox.Init_ArduinoNano_IO();  //Setup arduino Nano IO
  MyNanofox.Init_Modem_WISOL(RC2);  //Initialize WISOL Sigfox Modem
  Serial.println("Temperature and humidty sensor example with NANOFOX IoT Kit!");
  Serial.println("");
  timeref_ms = millis();    // Init time reference 
  timeref_per_ms = millis();  // Init time reference 
   ```
   #### Função Loop 
   Em nossa função loop, temos o monitoramento constante da temperatura e umidade (mais detalhes a seguir) e trabalhamos com algumas referências de tempo para que o programa envie dados para a plataforma Tago a cada 15 minutos e também realize leituras do ambiente de 1 em 1 segundo.
   ```c++
  delay(10);
  //Real time monitoring
  if((millis() - timeref_ms) > 1000) { 
    timeref_ms = millis(); //restart time reference
    Read_Sensor();
  }
  
  //Periodically send data to Sigfox
  if((millis() - timeref_per_ms) > ((10000)*1)){ 
    Serial.println("Enviando dados");
    timeref_per_ms = millis(); //restart time reference
    Build_Uplink_Msg();
    MyNanofox.Send_Payload_Sigfox(&Uplink_Buffer[0],4,&Downlink_Buffer[0],0);
  }
   ```
   #### Função Build_Uplink_Msg
   Essa função é chamada a cada 15 minutos, no momento em se iniciará uma transmissão para a rede Sigfox. Para transmitirmos nossos dados, eles devem estar "construídos" em uma mensagem apta a ser enviada, e é exatamente isso que essa função faz, trabalhando com ponteiros para que a mensagem de uplink seja construída.
   ```c++
   Pointer_16 = (uint16_t*)&Uplink_Buffer[0];
  *Pointer_16 = (uint16_t)(temperature*100);
  Pointer_16++;
  *Pointer_16 = (uint16_t)(humidity*100);
   ```
   #### Função Read_Sensor
   Todo o segundo em que essa função for chamada, irá ocorrer a leitura da temperatura e umidade do ambiente. Além disso, esses dados serão mostrados no monitor serial. Uma função simples e opcional, que tem como maior propósito a organização do código, você pode realizar as leituras no próprio loop. 
   ```c++
    temperature = htu.readTemperature();
    humidity = htu.readHumidity();
    Serial.print("Temperature: ");
    Serial.println(String(temperature) + " C ");
    Serial.print("Humidity: ");
    Serial.println(String(humidity) + " % ");
   ```
   
   Após o esclarecimento das funções e funcionamento delas, podemos prosseguir para a próxima etapa.
   
 ## Terceiro passo: integração com a plataforma Tago
 Lembra-se da função Build_Uplink_Msg? Aquela função é essencial para que agora realizemos a integração com a plataforma Tago. Os dados foram enviados para a plataforma, mas estão todos na forma de hexadecimais e de forma conjunta. Precisamos analisar, organizar e separar esses dados. Para isso iremos usar a ferramenta de scripts da Tago juntamente do uso de callbacks no Sigfox Backend. Primeiramente no Sigfox Backend devemos tratar os dados para que eles já cheguem como variável na Tago. Para isso, devemos estabelecer uma configuração para nossa payload como na imagem a seguir: 
 
 [![custom-callback.png](https://i.postimg.cc/qq2WBYxn/custom-callback.png)](https://postimg.cc/2L8020J8 "Custom Payload Configuration")
 
 Além disso, devemos fazer algumas alterações na aplicação JSON do **Uplink** para incluir a temperatura e umidade, conforme a imagem abaixo. O código completo a se colocar na aplicação do **Uplink** está disponibilizado nesse repositório também, procure pelo arquivo **callback_application**.
 
 [![application-json.png](https://i.postimg.cc/Nfcj8vV3/application-json.png)](https://postimg.cc/vD0yMkGh "Código JSON no callback")
 
 Após essas alterações no Sigfox Backend, iremos criar uma análise na plataforma Tago para deixar esses dados com suas respectivas unidades e valores originais. Para isso, crie uma análise na plataforma Tago e preencha a aba **General Information** com o nome que deseja, além das outras opções que devem ficar como a imagem a seguir:
 
 [![general-information.png](https://i.postimg.cc/vT7mpGD6/general-information.png)](https://postimg.cc/YL9kFK6p "Configuração da aba General Information da Análise")
 
 Após isso, copie e cole na aba **Script** o código para processar a temperatura e umidade (dividir por 100 e colocar as respectivas unidades). Esse código está disponibilizado nesse repositório, procure por **nanofox_parser**. Após colocar o código na aba **Script**, entre na aba **Environment Variables**, lá você deve criar uma variável de nome **account_token** e colocar o token de sua conta (Pode ser encontrado em My Account -> Tokens). Dessa forma, sua aba **Environment Variables** deve ficar assim:
 
 [![account-token.png](https://i.postimg.cc/15P04tG2/account-token.png)](https://postimg.cc/PN3vRdFQ "Configuração da aba Environment Variables da Análise")
 
 Pronto! Fazendo tudo conforme descrito, salve sua análise. Agora vamos criar uma **Action** para que essa análise entre em ação sempre que a plataforma Tago receber dados da rede Sigfox. Na aba **Actions** clique no botão **Add Action**. Assim, você entrará na aba **General Information** de uma nova action. Dentro dessa aba, preencha com o nome da action, escolha ação a ser tomada como **Run Analysis** e então escolha a análise que você criou agora há pouco, sua tela deve ficar parecida com a imagem a seguir:
 
 [![action.png](https://i.postimg.cc/zfjYZ3kN/action.png)](https://postimg.cc/wt7GDqP4 "Configuração da aba General Information da action")
 
 Agora na aba **Trigger**, escolha a variável **data** do seu device e também selecione as seguintes opções conforme os destaques da imagem a seguir e sua ação estará pronta para ser salva e usada!
 
 [![trigger.png](https://i.postimg.cc/sxGLX25v/trigger.png)](https://postimg.cc/xNY5pnx2 "Configuração da aba Trigger da action")
 
 Feito isso, seus dados já estarão chegando na plataforma Tago e sendo processados de volta para como devem ser mostrados. Se tudo estiver funcionando, você pode transmitir dados para a plataforma, acessar o bucket para onde seus dados estão indo e lá você encontrará as seguintes variáveis e suas respectivas unidades:
 
 [![variaveis.png](https://i.postimg.cc/kMWZk2Mp/variaveis.png)](https://postimg.cc/2382W8ZQ "Variáveis provenientes da análise")
 
 Se tudo funcionou corretamente, podemos ir para o próximo e último passo, construir uma dashboard!
 
 ## Quarto passo: melhorando a visualização dos dados
 Para melhorarmos a visualização dos dados iremos trabalhar com a criação de widgets na plataforma Tago. A Tago proporciona ferramentas para visualização dos dados que podem ser criadas de forma prática e ágil. Vamos começar criando uma nova Dashboard, para fazer isso, basta clicar em **Add Dashboard**, na aba **Dashboards**, escolher um nome e um ícone caso queira. Após a dashboard ser criada, vamos criar alguns widgets:
 ### Gráficos
 Começando pelos gráficos (**Charts na tago.io**), podemos escolher os mais diversos tipos de gráficos e customizar vários aspectos. Em nosso projeto, queremos um monitoramento detalhado da temperatura e umidade, assim, criaremos um gráfico para cada um desses dados. Para criar o gráfico, basta escolher as variáveis que você deseja plotar e então, realizar algumas configurações adicionais na aba **Parameter Configuration**. Com as devidas configurações feitas, nossos gráficos ficaram assim:
 
 [![graficos.png](https://i.postimg.cc/cH8GPGj2/graficos.png)](https://postimg.cc/TLGsWFTJ)
 
 ### Icons
 Outro widget que colocamos em nosso projeto é o **Icons**, essa ferramenta em nosso caso foi usado para mostrar a última leitura de temperatura e umidade coletada e enviada para a rede Tago. Assim, configurando para mostrar as unidades e escolhendo ícones de sua preferência, o widget ficou conforme a imagem: 
 
 [![icons.png](https://i.postimg.cc/XqT9jjgN/icons.png)](https://postimg.cc/m1yPVGqK "Widget Icons")
 
 ### Outras formas de representação de dados
 Como você já deve ter visto, a plataforma Tago disponibiliza os mais diversos widgets para você criar uma dashboard do seu jeito. A nossa ficou assim:
 
 [![analise.png](https://i.postimg.cc/4x5k9mH0/analise.png)](https://postimg.cc/68yms60V "Dashboard criada")
 
 Mas não se prenda somente a nossa dashboard, explore outros widgets e libere sua criatividade, comece dando uma olhada nesse [artigo explicando os widgets da TagoIO](https://tago.elevio.help/en/categories/9-widgets). A plataforma Tago tem ferramentas poderosas e saber usá-la da melhor forma possível é um grande diferencial em sua jornada Nanofox IoT. Parabéns por desenvolver esse projeto! Agora você já pode aplicar seus conhecimentos em outros tipos de sensores para captação de dados além de saber como representá-los da melhor maneira possível na Tago.io! Acompanhe a Gridya nas redes sociais e fique ligado nos próximos exemplos que serão lançados!
   
