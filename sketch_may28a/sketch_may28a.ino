const int PINO_SENSOR = A0;
const int PINO_RELE = A1;
const int VALOR_SECO = 800;
const int VALOR_MOLHADO = 350;
const int UMIDADE_LIGA = 35;
const int UMIDADE_DESLIGA = 55;
const unsigned long INTERVALO_LEITURA_MS = 1000;
const unsigned long TEMPO_TESTE_BOMBA_MS = 2000;
// Muitos modulos rele ligam com LOW. Troque para false se ficar invertido.
const bool RELE_ATIVO_EM_LOW = true;
bool bombaLigada = false;
bool modoAutomatico = true;
bool modoTeste = false;
unsigned long ultimaLeitura = 0;
unsigned long desligarTesteEm = 0;
void acionarBomba(bool ligar) {
  bombaLigada = ligar;
  digitalWrite(PINO_RELE, RELE_ATIVO_EM_LOW ? !ligar : ligar);
}
void processarComando(String comando) {
  comando.trim();
  comando.toUpperCase();
  if (comando == "BOMBA_TESTE") {
    modoAutomatico = false;
    modoTeste = true;
    desligarTesteEm = millis() + TEMPO_TESTE_BOMBA_MS;
    acionarBomba(true);
  } else if (comando == "BOMBA_ON") {
    modoAutomatico = false;
    modoTeste = false;
    acionarBomba(true);
  } else if (comando == "BOMBA_OFF") {
    modoAutomatico = false;
    modoTeste = false;
    acionarBomba(false);
  } else if (comando == "AUTO") {
    modoAutomatico = true;
    modoTeste = false;
  }
}
void lerComandosSerial() {
  static String buffer = "";
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (buffer.length() > 0) {
        processarComando(buffer);
        buffer = "";
      }
    } else {
      buffer += c;
    }
  }
}
int calcularUmidade(int leitura) {
  int umidade = map(leitura, VALOR_SECO, VALOR_MOLHADO, 0, 100);
  return constrain(umidade, 0, 100);
}
void enviarJson(int leitura, int umidade) {
  Serial.print("{\"sensor\":\"umidade_solo\",\"raw\":");
  Serial.print(leitura);
  Serial.print(",\"umidade\":");
  Serial.print(umidade);
  Serial.print(",\"bomba\":\"");
  Serial.print(bombaLigada ? "ligada" : "desligada");
  Serial.print("\",\"modo\":\"");
  if (modoTeste) {
    Serial.print("teste");
  } else if (modoAutomatico) {
    Serial.print("auto");
  } else {
    Serial.print("manual");
  }
  Serial.println("\"}");
}
void setup() {
  Serial.begin(9600);
  pinMode(PINO_RELE, OUTPUT);
  acionarBomba(false);
}
void loop() {
  lerComandosSerial();
  if (modoTeste && millis() >= desligarTesteEm) {
    modoTeste = false;
    acionarBomba(false);
  }
  if (millis() - ultimaLeitura >= INTERVALO_LEITURA_MS) {
    ultimaLeitura = millis();
    int leitura = analogRead(PINO_SENSOR);
    int umidade = calcularUmidade(leitura);
    if (modoAutomatico) {
      if (!bombaLigada && umidade <= UMIDADE_LIGA) {
        acionarBomba(true);
      }
      if (bombaLigada && umidade >= UMIDADE_DESLIGA) {
        acionarBomba(false);
      }
    }
    enviarJson(leitura, umidade);
  }
}