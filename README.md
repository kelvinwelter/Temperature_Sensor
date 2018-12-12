# Sensor de Temperatura e Umidade usando Nanofox IoT
  Nessa página iremos apresentar outra aplicação usando a placa Nanofox IoT, um pouco mais complexa que o Smart Buttom mas que ainda assim é possível realizar esse projeto sem menores problemas, se trata de um sensor de temperatura e humidade via Nanofox IoT!

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
- Programação do firmware: através da programação do firmware que iremos estabelecer toda a lógica por trás do projeto.
- Integração com a plataforma Tago: precisamos fazer alguns acertos para que nossos dados sejam recebidos de forma correta na Tago, essa etapa se encarrega disso.
- Visualização dos dados com a plataforma Tago: após tudo estar pronto, podemos partir para a melhor visualização de nossos dados na dashboard da Tago.

## Primeiro passo: desenvolvimento do circuito eletrônico
  Para realizarmos as medições de temperatura e umidade vamos utilizar o sensor HTU21D. Esse sensor, compacto mas que ainda assim é preciso, é muito simples de se usar. Tendo apenas que conectar seus 4 pinos na placa Nanofox IoT conforme o esquemático:
  
  [![esquematico-temperature-sensor.png](https://i.postimg.cc/445fwgw9/esquematico-temperature-sensor.png)](https://postimg.cc/9zDHFs6m)
   
  A montagem é muito simples, com somente 4 pinos para conectar, as chances de algo dar errado são mínimas. Porém, mantenha a atenção, não troque os pinos SDA e SCL de lugar, eles devem ser conectados conforme o esquemático já que o sensor é um dispositivo I²C.

  [![sensor-temperatura.jpg](https://i.postimg.cc/zDbcgTXV/sensor-temperatura.jpg)](https://postimg.cc/4nGPC7cg "Nanofox IoT conectado ao sensor de temperatura e umidade")
  
  Após realizarmos as devidas conexões, nossa placa Nanofox IoT já está pronta para ser programada. A parte física do nosso projeto deve ficar conforme a imagem abaixo, se estiver tudo de acordo prossiga para a próxima etapa:
  
## Segundo passo: programação do Arduino (Firmware)
  Após termos concluído a parte do hardware, iremos para a programação. O código fonte do projeto está disponibilizado nesse repositório, aqui iremos apenas realizar a explicações de alguns trechos de maior importância:
  #### Bibliotecas necessárias
   Para esse projeto, serão necessárias apenas as bibliotecas [Adafruit_HTU21DF](https://github.com/adafruit/Adafruit_HTU21DF_Library/archive/master.zip) e [Nanofox](https://github.com/Gridya/nanofox). Sem a biblioteca Adafruit_HTU21DF não conseguiríamos realizar as leituras do nossso sensor e sem a biblioteca Nanofox não conseguiriamos trasmitir nossos dados.
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
 
 [![custom-callback.png](https://i.postimg.cc/qq2WBYxn/custom-callback.png)](https://postimg.cc/2L8020J8)
 
 ## Quarto passo: melhorando a visualização dos dados
 Para melhorarmos a visualização dos dados iremos trabalhar com a criação de widgets na plataforma Tago. A Tago proporciona ferramentas para visualização dos dados que podem ser criadas de forma prática e ágil. Assim, vamos criar na nossa dashboard os seguintes widgets: 
   
