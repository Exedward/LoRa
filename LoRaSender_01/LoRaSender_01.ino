#include "LoRaWan_APP.h"
#include "Arduino.h"

//------------------- Definicoes De Projeto -------------------
#define   RF_FREQUENCY                                915000000 // Hz

#define   TX_OUTPUT_POWER                             5        // dBm

#define   LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define   LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define   LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define   LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define   LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define   LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define   LORA_IQ_INVERSION_ON                        false


#define   RX_TIMEOUT_VALUE                            1000
#define   BUFFER_SIZE                                 30 //Define o tamanho do payload aqui

#define   PINO_STATUS                                 23
#define   PINO_CONTAGEM                               20

//------------------- Variáveis Auxiliares -------------------
bool status_atual = 0, status_ultimo = 0, state_atual_contangem = 0, state_ultimo_contangem = 0;
unsigned long cont = 0;

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];

bool lora_idle = true;

static RadioEvents_t RadioEvents;

void OnTxDone();
void OnTxTimeout();

void setup(){
    Serial.begin(115200);
    Mcu.begin();
	
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    
    Radio.Init(&RadioEvents);
    Radio.SetChannel(RF_FREQUENCY);
    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                      true, 0, 0, LORA_IQ_INVERSION_ON, 3000); 
}

void loop(){
	if(lora_idle == true){
    delay(500);
    status_atual = digitalRead(PINO_STATUS);
    state_atual_contangem = digitalRead(PINO_CONTAGEM);
    if((status_atual != status_ultimo) || ((state_atual_contangem != state_ultimo_contangem) && state_atual_contangem)){
      cont += 1;
      sprintf(txpacket,"status: %s;contagem: %lu", status_atual == false ? "0" : "1", cont);  //Start package
      Serial.printf("\r\nEnviando Package... \"%s\" , Tamanho: %d\r\n", txpacket, strlen(txpacket));
      Radio.Send((uint8_t *)txpacket, strlen(txpacket) ); //send the package out	
      lora_idle = false;
    }
	}
  Radio.IrqProcess();
  status_ultimo = status_atual;
  state_atual_contangem = state_ultimo_contangem;
}

void OnTxDone(){
	Serial.println("TX done...");
	lora_idle = true;
}

void OnTxTimeout(){
    Radio.Sleep();
    Serial.println("TX timeout...");
    lora_idle = true;
}