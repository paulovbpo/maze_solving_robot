int S[]={2,3,4,5,6}; // declaração das portas dos sensores,(verde=s0, verde=s1,branco=s2 central, amarelo=s3, amarelo=s4)
int sensor[5];//vetor de sensores
int num=0,den=0, vp; //variaveis utilizadas no PID
int ve,vd; //Variavel que recebe os valores do pwm para velocidade
//int i;
int ci,vp0;
float G=1.7,c=0,cp=0,erro=0; //parametros do PID
int z,s; // z é utilizado para a função da curva e s para a medir distancia
int e = 0;
int d = 0;
int cr = 0;
int fim = 0;


int IN1 = 8 ;
int IN2 = 9 ;
int velocidadeA = 2; //motor esquerdo(ENA)
 
//motor_B
int IN3 = 10 ;
int IN4 = 11;
int velocidadeB = 7; //motor direito(ENB)
long ti,tf; //tempo inicial e final de cada reta
void setup() 
{
  Serial.begin(38400);//configura a taxa de comunicação serial em bits por segundo (baud rate)(9600)sem retorno
  pinMode(IN1, OUTPUT);// Configura o pino do IN1 como saída
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
 
  pinMode(velocidadeA,OUTPUT);
  pinMode(velocidadeB,OUTPUT);
  //for(i=8;i<=12;i++)
  pinMode(sensor[0],INPUT);
  pinMode(sensor[1],INPUT);
  pinMode(sensor[2],INPUT);
  pinMode(sensor[3],INPUT);
  pinMode(sensor[4],INPUT);
  delay(1000);//espera 2 segundos
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  ler_sensor();
 
  if(fim == 1)
  {
   parar(); 
   Serial.println("FIM");
  }
///////////////////////////////  
  else
  {
    ler_sensor();
    if(d ==  0 && e == 0 && sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 1 && sensor[3] == 0 && sensor[4] == 0)
    {
      frente();
      ler_sensor();
    }
  
    if ((sensor[1] == 1 || sensor[3] == 1) && (sensor[0] == 0 || sensor[4] == 0))
    {
      controlPID();
      ler_sensor();
    }
    ler_sensor();
    if(sensor [0] == 1 && sensor [4] == 0) // ESQUERDA
    {
      e = 1;
    }
    if(e == 1 && d == 0)
    {
      parar();
      posicionar();
      parar();
      ler_sensor();
      if(sensor[2] == 1)
      {
        int rcd = random(0,10);///SORTEIO
        Serial.println("Sorteio ESQUERDA x FRENTE.");
      
        if(rcd <= 5) // IR PRA FRENTE
        {
          Serial.println("FRENTE");
          e = 0;
        }
        else
        {
          Serial.println("ESQUERDA");
          esquerda();
        }
       }
       else
      {
        Serial.println("ESQUERDA");
        esquerda();
        delay(20);
      }
    }
  
    ler_sensor();
    if(sensor [0] == 0 && sensor[4] == 1) // DIREITA
    {
      d = 1;
    }
    if(e == 0 && d == 1)
    {
      parar();
      posicionar();
      parar();
      ler_sensor();
      if(sensor[2] == 1)
      {
        int rcd = random(0,10);///SORTEIO
        Serial.println("Sorteio DIREITA x FRENTE.");
        if(rcd <= 5) // IR PRA FRENTE
        {
          Serial.println("FRENTE.");
          d = 0;
        }
        else
        {
          Serial.println("DIREITA");
          direita();
        }
      }
      else
      {
        Serial.println("DIREITA");
        direita();
        delay(20);
      }
    }
    ler_sensor();
    if(sensor[0] == 0 && sensor[1] == 0  && sensor[2] == 0 && sensor[3] == 0 && sensor[4] == 0)
    {
      Serial.println("AJUSTAR");
      parar();
      esquerda_correcao();
    }
    if(sensor[0] == 1 && sensor[1] == 1 && sensor[2] == 1 && sensor[3] == 1 && sensor[4] == 1)
    {
      parar();
      posicionar();
      parar();
      ler_sensor();
      if(sensor[0] == 1 && sensor[1] == 1 && sensor[2] == 1 && sensor[3] == 1 && sensor[4] == 1)
      {
        fim = 1;
        parar(); 
        Serial.println("FIM");
      }
      else
      {
        delay(200);
        if(sensor[0] == 0 && sensor[2] == 1 && sensor[4] == 0)
        { // SE PUDER SEGUIR
          Serial.println("Sorteio FRENTE x DIREITA x ESQUERDA");
          int rcd2 = random(0,12);//sorteio do caminho a seguir
        
          if(rcd2 <= 4)
          { //SEGUIR
            Serial.println("FRENTE");
            parar();
            frente();
          }
        
          if(rcd2 > 4 && rcd2 <= 8)
          { // VIRAR A DIREITA
            Serial.println("DIREITA");
            parar();
            direita();
          }
        
          if(rcd2 > 8)
          { //VIRAR A ESQUERDA
            Serial.println("ESQUERDA");
            parar();
            esquerda();
          }
        
        }
      }
    }
  
  }
/////////////////////////////////////////////////////  
}
//////////////////////////////////////////////////////////////////////////////////////////////
void controlPID()
{
  den=0;
  num=0;
  for(int i=0;i<5;i++)
  {
      sensor[i]=digitalRead(S[i]);
      if ((i>=1)&&(i<=3))
      {
        num=(i+1)*sensor[i]+num;
        den=sensor[i]+den;
        delay(5);
      }
   }
  vp=((num*10)/den);
  if(den>0) {
    vp=num*10/den;
  }
  else {
    vp=vp0;
  }
  if(den>0) {
    vp0=vp;
  }
  erro=30-vp; //Caso apenas o sensor(2),no meio, esteja ativo o vp será 30
  cp=G*erro;
  c=cp;
  ve = 58-c;
  vd = 53+c;
  digitalWrite(IN1,HIGH);//avançar
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);//avançar
  digitalWrite(IN4,LOW);
  analogWrite(velocidadeA,ve);
  analogWrite(velocidadeB,vd);
}

