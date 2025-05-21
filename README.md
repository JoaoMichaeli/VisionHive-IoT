# 🛵 Mapeamento Inteligente do Pátio - Mottu

Solução de IoT com ESP32 para identificação, rastreamento e visualização em tempo real da localização de motos nos pátios das mais de 100 filiais da Mottu no Brasil e no México.

---

## 📌 Descrição do Projeto

Este projeto tem como objetivo automatizar e otimizar a gestão das motos nos pátios da Mottu utilizando microcontroladores ESP32 conectados via Wi-Fi, sensores físicos e um sistema web responsivo. A proposta visa garantir uma operação mais ágil, segura e escalável, eliminando o controle manual e aumentando a precisão na localização e no monitoramento dos veículos.

---

## 🎯 Objetivos

- Identificar rapidamente uma moto específica no pátio utilizando um alerta visual e/ou sonoro.
- Fornecer uma visualização **em tempo real** da chamada e do status de resposta da moto.
- Garantir escalabilidade para aplicação em mais de 100 filiais com diferentes layouts.
- Oferecer uma interface intuitiva, acessível por desktop e mobile.
- Integrar sensores IoT nas motos para coleta automatizada de dados e status.

---

## 🔧 Componentes da Solução

### 1. Dispositivo IoT com ESP32

- Cada moto recebe um **dispositivo ESP32** com LED e/ou buzzer, encapsulado em uma **caixa protetora impressa em 3D** resistente à água e poeira.
- O ESP32 é alimentado por uma **bateria recarregável 18650**, com sistema de gerenciamento de carga (TP4056).
- O ESP32 permanece em **Deep Sleep** para economizar energia, acordando em intervalos programados ou por solicitação.
- Ao receber um chamado via rede Wi-Fi, o dispositivo acende o LED ou ativa o buzzer, permitindo rápida identificação da moto no pátio.

### 2. Mapeamento Digital do Pátio

- Representação visual interativa da disposição das motos.
- Atualização em tempo real com base na comunicação dos ESPs.
- Suporte a diferentes tamanhos e layouts de pátio.

### 3. Interface Web/App

- Painel de controle responsivo e intuitivo.
- Visualização do pátio com posição aproximada das motos.
- Botão para chamar a moto específica, com retorno visual/auditivo imediato.
- Acesso via desktop ou dispositivos móveis.

### 4. Integração com Backend

- Infraestrutura leve para comunicação via Wi-Fi com os dispositivos ESP32.
- Registro dos eventos de chamada, status da moto e tempo de resposta.
- API para envio de comandos às unidades nos pátios.

---

## 🔋 Alimentação e Eficiência Energética

- A bateria **18650 recarregável (2500 mAh)** garante autonomia de **até 38 dias** com uso otimizado de Deep Sleep.
- O módulo **TP4056** permite carregamento seguro via USB e protege contra descarga excessiva.
- A caixa 3D garante proteção IP54 para uso em ambientes externos.

---

## 🌎 Escalabilidade

A arquitetura da solução foi projetada para ser facilmente adaptável a diferentes contextos de filiais, permitindo:

- Inclusão de novos pátios sem reestruturação do sistema.
- Customização de layout de pátio conforme necessidade local.
- Manutenção de alto desempenho mesmo com milhares de motos e dispositivos conectados.

---

## 📝 Licença

Este projeto é de uso acadêmico/desafio técnico. Uso e distribuição devem ser autorizados pela organização proponente (Mottu).

---