void frente()
{
  s=s+1;
  if(s==1)
  {
    ti=millis();
    //Serial.println("ti");
    //Serial.println(ti);
  }
  digitalWrite(IN1,HIGH);//avançar
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);//avançar
  digitalWrite(IN4,LOW);
  analogWrite(velocidadeA,50);
  analogWrite(velocidadeB,48); 
}

void ler_sensor()
{
  for(int i=0;i<5;i++)
    {
      sensor[i]=digitalRead(S[i]); /*Atualiza os valores dos sensores. A função digitalRead() é usada para ler o estado lógico de um pino. 
      Ela é capaz de dizer se a tensão neste pino é alta (~ 5V) ou baixa (~ 0V), ou em outras palavras, dizer se o pino tem estado lógico 1 ou 0 (ou ainda HIGH/LOW).*/
    }
}
void parar()
{ 
    s=0;
    tf=millis();
    long dist=abs(tf-ti);
    //Serial.println("s");
    //Serial.println(s);
    //Serial.println(tf);
    //Serial.println(dist);
    
    int result;
    if(dist<=70000)
    {
        result=30;
    }
  
    if(dist>70000)
    {
        result=60;
    }
    
    Serial.print("Distancia do segmento: ");
    Serial.println (result);
    
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,LOW);
    analogWrite(velocidadeA,0);
    analogWrite(velocidadeB,0);
    delay(300);
}
void posicionar()
{
  digitalWrite(IN1,HIGH);//avançar
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);//avançar
  digitalWrite(IN4,LOW);
  analogWrite(velocidadeA,83);
  analogWrite(velocidadeB,80);
  delay(120);
}
void esquerda()
{
    e = 0;
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);
    analogWrite(velocidadeA, 95); //75 e 77
    analogWrite(velocidadeB,93);
    delay(350);    
    parar();
    ler_sensor();
    while(sensor[2] == 0)
    {
      ler_sensor();
      direita_correcao();
      parar();
    }
}
void direita()
{
    d = 0;
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,HIGH);
    analogWrite(velocidadeA, 95); //75 e 77
    analogWrite(velocidadeB,93);
    delay(350);
    parar();
    ler_sensor();
    while(sensor[2] == 0)
    {
      ler_sensor();
      esquerda_correcao();
      parar(); 
    }
}
void esquerda_correcao()
{
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);
    analogWrite(velocidadeA, 83); //75 e 77
    analogWrite(velocidadeB,81);
    delay(100);
    parar();
    ler_sensor();
}
void direita_correcao()
{
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,HIGH);
    analogWrite(velocidadeA, 83); //75 e 77
    analogWrite(velocidadeB,81);
    delay(100);
    parar();
    ler_sensor();
}
